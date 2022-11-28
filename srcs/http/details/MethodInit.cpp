/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:28:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/28 11:49:59 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"
#include "Http.hpp"
#include "MIME.hpp"
#include "RequestHandler.hpp"
#include "FieldParser.hpp"
#include "FileUtils.hpp"
#include "Utils.hpp"
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
				m_page_to_send = AutoIndex::generatePage(m_virtserv->m_routes_vec, *m_route, m_header_info.uri.absolute_path, m_ressource_path, m_header_info.uri.query);
				return ;
			}
		}
		else
		{
			if (!_isAReadableRegFile(m_ressource_path.c_str()))
				throw (Exception(NotFound));
		}

		m_file_handle.open(m_ressource_path.c_str(), ios::in | ios::binary);
		if (!m_file_handle.is_open()) // at this point, the program MUST be able to open the file.
			throw (RequestHandler::Exception(InternalServerError));
		m_request_type = FILE;
	}

	void	RequestHandler::_methodInitPost()
	{
		std::string	cgiExt = ".php";

		if (m_ressource_path.compare(m_ressource_path.length() - cgiExt.length(), cgiExt.length(), cgiExt) == 0)
		{
			throw (Exception(NotImplemented));
			m_request_type = CGI;
		}
		else
		{
			// Check if this is a correct directory and that we've write permissions.
			mode_t	file_mode = IO::getFileMode(m_ressource_path.c_str());

			if (!((file_mode & S_IFMT) & S_IFDIR && file_mode & S_IWUSR))
				throw (Exception(Forbidden));

			const HeaderFieldMap::const_iterator	content_type = m_header_info.header_field.find(Field::ContentType());
			const HeaderFieldMap::const_iterator	content_len = m_header_info.header_field.find(Field::ContentLength());
			const HeaderFieldMap::const_iterator	transfer_enc = m_header_info.header_field.find(Field::TransferEncoding());

			// Check the existance of borth ContentType and ContentLenght header (does not support chunked request).
			if (content_type == m_header_info.header_field.end())
				throw (Exception(BadRequest));
			if (content_len == m_header_info.header_field.end())
			{ 
				if (transfer_enc != m_header_info.header_field.end())
					throw (Exception(NotImplemented));
				else
					throw (Exception(BadRequest));
			}

			ContentInfo										ctype = FieldParser()(content_type->second);
			const std::map<string, string>::const_iterator	boundary = ctype.param.find("boundary");
			size_t											clen = Utils::stringToIntegral<size_t>(content_len->second);

			// Only support multipart/form-data for uploading with POST
			if (ctype.value != MIME::MultipartFormData().toStr())
				throw (Exception(NotImplemented));
			// Check if the boundary exist and is not empty
			if (boundary == ctype.param.end())
				throw (Exception(BadRequest));
			else
			{
				if (boundary->second.length() == 0)
					throw (Exception(BadRequest));
			}

			m_multipart_handler = new MultiPartHandler(m_ressource_path, clen, boundary->second);
			m_request_type = FILE_UPLOAD;
		}
	}

	void	RequestHandler::_methodInitDelete()
	{
	}
}
