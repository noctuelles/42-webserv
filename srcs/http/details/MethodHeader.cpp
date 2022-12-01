/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodHeader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:38:57 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/01 13:20:10 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "FileUtils.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include "MIME.hpp"
#include "StatusInfoPages.hpp"
#include <iostream>

namespace HTTP
{
	void	RequestHandler::_methodHeaderGet(ResponseHeader& header)
	{
		header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);

		if (m_request_type == FILE)
		{
			MIME		mime = getMimeFromFileExtension(m_res_info.path.c_str());
			size_t		fileSize = IO::getFileSize(m_res_info.path.c_str());

			header.addField(Field::ContentLength(), Utils::integralToString(fileSize));
			header.addField(Field::ContentType(), mime);
			if (mime == MIME::ApplicationOctetStream())
			{
				header.addField(Field::ContentDisposition(), string("attachment; filename=\"").append(::basename(m_res_info.path.c_str())).append("\""));
				header.addField(Field::ContentTransferEncoding(), "binary");
			}
		}
		else if (m_request_type == AUTOINDEX)
		{
			header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);
			header.addField(Field::ContentLength(), Utils::integralToString(m_page_to_send.size()));
			header.addField(Field::ContentType(), MIME::TextHtml());
		}
		else if (m_request_type == CGI)
		{
			const CGIScriptHandler::CGIScriptInfo&	script_info = m_cgi_handler.getScriptInfo();

			header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);
			header.addField(Field::ContentLength(), Utils::integralToString(script_info.content_length));
			header.insert(script_info.header_field.begin(), script_info.header_field.end());
		}
	}

	void	RequestHandler::_methodHeaderPost(ResponseHeader& header)
	{
		if (m_request_type == FILE_UPLOAD)
		{
			header.setReasonPhrase(StatusInfoPages::get()[Created].phrase);

			header.addField(Field::Location(), m_header_info.request_line);
		}
		else if (m_request_type == CGI)
		{
			
		}
	}

	void	RequestHandler::_methodHeaderDelete(ResponseHeader& header)
	{
		(void) header;
	}

	void	RequestHandler::_methodHeaderError(ResponseHeader& header)
	{
		header.setReasonPhrase(StatusInfoPages::get()[m_status_code].phrase);

		if (m_request_type == ERROR)
		{
			header.addField(Field::ContentType(), MIME::TextHtml());
			header.addField(Field::ContentLength(), Utils::integralToString(StatusInfoPages::get()[m_status_code].page.size()));
		}
		else if (m_request_type == REDIRECT_ERROR)
		{
			header.addField(Field::Location(), m_res_info.path);
		}

		if (m_status_code == MethodNotAllowed)
		{
			string	allowed_method;

			for (size_t i = 0; i < NbrAvailableMethod; i++)
			{
				if (m_route->m_methods[i])
				{
					if (i != NbrAvailableMethod - 1)
						allowed_method.append(MethodTable[i]).append(", ");
					else
						allowed_method.append(MethodTable[i]);
				}
			}
			header.addField(Field::Allow(), allowed_method);
		}
	}
}
