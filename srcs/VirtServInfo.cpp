#include <arpa/inet.h>
#include <cctype>
#include <cstddef>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <netinet/in.h>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

#include "ClientSocket.hpp"
#include "VirtServInfo.hpp"
#include "FileUtils.hpp"
#include "HTTP.hpp"

namespace ft
{

using std::cout;
using std::ifstream;
using std::istream_iterator;
using std::map;
using std::string;
using std::vector;
using std::ostringstream;

/* STATIC DATA */

#define SIZE(x) sizeof(x) / sizeof(x[0])

const VirtServInfo::token_dispatch_t VirtServInfo::m_block_dispatch_table[] = {
	{"server", &VirtServInfo::_parseServerBlock},
	{"server{", &VirtServInfo::_parseServerBlock},
};

// Had to create a C array before because C++98 does not support uniform initialization
// Range constructor
const vector< VirtServInfo::token_dispatch_t > VirtServInfo::m_block_dispatch_vec(m_block_dispatch_table,
                                                             m_block_dispatch_table + SIZE(m_block_dispatch_table));

const VirtServInfo::token_dispatch_t VirtServInfo::m_server_block_dispatch_table[] = {
    {"listen", &VirtServInfo::_parseListen},
    {"root", &VirtServInfo::_parseRoot},
    {"index", &VirtServInfo::_parseIndex},
    {"server_name", &VirtServInfo::_parseServerName},
};

// Range constructor
const vector< VirtServInfo::token_dispatch_t > VirtServInfo::m_server_block_dispatch_vec(m_server_block_dispatch_table,
                                                                    m_server_block_dispatch_table +
                                                                        SIZE(m_server_block_dispatch_table));

#undef SIZE

/* METHODS */

#ifndef DEFAULT_CONFIG 
# define DEFAULT_CONFIG "webserv.conf"
#endif

/* Ctor */ VirtServInfo::VirtServInfo(const char* config)
{
	std::ifstream ifs;
	if (config != NULL)
	{
		ifs.open(config);
		if (not ifs)
			throw std::runtime_error("Could not open config file passed in paramater");
	}
	else
	{
		ifs.open(DEFAULT_CONFIG);
		if (not ifs)
			throw std::runtime_error("Could not open defaut config file: " DEFAULT_CONFIG );
	}

	istream_iterator< string > stream_it(ifs);
	istream_iterator< string > eos;

	// Where the actual parsing takes place
	while (stream_it != eos)
		_match(stream_it, m_block_dispatch_vec, true);

	// Copy the virtserv addresses into a map<sockaddr_in, vec_of_virtserv_refs>
	vector< VirtServ >::iterator	virtserv		= m_virtserv_vec.begin();
	vector< VirtServ >::iterator	end				= m_virtserv_vec.end();
	for (; virtserv != end; ++virtserv)
	{
		vector< sockaddr_in >&			m_sockaddr_vec	= virtserv->m_sockaddr_vec;
		vector< sockaddr_in >::iterator sockaddr		= m_sockaddr_vec.begin();
		vector< sockaddr_in >::iterator vec_end			= m_sockaddr_vec.end();
		for (; sockaddr != vec_end; ++sockaddr)
		{
			m_virtserv_map[*sockaddr].push_back(&(*virtserv));
		}
	}
#ifdef DEBUG // Debugging output 
	{
		map< sockaddr_in, vector< VirtServ* > >::iterator it  = m_virtserv_map.begin();
		map< sockaddr_in, vector< VirtServ* > >::iterator end = m_virtserv_map.end();
		for (; it != end; ++it)
		{
			vector< VirtServ* >&          virtserv_vec = it->second;
			vector< VirtServ* >::iterator vit          = virtserv_vec.begin();
			vector< VirtServ* >::iterator vend         = virtserv_vec.end();
			cout << "=========================================\n";
			cout << "ON " << (long)(it->first.sin_addr.s_addr) <<":"<< it->first.sin_port << '\n';
			cout << "=========================================\n";
			for (; vit != vend; ++vit)
			{
				cout << **vit << '\n';
			}
			cout << "_________________________________________\n";
		}
	}
#endif
}

#define BEFORE false
#define AFTER true

static void check_delim(istream_iterator<string>& it, char delim, string info, bool position)
{
	istream_iterator<string> eos;
	if (it != eos and  *(it->rbegin()) == delim)
	{
		++it;
		return;
	}
	++it;
	if (it != eos and *it == string(1, delim))
	{
		++it;
		return;
	}
	ostringstream ss;
	ss << "Configuration file error: missing '" << delim;
	if (position == BEFORE)
		ss << "' before ";
	else
		ss <<"' after " ;
	ss << info << " directive\n";
	throw std::runtime_error(ss.str());
}

void VirtServInfo::_match(istream_iterator<string>& it, const vector<token_dispatch_t>& dispatch_table, bool throw_on_not_found = false)
{
	istream_iterator<string> eos;
	size_t i;
	size_t table_size = dispatch_table.size();
	for ( i = 0;  i < table_size and it != eos; ++i )
	{
		if ( (*it).compare(dispatch_table[i].token) == 0)
		{
			(this->*(dispatch_table[i].parse_method))(it); 
			i = -1;
		}
	}
	if (throw_on_not_found and i == table_size)
	{
		throw std::runtime_error("Config file error: Unknown token");
	}
}

void VirtServInfo::_parseServerBlock(istream_iterator<string>& it)
{
	m_virtserv_vec.push_back(VirtServ());
	check_delim(it, '{', "server", BEFORE);
	_match(it, m_server_block_dispatch_vec);
	check_delim(it, '}', "server", AFTER);
}

// Note PAUL: un port peut etre 0 <-> 65565. A port is coded on 16 bits.
static int xatoi(const string& str)
{
	string::const_iterator it = str.begin();
	string::const_iterator end = str.end();
	for (--end; it != end ; ++it)
		if ( not std::isdigit(*it) )
			throw std::runtime_error("Config file error: invalid port in listen directive ");
	if (*it != ';')
			throw std::runtime_error("Config file error: invalid port in listen directive ");
	return ::atoi(str.c_str());
}

void VirtServInfo::_parseListen(istream_iterator<string>& it)
{
	++it;
	const string& host_port = *it;
	string host;
	string port;
	//size_t colon = it->rfind(':'); 
	size_t colon = host_port.find_last_of(":."); 
	if ( colon == string::npos ) // No colons, then only port information
	{
		host.assign("0");
		port.assign(host_port);
	}
	else if ( host_port[colon] == ':' ) // Is of the form host:port 
	{
		host.assign(*it, 0, colon);
		port.assign(*it, colon + 1, string::npos);
	}
	else if ( host_port[colon] == '.' ) // Only dot-style ip adress
	{
		host.assign(host_port);
		port.assign("80");
	}
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(host.c_str());
	if ( sockaddr.sin_addr.s_addr == INADDR_NONE )
		throw std::runtime_error("Config file error: listen directive does not have a valid ip address");
	// I'm happy now.
	sockaddr.sin_port = htons(xatoi(port.c_str()));

	m_virtserv_vec.back().m_sockaddr_vec.push_back(sockaddr);
	check_delim(it, ';', "listen", AFTER);
}

void VirtServInfo::_parseServerName(istream_iterator<string>& it)
{
	++it;
	m_virtserv_vec.back().m_server_name = *it;
	m_virtserv_vec.back().m_server_name.erase(m_virtserv_vec.back().m_server_name.end() - 1);
	check_delim(it, ';', "server_name", AFTER);
}

void VirtServInfo::_parseRoot(istream_iterator<string>& it)
{
	++it;
	m_virtserv_vec.back().m_root = *it;
	m_virtserv_vec.back().m_root.erase(m_virtserv_vec.back().m_root.end() - 1);
	check_delim(it, ';', "root", AFTER);
}

void VirtServInfo::_parseIndex(istream_iterator<string>& it)
{
	++it;
	m_virtserv_vec.back().m_index = *it;
	m_virtserv_vec.back().m_index.erase(m_virtserv_vec.back().m_index.end() - 1);
	check_delim(it, ';', "index", AFTER);
}

#undef BEFORE 
#undef AFTER

} // namespace ft
