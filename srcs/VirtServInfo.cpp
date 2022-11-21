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
#include <stack>

#include "VirtServ.hpp"
#include "VirtServInfo.hpp"
#include "Utils.hpp"

namespace ft
{

using std::ifstream;
using std::string;
using std::vector;
using std::stack;
using std::ostringstream;
using std::cout;

/** STATIC DATA **/

#define SIZE(x) sizeof(x) / sizeof(x[0])

	/* Main context tokens */

const VirtServInfo::token_dispatch_t VirtServInfo::m_block_dispatch_table[] = {
	{"server", &VirtServInfo::_parseServerBlock},
};

// Had to create a C array before because C++98 does not support uniform initialization
// Range constructor
const vector< VirtServInfo::token_dispatch_t > VirtServInfo::m_block_dispatch_vec(m_block_dispatch_table,
                                                             m_block_dispatch_table + SIZE(m_block_dispatch_table));

	/* Server context tokens */

const VirtServInfo::token_dispatch_t VirtServInfo::m_server_block_dispatch_table[] = {
    {"location", &VirtServInfo::_parseLocationBlock},
    {"listen", &VirtServInfo::_parseListen},
    {"root", &VirtServInfo::_parseRoot},
    {"index", &VirtServInfo::_parseIndex},
    {"server_name", &VirtServInfo::_parseServerName},
    {"autoindex", &VirtServInfo::_parseAutoindex},
    {"error_page", &VirtServInfo::_parseErrorPage},
    {"client_max_body_size", &VirtServInfo::_parseClientMaxBodySize},
    {"cgi_setup", &VirtServInfo::_parseCgiSetup},
};

// Range constructor
const vector< VirtServInfo::token_dispatch_t > VirtServInfo::m_server_block_dispatch_vec(m_server_block_dispatch_table,
                                                                    m_server_block_dispatch_table +
                                                                        SIZE(m_server_block_dispatch_table));
	/* Location context tokens */

const VirtServInfo::token_dispatch_t VirtServInfo::m_location_block_dispatch_table[] = {
    {"root", &VirtServInfo::_parseLocationRoot},
    {"autoindex", &VirtServInfo::_parseLocationAutoindex},
    {"index", &VirtServInfo::_parseLocationIndex},
    {"limit_except", &VirtServInfo::_parseLocationLimitExcept},
    {"cgi_setup", &VirtServInfo::_parseLocationCgiSetup},
};

// Range constructor
const vector< VirtServInfo::token_dispatch_t > VirtServInfo::m_location_block_dispatch_vec(m_location_block_dispatch_table,
                                                                    m_location_block_dispatch_table +
                                                                        SIZE(m_location_block_dispatch_table));

#undef SIZE

/** METHODS **/

#ifndef DEFAULT_CONFIG 
# define DEFAULT_CONFIG "webserv.conf"
#endif

/* Ctor */ VirtServInfo::VirtServInfo(const char* config)
{
	// Init stream
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

	VirtServInfo::configstream_iterator it(ifs);

	// Where the actual parsing takes place
	while (ifs)
		_match(it, m_block_dispatch_vec, true);

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
		cout << std::boolalpha;
		for (; it != end; ++it)
		{
			vector< VirtServ* >&          virtserv_vec = it->second;
			vector< VirtServ* >::iterator vit          = virtserv_vec.begin();
			vector< VirtServ* >::iterator vend         = virtserv_vec.end();
			cout << "=========================================\n";
			cout << "ON " << inet_ntoa(it->first.sin_addr) <<":"<< ntohs(it->first.sin_port) << '\n';
			cout << "=========================================\n";
			for (; vit != vend; ++vit)
			{
				cout << **vit;
			}
			cout << "_________________________________________\n";
		}
	}
#endif
}

#define BEFORE false
#define AFTER true

