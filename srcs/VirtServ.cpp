#include "VirtServ.hpp"

#include <sstream>


std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo)
{
	os	<< "VIRTUAL SERVER: \n" 
		<< "\tserver_name: " << servinfo.m_server_name << '\n'
		<< "\troot: " << servinfo.m_root << '\n'
		<< "\tindex: " << servinfo.m_index << '\n';
	return os;
}


