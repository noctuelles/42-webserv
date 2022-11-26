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
#include "CGIScriptHandler.hpp"
#include "RequestHandler.hpp"
#include "FileUtils.hpp"
#include "StringArray.hpp"
#include "WebServ.hpp"

#include <algorithm>
#include <cstdlib>
#include <algorithm>
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

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
			if (m_route->m_cgi_extension != "") // Check if a cgi is set up
			{
				if (Utils::suffixMatch(m_uri_info.absolute_path, m_route->m_cgi_extension)) // Does the end of the URL match the extension defined ?
				{
					if ( pipe(m_cgi_input_pipe) == -1 )
						throw std::runtime_error("pipe");
					if ( pipe(m_cgi_output_pipe) == -1 )
						throw std::runtime_error("pipe");
					int cpid;
					if ( (cpid = fork()) == -1 )
						throw std::runtime_error("fork");
					else if (cpid == 0) // In child process
					{
						m_cgi_handler.addMetaVar("GATEWAY_INTERFACE", CGIScriptHandler::GatewayInterfaceVer);
						m_cgi_handler.addMetaVar("SCRIPT_NAME", m_ressource_path.c_str());
						m_cgi_handler.addMetaVar("SERVER_NAME", m_header_info.header_fields.at(Field::Host()));
						m_cgi_handler.addMetaVar("SERVER_PROTOCOL", "HTTP/1.1");
						m_cgi_handler.addMetaVar("REQUEST_METHOD", HTTP::MethodTable[m_header_info.method]);
						m_cgi_handler.addMetaVar("QUERY_STRING", m_uri_info.query);
						m_cgi_handler.addMetaVar("PATH_INFO", m_ressource_path.c_str());
						m_cgi_handler.addMetaVar("SERVER_SOFTWARE", WebServ::Version);

						m_cgi_handler.addMetaVar("SERVER_PORT", "8080"); // ?
						m_cgi_handler.addMetaVar("REMOTE_ADDRESS", "192.168.1.1"); // ??

						
						dup2(m_cgi_input_pipe[0], STDIN_FILENO); close(m_cgi_input_pipe[0]); // Replace stdin with m_cgi_input_pipe reading end
						close(m_cgi_input_pipe[1]); // Close m_cgi_input_pipe writing end;

						dup2(m_cgi_output_pipe[1], STDOUT_FILENO); close(m_cgi_output_pipe[1]); // Replace stdout with m_cgi_output_pipe writing end
						close(m_cgi_output_pipe[0]); // Close m_cgi_output_pipe reading end;

						// Setup env here 
						//execve("script_name", &"script_name", sa,getData());
						// Check errno for if not executable
						close(m_cgi_input_pipe[0]);
						std::cout << "Content-type: text/html\r\n\r\n";
						std::cout << "<html><head>CGI youpi</head><body>CGI youpi !</body></html>\r\n\r\n";
						exit(42);
					}
					m_request_type = CGI;
					return;
				}
			}
			else if (not _isAReadableRegFile(m_uri_info.absolute_path.c_str()))
				throw (Exception(Forbidden));
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
