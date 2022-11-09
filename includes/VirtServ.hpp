#ifndef VIRTSERV_HPP
#define VIRTSERV_HPP

#include <netinet/in.h>
#include <string>
#include <vector>

struct VirtServ
{
	std::string m_server_name;
	std::string m_root;
	std::string m_index;
	std::vector<int> ports;
	std::vector<sockaddr_in> m_sockaddr_vec;
};

std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo);

#endif /* VIRTSERV_HPP */
