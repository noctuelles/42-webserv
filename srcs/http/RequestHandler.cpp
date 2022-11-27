/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 16:11:40 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/24 14:09:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "Utils.hpp"
#include "VirtServInfo.hpp"
#include "WebServ.hpp"
#include "Log.hpp"
#include "ConnectionSocket.hpp"
#include <arpa/inet.h>
#include <ios>
#include <netinet/in.h>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using std::make_pair;
using std::vector;
using std::string;

namespace HTTP
{
	const RequestHandler::methodInitFnct		RequestHandler::m_method_init_fnct[NbrAvailableMethod] = 
	{
		&RequestHandler::_methodInitGet,
		&RequestHandler::_methodInitPost,
		&RequestHandler::_methodInitDelete
	};

	const RequestHandler::methodHeaderFnct	RequestHandler::m_method_header_fnct[NbrAvailableMethod + 1] = 
	{
		&RequestHandler::_methodHeaderGet,
		&RequestHandler::_methodHeaderPost,
		&RequestHandler::_methodHeaderDelete,
		&RequestHandler::_methodHeaderError
	};

	const RequestHandler::methodSendFnct		RequestHandler::m_method_send_fnct[NbrAvailableMethod + 1] = 
	{
		&RequestHandler::_methodSendGet,
		&RequestHandler::_methodSendPost,
		&RequestHandler::_methodSendDelete,
		&RequestHandler::_methodSendError
	};

	RequestHandler::RequestHandler(const VirtServInfo::VirtServMap& virt_serv_map) :
		m_state(FETCHING_REQUEST_HEADER),
		m_request_type(FILE),
		m_virtserv_map(virt_serv_map),
		m_virtserv(NULL),
		m_route(NULL),
		m_bound_sock(),
		m_peer_sock(),
		m_data_buff(IO::ConnectionSocket::MaxSendBufferSize),
		m_data_to_send(NULL),
		m_data_to_send_size(0),
		m_page_to_send(),
		m_cgi_input_pipe(),
		m_cgi_output_pipe(),
		m_file_handle(),
		m_header_info(),
		m_uri_info(),
		m_header_parser(m_header_info, m_uri_info),
		m_status_code(OK),
		m_ressource_path(),
		m_cgi_handler()
	{
	}

	/* We gather the data */

	RequestHandler::State	RequestHandler::fetchIncomingData(const vector<uint8_t>& data_buff, size_t recv_bytes)
	{
		try
		{
			if (_state(FETCHING_REQUEST_HEADER))
			{
				if (m_header_parser.parseHeader(data_buff, recv_bytes))
				{
					_parseGeneralHeaderFields();

					::Log().get(INFO) << "Req. line " << '"' << getRequestLine() << '"' << '\n';

					m_ressource_path = m_uri_info.absolute_path;
					m_ressource_path.insert(0, m_route->m_root);

					(this->*m_method_init_fnct[m_header_info.method])();

					_setState(FETCHING_REQUEST_BODY);
				}
			}
			if (_state(FETCHING_REQUEST_BODY))
			{
				if (m_header_info.method != Get)
				{
				}
				else
					_setState(PROCESSING_RESPONSE_HEADER);
			}
		}
		catch (const Exception& e)
		{
			::Log().get(WARNING) << "Issuing a " << e.what() << " HTTP error.\n";
			_setErrorState(PROCESSING_RESPONSE_HEADER, e.what());
		}
		return (m_state);
	}

	RequestHandler::State	RequestHandler::prepareOutcomingData()
	{
		if (_state(PROCESSING_RESPONSE_HEADER))
		{
			ResponseHeader	respHeader;

			respHeader.addField(Field::Date(), Utils::getRFC822NowDate());
			respHeader.addField(Field::Server(), WebServ::Version);
			respHeader.addField(Field::Connection(), "closed");

			(this->*m_method_header_fnct[m_header_info.method])(respHeader);

			std::memcpy(m_data_buff.data(), respHeader.toCString(), respHeader.size()); // changed later.
			m_data_to_send = m_data_buff.data();
			m_data_to_send_size = respHeader.size();

			_setState(PROCESSING_RESPONSE_BODY);
		}
		else if (_state(PROCESSING_RESPONSE_BODY))
			(this->*m_method_send_fnct[m_header_info.method])();
		return (m_state);
	}

	void	RequestHandler::setBoundInterface(const struct sockaddr_in& bound_sock)
	{
		stringstream ss;
		m_bound_sock = bound_sock;
		ss << ntohs(bound_sock.sin_port);
		m_server_port_str = ss.str();
	}

	void	RequestHandler::setPeerInterface(const struct sockaddr_in& peer_sock)
	{
		m_peer_sock = peer_sock;
		m_remote_addr_str = inet_ntoa(peer_sock.sin_addr);
	}

	RequestHandler::DataInfo	RequestHandler::getDataToSend() const
	{
		return (make_pair(m_data_to_send, m_data_to_send_size));
	}

	StatusCode	RequestHandler::getStatusCode() const
	{
		return (m_status_code);
	}

	const string&	RequestHandler::getRequestLine() const
	{
		return (m_header_info.request_line);
	}

	const string&	RequestHandler::getAbsPath() const
	{
		return (m_uri_info.absolute_path);
	}

	/* ############################ Private function ############################ */

	const vector<VirtServ*>&	RequestHandler::_getBoundVirtServ()
	{
		VirtServInfo::VirtServMap::const_iterator	it = m_virtserv_map.find(m_bound_sock);

		if (it != m_virtserv_map.end())
			return (it->second);
		else
		{
			sockaddr_in	tmp_sock = m_bound_sock;

			tmp_sock.sin_addr.s_addr = INADDR_ANY;
			return (m_virtserv_map.at(tmp_sock));
		}
	}

	void	RequestHandler::_parseGeneralHeaderFields()
	{
		// First, get the correct virtual server by parsing the Host field.
		{
			const vector<VirtServ*>&				virt_serv	=  _getBoundVirtServ();
			const string&							host		= m_header_info.header_fields.at(Field::Host());
			const vector<VirtServ*>::const_iterator	it			= std::find_if(virt_serv.begin(), virt_serv.end(), MatchingServerName(host));

			if (it != virt_serv.end())
				m_virtserv = *it;
			else
				m_virtserv = *virt_serv.begin();
		}

		// Next, find if the request URI match any of the location block to find a route.
		{
			typedef vector<VirtServ::RouteOptions>::const_iterator	RouteOptionsIt;
			const vector<VirtServ::RouteOptions>&					routes = m_virtserv->m_routes_vec;
			vector<RouteOptionsIt>									matching_candidate;

			for (RouteOptionsIt it = routes.begin(); it != routes.end(); it++)
			{
				if (m_header_info.uri.compare(0, it->m_location_match.length(), it->m_location_match) == 0)
					matching_candidate.push_back(it);
			}

			// Select the longest matching location match.
			vector<RouteOptionsIt>::const_iterator	best_candidate = std::max_element(matching_candidate.begin(), matching_candidate.end());

			// Either our route vector was empty or no matching candidate
			if (best_candidate != matching_candidate.end())
				m_route = &**best_candidate;
			else
				m_route = &m_virtserv->m_default_route_options;
		}

		// Route, virtual server, has been found. We can now see if the method is supported in this route.
		{
			if (!m_route->m_methods[m_header_info.method])
				throw (Exception(MethodNotAllowed));
		}
	}
}