void VirtServInfo::_match(VirtServInfo::configstream_iterator& it, const vector<token_dispatch_t>& dispatch_table, bool throw_on_not_found = false)
{
	size_t i;
	size_t table_size = dispatch_table.size();
	for ( i = 0;  i < table_size and not it->empty(); ++i )
	{
		if ( it->compare(dispatch_table[i].token) == 0)
		{
			(this->*(dispatch_table[i].parse_method))(it); 
			i = -1;
		}
	}
	if (*it == ";")
	{
		throw ConfigFileError("Empty directive");
	}
	else if (throw_on_not_found and i == table_size)
	{
		throw ConfigFileError("Unknown token");
	}
}

// Each _parse* function is responsible for consuming the terminating delimiter

#ifndef DEFAULT_REQUEST_MAX_BODY_SIZE
# define DEFAULT_REQUEST_MAX_BODY_SIZE 1024*1024
#endif

void VirtServInfo::_parseServerBlock(VirtServInfo::configstream_iterator& it)
{
	++it;
	// Check begin delimiter
	if (*it != "{")
		throw ConfigFileError("Invalid token after server block directive: Missing '{' maybe ?");
	++it;
	// Add a new virtual server
	m_virtserv_vec.push_back(VirtServ());
	// Parse virtual server data
	_match(it, m_server_block_dispatch_vec);
	// Makes undefined data in routes_options depend inherit from the server block
	VirtServ& vs= m_virtserv_vec.back();
	if (vs.m_sockaddr_vec.empty())
	{
		sockaddr_in sockaddr;
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = INADDR_ANY;
		sockaddr.sin_port = htons(80);
		vs.m_sockaddr_vec.push_back(sockaddr);
	}
	if (vs.m_max_body_size == 0)
	{
		vs.m_max_body_size = DEFAULT_REQUEST_MAX_BODY_SIZE;
	}
	if (vs.m_default_route_options.m_autoindex == -1)
	{
		vs.m_default_route_options.m_autoindex = 0;
	}
	if (vs.m_default_route_options.m_index_vec.empty())
	{
		vs.m_default_route_options.m_index_vec.push_back("index.html");
	}
	for (vector<VirtServ::RouteOptions>::iterator it = vs.m_routes_vec.begin()
			; it != vs.m_routes_vec.end()
			;	++it )
	{
		if (it->m_autoindex == -1)
			 it->m_autoindex = vs.m_default_route_options.m_autoindex;
		if (it->m_index_vec.empty())
			 it->m_index_vec.push_back("index.html");
	}
	// Check for end delimiter
	if (*it != "}")
		throw ConfigFileError("Invalid token in server block: Forgot '}' maybe ?");
	++it;
}
#undef DEFAULT_REQUEST_MAX_BODY_SIZE

void VirtServInfo::_parseAutoindex(VirtServInfo::configstream_iterator& it)
{
	// Can only have one argument
	++it;
	if ( *it == "on")
		m_virtserv_vec.back().m_default_route_options.m_autoindex = 1;
	else if ( *it == "off")
		m_virtserv_vec.back().m_default_route_options.m_autoindex = 0;
	else
		throw ConfigFileError("Invalid argument for autoindex directive in server block: Can only be \"on\" on \"off\"");
	++it;
	if (*it != ";")
		throw ConfigFileError("missing ; after autoindex directive in server block");
	else
		++it;
}

void VirtServInfo::_parseLocationAutoindex(VirtServInfo::configstream_iterator& it)
{
	// Can only have one argument
	++it;
	if ( *it == "on")
		m_virtserv_vec.back().m_routes_vec.back().m_autoindex = true;
	else if ( *it == "off")
		m_virtserv_vec.back().m_routes_vec.back().m_autoindex = false;
	else
		throw ConfigFileError("Invalid argument for autoindex directive in location block: Can only be \"on\" on \"off\"");
	++it;
	if (*it != ";")
		throw ConfigFileError("missing ; after autoindex directive in location block");
	else
		++it;
}

void VirtServInfo::_parseLocationRoot(VirtServInfo::configstream_iterator& it)
{
	// Can only have one argument
	if ( not m_virtserv_vec.back().m_routes_vec.back().m_root.empty() )
		throw ConfigFileError("Two root directives are defined in the same location block");
	++it;
	m_virtserv_vec.back().m_routes_vec.back().m_root = *it;
	++it;
	if (*it != ";")
		throw ConfigFileError("missing ; after root directive in location block");
	else
		++it;
}

