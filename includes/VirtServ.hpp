#ifndef VIRTSERV_HPP
#define VIRTSERV_HPP

#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#include "Http.hpp"

namespace ft {

using std::string;
using std::vector;
using std::map;
using std::bitset;

struct VirtServ
{
	struct RouteOptions 
	{
		RouteOptions(string uri) : m_uri(uri) , m_autoindex(-1)
		{}

		bitset<http::NbrAvailableMethod>	m_methods;
		string							m_cgi_extension;
		string							m_root;
		string							m_uri;
		vector<string>					m_index_vec;
		map<http::StatusCode, string>	m_error_page_map;
		short							m_autoindex;
	};

	vector<string>					m_server_name_vec;
	string							m_root;
	vector<string>					m_index_vec;
	vector<sockaddr_in>				m_sockaddr_vec;
	vector<RouteOptions>			m_routes_vec;
	map<http::StatusCode, string>	m_error_page_map;
	bool							m_autoindex;
	int								m_max_body_size;
	string							m_cgi_extension;
};

} // namespace ft
  
std::ostream& operator<<(std::ostream& os, const ft::VirtServ& servinfo);

#endif /* VIRTSERV_HPP */
