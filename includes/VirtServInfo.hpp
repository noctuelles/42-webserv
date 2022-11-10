#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iterator>
#include <map>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "EPoll.hpp"
#include "FileUtils.hpp"
#include "HTTP.hpp"
#include "ListeningSocket.hpp"
#include "VirtServ.hpp"
#include "less_sockaddr.hpp"

namespace ft
{
	using std::vector;
	using std::map;

class VirtServInfo
{
  public:

	typedef vector<VirtServ>								VirtServVector;
	typedef vector<VirtServ*>								VirtServPtrVector;

	typedef std::map<sockaddr_in, VirtServPtrVector >		VirtServMap;
	typedef VirtServMap::iterator							iterator;

	std::vector<VirtServ>								m_virtserv_vec;
	VirtServMap											m_virtserv_map;

	/* Ctor */ VirtServInfo(const char* config);

	iterator	begin()	{	return m_virtserv_map.begin();	}
	iterator	end()	{	return m_virtserv_map.end();	}
	
	const vector<VirtServ*>&	operator[](const VirtServMap::key_type& key) const
	{
		return (m_virtserv_map.at(key));
	}

  private:
	typedef struct token_dispatch
	{
		std::string token;
		void (VirtServInfo::*parse_method)(std::istream_iterator< std::string >&);
	} token_dispatch_t;

	static const token_dispatch_t                m_block_dispatch_table[];
	static const token_dispatch_t                m_server_block_dispatch_table[];
	static const std::vector< token_dispatch_t > m_block_dispatch_vec;
	static const std::vector< token_dispatch_t > m_server_block_dispatch_vec;

	void _parseConfig(const char* config);
	void _match(std::istream_iterator< std::string >& it, const std::vector< token_dispatch_t >& dispatch_table, bool throw_on_not_found);
	void _parseServerBlock(std::istream_iterator< std::string >& it);
	void _parseListen(std::istream_iterator< std::string >& it);
	void _parseRoot(std::istream_iterator< std::string >& it);
	void _parseIndex(std::istream_iterator< std::string >& it);
	void _parseServerName(std::istream_iterator< std::string >& it);
};
} // namespace ft

#endif /* CONFIGPARSER_HPP */
