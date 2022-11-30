/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/14 16:11:40 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/30 11:59:59 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "ConnectionSocket.hpp"
#include "Utils.hpp"
#include "WebServ.hpp"
#include "FieldParser.hpp"
#include "Log.hpp"
#include <ios>
#include <utility>
#include <vector>
#include <algorithm>
#include <iostream>
#include "StatusInfoPages.hpp"

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
		m_bounded_sock(),
		m_data_buff(IO::ConnectionSocket::MaxSendBufferSize),
		m_data_to_send(NULL),
		m_data_to_send_size(0),
		m_page_to_send(),
		m_file_handle(),
		m_ofile_handle(),
		m_header_parser(),
		m_header_info(),
		m_multipart_handler(NULL),
		m_status_code(OK),
		m_ressource_path()
	{
	}

	/* We gather the data */

	RequestHandler::State	RequestHandler::fetchIncomingData(const Buffer& buff)
	{
		Buffer::const_iterator	it = buff.begin();

		try
		{
			if (_state(FETCHING_REQUEST_HEADER))
			{
				it = m_header_parser(buff, it);

				if (m_header_parser.getState() == HeaderParser::ST_DONE)
				{
					m_header_info = m_header_parser.get();
					_parseGeneralHeaderFields();

					m_ressource_path = m_header_info.uri.absolute_path;
					m_ressource_path.erase(0, m_route->m_location_match.length());
					m_ressource_path.insert(0, m_route->m_root);

					::Log().get(INFO) << "Req. line " << '"' << getRequestLine() << '"' << " -> " << '"' << m_ressource_path << '"' << '\n';

					(this->*m_method_init_fnct[m_header_info.method])();

					_setState(FETCHING_REQUEST_BODY);
				}
			}
			if (_state(FETCHING_REQUEST_BODY))
			{
				if (m_header_info.method == Post)
				{
					if (m_request_type == FILE_UPLOAD)
					{
						(*m_multipart_handler)(buff, it);

						if (m_multipart_handler->getState() == MultiPartHandler::ST_DONE)
						{
							delete (m_multipart_handler);
							_setState(PROCESSING_RESPONSE_HEADER);
						}
					}
				}
				else
					_setState(PROCESSING_RESPONSE_HEADER);
			}
		}
		catch (const RequestHandler::Exception& e)
		{
			m_request_type = ERROR;
			_setErrorState(PROCESSING_RESPONSE_HEADER, e.what());

			if (m_virtserv)
			{
				// Trying to find a custom error pages defined by the user...
				const std::map<StatusCode, std::string>::const_iterator	custom_page = m_virtserv->m_error_page_map.find(e.what());

				// A custom page exists...
				if (custom_page != m_virtserv->m_error_page_map.end())
				{
					// See if the redirect file exist, to avoid multiple redirection.
					std::string	redirect = custom_page->second;

					m_ressource_path = redirect;
					m_route = &Utils::findRoute(redirect, *m_virtserv);
					redirect.erase(0, m_route->m_location_match.length());
					redirect.insert(0, m_route->m_root);

					try
					{
						mode_t	file_mode = IO::getFileMode(redirect.c_str());

						if ((file_mode & S_IFMT) & S_IFREG && (file_mode & S_IRUSR))
						{
							m_request_type = REDIRECT_ERROR;
							_setErrorState(PROCESSING_RESPONSE_HEADER, SeeOther);
						}
					}
					catch (const RequestHandler::Exception& er)
					{}
				}
			}
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

			m_page_to_send.assign(respHeader.toString());

			m_data_to_send = m_page_to_send.data();
			m_data_to_send_size = m_page_to_send.size();

			_setState(PROCESSING_RESPONSE_BODY);
		}
		else if (_state(PROCESSING_RESPONSE_BODY))
			(this->*m_method_send_fnct[m_header_info.method])();
		return (m_state);
	}

	void	RequestHandler::setConnectionBoundedSocket(const struct sockaddr_in& bounded_sock)
	{
		m_bounded_sock = bounded_sock;
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
		return (m_header_info.uri.absolute_path);
	}

	RequestHandler::~RequestHandler()
	{
		if (!m_multipart_handler)
			delete (m_multipart_handler);
	}

	/* ############################ Private function ############################ */

	const vector<VirtServ*>&	RequestHandler::_getBoundedVirtServ()
	{
		VirtServInfo::VirtServMap::const_iterator	it = m_virtserv_map.find(m_bounded_sock);

		if (it != m_virtserv_map.end())
			return (it->second);
		else
		{
			sockaddr_in	tmp_sock = m_bounded_sock;

			tmp_sock.sin_addr.s_addr = INADDR_ANY;
			return (m_virtserv_map.at(tmp_sock));
		}
	}


	void	RequestHandler::_parseGeneralHeaderFields()
	{
		// First, get the correct virtual server by parsing the Host field.
		{
			const vector<VirtServ*>&				virt_serv	=  _getBoundedVirtServ();
			const string&							host		= m_header_info.header_field.at(Field::Host());
			const vector<VirtServ*>::const_iterator	it			= std::find_if(virt_serv.begin(), virt_serv.end(), MatchingServerName(host));

			if (it != virt_serv.end())
				m_virtserv = *it;
			else
				m_virtserv = *virt_serv.begin();
		}

		m_route = &Utils::findRoute(m_header_info.uri.absolute_path, *m_virtserv);

		if (!m_route->m_methods[m_header_info.method])
			throw (Exception(MethodNotAllowed));
	}
}
