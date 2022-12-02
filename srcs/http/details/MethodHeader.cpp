/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodHeader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:38:57 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/02 13:13:29 by plouvel          ###   ########.fr       */
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
		switch (m_request_type)
		{
			case REDIRECT:
				header.setReasonPhrase(StatusInfoPages::get()[SeeOther].phrase);
				header.addField(Field::Location(), m_res_info.path);
				break;

			case FILE:
			{
				header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);
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
				break;

			case AUTOINDEX:
				header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);
				header.addField(Field::ContentLength(), Utils::integralToString(m_autoindex_page.size()));
				header.addField(Field::ContentType(), MIME::TextHtml());
				break;

			case CGI:
			{
				const CGIScriptHandler::CGIScriptInfo&	script_info = m_cgi_handler.getScriptInfo();
				const HeaderFieldMap::const_iterator	status_field = script_info.header_field.find(Field::Status());

				header.insert(script_info.header_field.begin(), script_info.header_field.end());
				if (status_field != script_info.header_field.end())
				{
					header.setReasonPhrase(status_field->second);
					header.removeField(Field::Status());
				}
				else
					header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);

				header.addField(Field::ContentLength(), Utils::integralToString(script_info.content_length));
			}
				break;
			default:
				;
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
			// Same as GET, for flexibility.
			const CGIScriptHandler::CGIScriptInfo&	script_info = m_cgi_handler.getScriptInfo();
			const HeaderFieldMap::const_iterator	status_field = script_info.header_field.find(Field::Status());

			header.insert(script_info.header_field.begin(), script_info.header_field.end());
			if (status_field != script_info.header_field.end())
			{
				header.setReasonPhrase(status_field->second);
				header.removeField(Field::Status());
			}
			else
				header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);

			header.addField(Field::ContentLength(), Utils::integralToString(script_info.content_length));
		}
	}

	void	RequestHandler::_methodHeaderDelete(ResponseHeader& header)
	{
		header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);
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
