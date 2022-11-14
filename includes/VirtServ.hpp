#ifndef VIRTSERV_HPP
#define VIRTSERV_HPP

#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <vector>

struct VirtServ
{
	struct RouteOptions 
	{
		RouteOptions(std::string uri) : m_uri(uri)
			, m_root()
			, m_autoindex(-1)
		{}

		std::string m_uri;
		std::string m_root;
		int m_autoindex;
	};

	std::vector<std::string> m_server_name_vec;
	std::string m_root;
	std::vector<std::string> m_index_vec;
	std::vector<sockaddr_in> m_sockaddr_vec;
	std::vector<RouteOptions> m_routes_vec;
	bool m_autoindex;
};

std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo);

#endif /* VIRTSERV_HPP */
