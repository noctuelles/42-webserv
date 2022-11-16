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

using std::string;
using std::vector;

struct VirtServ
{
	struct RouteOptions 
	{
		RouteOptions(string uri) : m_location_match(uri) , m_autoindex(-1)
		{
			m_index_vec.push_back("index.html");
		}

		bool	operator<(const RouteOptions& rhs)
		{
			return (m_location_match.length() < rhs.m_location_match.length());
		}

		bitset<http::NbrAvailableMethod>	m_methods;
		string							m_cgi_extension;
		string							m_root;
		string							m_location_match;
		vector<string>					m_index_vec;
		map<http::StatusCode, string>	m_error_page_map;
		short							m_autoindex;
	};

	VirtServ() : m_default_route_options("/"), m_max_body_size(), m_routes_vec(), m_sockaddr_vec(), m_server_name_vec() {}

	RouteOptions					m_default_route_options;
	int								m_max_body_size;
	vector<RouteOptions>			m_routes_vec;
	vector<sockaddr_in>				m_sockaddr_vec;
	vector<string>					m_server_name_vec;
};

} // namespace ft
  
std::ostream& operator<<(std::ostream& os, const ft::VirtServ& servinfo);

#endif /* VIRTSERV_HPP */
