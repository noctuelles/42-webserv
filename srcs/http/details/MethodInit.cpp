/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:28:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/24 14:34:05 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"
#include "Http.hpp"
#include "RequestHandler.hpp"
#include "FileUtils.hpp"
#include "WebServ.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/stat.h>

using std::find_if;

namespace HTTP
{
	void	RequestHandler::_methodInitGet()
	{
		using std::ios;
		using std::ifstream;

		if (*m_ressource_path.rbegin() == '/')
		{
			if (!m_route->m_autoindex)
			{
				const vector<string>&			index_vec		= m_route->m_index_vec;
				vector<string>::const_iterator	selected_file	= find_if(index_vec.begin(), index_vec.end(), ValidIndexFile(m_ressource_path));

				if (selected_file != index_vec.end())
					m_ressource_path.append(*selected_file);
				else
					throw (Exception(NotFound));
			}
			else // autoindex
			{
				m_request_type = AUTOINDEX;
				m_page_to_send = AutoIndex::generatePage(m_virtserv->m_routes_vec, *m_route, m_uri_info.absolute_path, m_ressource_path, m_uri_info.query);
			}
		}
		else
		{
			string::size_type	i = m_ressource_path.rfind('.');

			if (i != string::npos)
			{
				if (m_ressource_path.compare(i, 4, ".php") == 0)
				{
					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::GatewayInterface(), CGIScriptHandler::GatewayInterfaceVer);
					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::ScriptName(), m_header_info.uri);
					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::ServerName(), m_virtserv->m_server_name_vec[0]);
					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::ServerProtocol(), "HTTP/1.1");
					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::ServerPort(), "8080");
					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::ServerSoftware(), "8080");

					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::RemoteAddr(), "192.168.1.1");

					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::RequestMethod(), HTTP::MethodTable[m_header_info.method]);
					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::QueryString(), m_uri_info.query);

					m_cgi_handler.addMetaVar(CGIScriptHandler::MetaVariable::PathInfo(), ::dirname(m_ressource_path.c_str()));
				}
			}
			else if (!_isAReadableRegFile(m_ressource_path.c_str()))
				throw (Exception(NotFound));
		}

		m_file_handle.open(m_ressource_path.c_str(), ios::in | ios::binary);
		if (!m_file_handle.is_open()) // at this point, the program MUST be able to open the file.
			throw (RequestHandler::Exception(InternalServerError));
	}

	void	RequestHandler::_methodInitPost()
	{
	}

	void	RequestHandler::_methodInitDelete()
	{
	}
}