void VirtServInfo::_parseLocationIndex(VirtServInfo::configstream_iterator& it)
{
	for (++it; not it.is_delim() ; ++it)
		m_virtserv_vec.back().m_routes_vec.back().m_index_vec.push_back(*it);
	if (*it != ";")
		throw ConfigFileError("missing ; after index directive in location block");
	else
		++it;
}

void VirtServInfo::_parseLocationLimitExcept(VirtServInfo::configstream_iterator& it)
{
	for (++it; not it.is_delim() ; ++it)
	{
		if ( *it == "GET" )
			m_virtserv_vec.back().m_routes_vec.back().m_methods.set(http::Get);
		else if ( *it == "POST" )
			m_virtserv_vec.back().m_routes_vec.back().m_methods.set(http::Post);
		else if ( *it == "DELETE" )
			m_virtserv_vec.back().m_routes_vec.back().m_methods.set(http::Delete);
		else
			throw ConfigFileError("Invalid or unsupported method for limit_except directive in location block: Supported methods are \"GET\", \"POST\" and \"DELETE\"");
	}
	if (*it != ";")
		throw ConfigFileError("missing ; after autoindex directive in location block");
	else
		++it;
}


void VirtServInfo::_parseLocationBlock(VirtServInfo::configstream_iterator& it)
{
	++it;
	m_virtserv_vec.back().m_routes_vec.push_back(VirtServ::RouteOptions(*it));
	++it;
	// Check begin delimiter
	if (*it != "{")
		throw ConfigFileError("Invalid token after location block directive: Missing '{' maybe ?");
	++it;
	// Parse location data
	_match(it, m_location_block_dispatch_vec);
	if (*it != "}")
		throw ConfigFileError("Invalid token in location block");
	++it;

	// Get parent root directive if none was defined.
	if ( m_virtserv_vec.back().m_routes_vec.back().m_root.empty() )
		m_virtserv_vec.back().m_routes_vec.back().m_root = m_virtserv_vec.back().m_default_route_options.m_root;
	// Get parent index directive if none was defined.
	if ( m_virtserv_vec.back().m_routes_vec.back().m_index_vec.empty() )
		m_virtserv_vec.back().m_routes_vec.back().m_index_vec = m_virtserv_vec.back().m_default_route_options.m_index_vec;
	// Get parent autoindex directive if none was defined.
	if ( m_virtserv_vec.back().m_routes_vec.back().m_autoindex == -1)
		m_virtserv_vec.back().m_routes_vec.back().m_autoindex = m_virtserv_vec.back().m_default_route_options.m_autoindex;
	// Get cgi_setup if none defined
	if ( m_virtserv_vec.back().m_routes_vec.back().m_cgi_extension.empty() )
		m_virtserv_vec.back().m_routes_vec.back().m_cgi_extension = m_virtserv_vec.back().m_default_route_options.m_cgi_extension;
	// Get error_page_map if none defined
	if ( m_virtserv_vec.back().m_routes_vec.back().m_error_page_map.empty() )
		m_virtserv_vec.back().m_routes_vec.back().m_error_page_map = m_virtserv_vec.back().m_default_route_options.m_error_page_map;
	// Put all methods to true if 0
	if ( m_virtserv_vec.back().m_routes_vec.back().m_methods == 0 )
		m_virtserv_vec.back().m_routes_vec.back().m_methods.set();
	// Check for end delimiter
}

void VirtServInfo::_parseLocationCgiSetup(VirtServInfo::configstream_iterator& it)
{
	// Can only have one argument
	++it;
	if ((*it)[0] != '.')
		throw ConfigFileError("cgi_setup directive in location block must start with a '.'");
	m_virtserv_vec.back().m_routes_vec.back().m_cgi_extension = *it;
	++it;
	if (*it != ";")
		throw ConfigFileError("missing ; after cgi_setup directive in location block");
	else
		++it;
}

