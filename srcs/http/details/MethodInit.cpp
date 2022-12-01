/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:28:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/01 22:00:53 by plouvel          ###   ########.fr       */
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

		if (m_request_type == CGI)
		{
			m_cgi_handler.addMetaVar("QUERY_STRING", m_header_info.uri.query);
			// TODO: CGI
		}
		else
		{
			if (*m_res_info.path.rbegin() == '/')
			{
				if (!m_route->m_autoindex)
				{
					// Search a default index file.
					const vector<string>&			index_vec		= m_route->m_index_vec;
					vector<string>::const_iterator	selected_file	= find_if(index_vec.begin(), index_vec.end(), ValidIndexFile(m_res_info.path));

					if (selected_file != index_vec.end())
						m_res_info.path.append(*selected_file);
					else
						throw (Exception(NotFound));
				}
				else // autoindex
				{
					m_request_type = AUTOINDEX;
					m_autoindex_page = AutoIndex::generatePage(m_virtserv->m_routes_vec, *m_route, m_header_info.uri.absolute_path, m_res_info.path, m_header_info.uri.query);
					return ;
				}
			}

			mode_t	file_mode = IO::getFileMode(m_res_info.path.c_str());

			if (!((file_mode & S_IFMT) & S_IFREG && (file_mode & S_IRUSR)))
				throw (Exception(NotFound));

			m_file_handle.open(m_res_info.path.c_str(), ios::in | ios::binary);
			if (!m_file_handle.is_open()) // at this point, the program MUST be able to open the file.
				throw (RequestHandler::Exception(InternalServerError));
			m_request_type = FILE;
		}
	}

	void	RequestHandler::_methodInitPost()
	{
		const HeaderFieldMap::const_iterator	content_type = m_header_info.header_field.find(Field::ContentType());
		const HeaderFieldMap::const_iterator	content_len = m_header_info.header_field.find(Field::ContentLength());
		const HeaderFieldMap::const_iterator	transfer_enc = m_header_info.header_field.find(Field::TransferEncoding());

		// Check the existance of both ContentType and ContentLenght header (does not support chunked request).
		if (content_type == m_header_info.header_field.end())
			throw (Exception(BadRequest));
		if (content_len == m_header_info.header_field.end())
		{ 
			if (transfer_enc != m_header_info.header_field.end())
				throw (Exception(NotImplemented));
			else
				throw (Exception(BadRequest));
		}

		m_content_len = Utils::stringToIntegral<size_t>(content_len->second);
		if (m_content_len > m_virtserv->m_max_body_size)
			throw (Exception(ContentTooLarge));

		if (m_request_type == CGI)
		{
			m_cgi_handler.addMetaVar("CONTENT_TYPE", content_type->second); 
			m_cgi_handler.addMetaVar("CONTENT_LENGTH", content_len->second);
		}
		else
		{
			// Check if we can upload in this route.
			mode_t	file_mode = IO::getFileMode(m_route->m_upload_store.c_str());

			if (!((file_mode & S_IFMT) & S_IFDIR && file_mode & S_IWUSR) || *m_route->m_upload_store.rbegin() != '/')
				throw (Exception(Forbidden));

			ContentInfo										ctype = FieldParser()(content_type->second);
			const std::map<string, string>::const_iterator	boundary = ctype.param.find("boundary");

			// Check if the Content-Length does not exceed the max body size.

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

			m_multipart_handler = new MultiPartHandler(m_route->m_upload_store, m_content_len, boundary->second);
			m_request_type = FILE_UPLOAD;
		}
	}

	void	RequestHandler::_methodInitDelete()
	{
		errno = 0;
		::unlink(m_res_info.path.c_str());
		std::cerr << BRED << "DELETE " << RESET << "unlink: " << strerror(errno) << '\n';
		switch (errno)
		{
			case 0:
				return;
			case EISDIR:
				errno = 0;
				rmdir(m_res_info.path.c_str());
				std::cerr << BRED << "DELETE " << RESET << "rmdir: " << strerror(errno) << '\n';
				switch (errno)
				{
					case 0:
						return;
					case ENOENT:
					case ENOTDIR:
					case EFAULT:
						throw (Exception(NotFound));
					case EPERM:
					case EACCES:
					case EROFS:
						throw (Exception(Forbidden));
					default:
						throw (Exception(InternalServerError));
				}
			case ENOENT:
			case ENOTDIR:
			case EFAULT:
				throw (Exception(NotFound));
			case EPERM:
			case EACCES:
			case EROFS:
				throw (Exception(Forbidden));
			default:
				throw (Exception(InternalServerError));
		}
	}
}
