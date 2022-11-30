#ifndef VIRTSERV_HPP
#define VIRTSERV_HPP

#include <cstddef>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <bitset>

#include "Http.hpp"

using std::string;
using std::vector;
using std::map;
using std::bitset;

using std::string;
using std::vector;
using std::pair;

struct VirtServ
{
	struct RouteOptions 
	{
		RouteOptions(string uri) : m_location_match(uri)
			, m_methods(0xFFFFFFFF)
			, m_upload_store()
			, m_root()
			, m_index_vec()
			, m_autoindex(-1)
			, m_cgi_extension()
		{}

		bool	operator<(const RouteOptions& rhs)
		{
			return (m_location_match.length() < rhs.m_location_match.length());
		}

		string							m_location_match;
		bitset<HTTP::NbrAvailableMethod>	m_methods;
		string							m_upload_store;
		string							m_root;
		vector<string>					m_index_vec;
		short							m_autoindex;
		pair<string, string>			m_cgi_extension;
	};

	VirtServ() : m_default_route_options("/"), m_max_body_size(), m_routes_vec(), m_sockaddr_vec(), m_server_name_vec() {}

	RouteOptions					m_default_route_options;
	size_t							m_max_body_size;
	vector<RouteOptions>			m_routes_vec;
	vector<sockaddr_in>				m_sockaddr_vec;
	vector<string>					m_server_name_vec;
	map<HTTP::StatusCode, string>	m_error_page_map;
};
 
std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo);

#endif /* VIRTSERV_HPP */
