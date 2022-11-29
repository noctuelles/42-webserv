#include "VirtServ.hpp"
#include "Http.hpp"

#include <sstream>
#include <vector>
#include <string>
#include <map>

using std::ostream;
using std::vector;
using std::string;
using std::map;

std::ostream& operator<<(std::ostream& os, vector<string>& vec)
{
	for (size_t i = 0; i < vec.size() ; ++i)
	{
		os << vec[i] << "\t";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const VirtServ::RouteOptions& routeinfo)
{
	os << "\t\t-------------------------\n";
	os	<< "\t\turi:\t\t" ;
	os << routeinfo.m_location_match;
	os << '\n';

	os	<< "\t\troot:\t\t" ;
	os << routeinfo.m_root;
	os << '\n';

	os	<< "\t\tautoindex:\t";
	os << (bool) routeinfo.m_autoindex;
	os << '\n';

	os	<< "\t\tindex:\t\t";
	for (size_t i = 0; i < routeinfo.m_index_vec.size(); ++i)
	{
		os << routeinfo.m_index_vec[i] << '\t';
	}
	os << '\n';

	os	<< "\t\tmethods:\t";
	os << (routeinfo.m_methods.test(HTTP::Get) ? " GET " : "");
	os << (routeinfo.m_methods.test(HTTP::Post) ? " POST " : "");
	os << (routeinfo.m_methods.test(HTTP::Delete) ? " DELETE " : "");
	os << '\n';

	os	<< "\t\tcgi_setup:\t";
	os << routeinfo.m_cgi_extension;
	os << '\n';

	os	<< "\t\tupload_store:\t";
	os << routeinfo.m_upload_store.first << " " << routeinfo.m_upload_store.second;
	os << '\n';

	os << "\t\t-------------------------\n";

	return os;
}

std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo)
{
	os	<< "VIRTUAL SERVER: \n";

	os	<< "\tserver_name:\t";
	for (size_t i = 0; i < servinfo.m_server_name_vec.size(); ++i)
	{
		os << servinfo.m_server_name_vec[i] << '\t';
	}
	os << '\n';

	os	<< "\tmax_body_size:\t\t" ;
	os << servinfo.m_max_body_size << '\t';
	os << '\n';

	os	<< "\terror_page:\t\t";
	for (map<HTTP::StatusCode, string>::const_iterator it = servinfo.m_default_route_options.m_error_page_map.begin();
			it != servinfo.m_default_route_options.m_error_page_map.end(); ++it)
	{
		os << it->first <<": "<< it->second << '\t';
	}
	os << '\n';

	os	<< "\troot:\t\t" ;
	os << servinfo.m_default_route_options.m_root << '\t';
	os << '\n';

	os	<< "\tautoindex:\t\t" ;
	os << (bool) servinfo.m_default_route_options.m_autoindex << '\t';
	os << '\n';

	os	<< "\tindex:\t\t";
	for (size_t i = 0; i < servinfo.m_default_route_options.m_index_vec.size(); ++i)
	{
		os << servinfo.m_default_route_options.m_index_vec[i] << '\t';
	}
	os << '\n';

	os	<< "\tmethods:\t";
	os << (servinfo.m_default_route_options.m_methods.test(HTTP::Get) ? " GET " : "");
	os << (servinfo.m_default_route_options.m_methods.test(HTTP::Post) ? " POST " : "");
	os << (servinfo.m_default_route_options.m_methods.test(HTTP::Delete) ? " DELETE " : "");
	os << '\n';

	os	<< "\tcgi_setup:\t";
	os << servinfo.m_default_route_options.m_cgi_extension;
	os << '\n';

	os	<< "\tupload_store:\t";
	os << servinfo.m_default_route_options.m_upload_store.first << " " << servinfo.m_default_route_options.m_upload_store.second;
	os << '\n';

	os	<< "\tlocation blocks:\n";
	for (size_t i = 0; i < servinfo.m_routes_vec.size(); ++i)
	{
		os << servinfo.m_routes_vec[i] << '\t';
	}
	os << '\n';

	return os;
}