static int xatoi(const string& str, const char *info)
{
	string::const_iterator it = str.begin();
	string::const_iterator end = str.end();
	for (--end; it != end ; ++it)
		if ( not std::isdigit(*it) )
			throw std::runtime_error(info);
	return ::atoi(str.c_str());
}

void VirtServInfo::_parseListen(VirtServInfo::configstream_iterator& it)
{
	// Has only one arg
	++it;
	const string& host_port = *it;
	string host;
	string port;
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
		throw ConfigFileError("listen directive does not have a valid ip address");
	sockaddr.sin_port = htons(xatoi(port.c_str(), "Config file error: invalid port in listen directive "));

	m_virtserv_vec.back().m_sockaddr_vec.push_back(sockaddr);

	++it;
	if ( *it != ";")
		throw ConfigFileError("missing ; after listen directive");
	else
		++it;
}

void VirtServInfo::_parseServerName(VirtServInfo::configstream_iterator& it)
{
	// Various args possible
	for (++it; not it.is_delim() ; ++it)
		m_virtserv_vec.back().m_server_name_vec.push_back(*it);
	if (*it != ";")
		throw ConfigFileError("missing ; after server_name directive");
	else
		++it;
}

void VirtServInfo::_parseRoot(VirtServInfo::configstream_iterator& it)
{
	// Can only have one argument
	++it;
	if (not m_virtserv_vec.back().m_default_route_options.m_root.empty() )
		throw ConfigFileError("Two root directives are defined in the same server block");
	m_virtserv_vec.back().m_default_route_options.m_root = *it;
	++it;
	if (*it != ";")
		throw ConfigFileError("missing ; after root directive in server block");
	else
		++it;
}

void VirtServInfo::_parseIndex(VirtServInfo::configstream_iterator& it)
{
	static bool cleared = false;
	if (not cleared)
		m_virtserv_vec.back().m_default_route_options.m_index_vec.clear(), cleared = true;
	// Various args possible
	for (++it; not it.is_delim() ; ++it)
		m_virtserv_vec.back().m_default_route_options.m_index_vec.push_back(*it);
	if (*it != ";")
		throw ConfigFileError("missing ; after index directive in server block");
	else
		++it;
}

//	 Check if it's a valid status code ? Pb not worth it
void VirtServInfo::_parseErrorPage(VirtServInfo::configstream_iterator& it)
{
	stack<http::StatusCode> codes;
	for (++it; not it.is_delim() and (*it)[0] != '/'; ++it)
		codes.push((http::StatusCode)xatoi(*it,"Config file error: Invalid status code in error_page directive in server block: Not a number literal"));
	if (it.is_delim())
		throw ConfigFileError("Missing path to default error page file at the end of error_page directive in server block");
	else if ((*it)[0] != '/')
		throw ConfigFileError("Invalid path at the end of error_page directive in server block");
	for (; not codes.empty(); codes.pop())
	    m_virtserv_vec.back().m_default_route_options.m_error_page_map[codes.top()] = *it;;
	++it;
	if (*it != ";")
		throw ConfigFileError("missing ; after error_page directive in server block");
	else
		++it;
}

void VirtServInfo::_parseClientMaxBodySize(VirtServInfo::configstream_iterator& it)
{
	// Can only have one argument
	++it;
	m_virtserv_vec.back().m_max_body_size = xatoi(*it, "Config file error: Invalid number after client_max_body_size directive in server block");
	++it;
	if (*it != ";")
		throw ConfigFileError("missing ; after client_max_body_size directive in server block");
	else
		++it;
}

void VirtServInfo::_parseCgiSetup(VirtServInfo::configstream_iterator& it)
{
	// Can only have one argument
	++it;
	m_virtserv_vec.back().m_default_route_options.m_cgi_extension = *it;
	++it;
	if (*it != ";")
		throw ConfigFileError("missing ; after cgi_setup directive in server block");
	else
		++it;
}

#undef BEFORE 
#undef AFTER

} // namespace ft
