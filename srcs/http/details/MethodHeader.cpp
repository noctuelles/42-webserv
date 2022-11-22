/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodHeader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:38:57 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/22 23:51:52 by plouvel          ###   ########.fr       */
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
			MIME	mime = getMimeFromFileExtension(m_ressource_path.c_str());
			size_t		fileSize = IO::getFileSize(m_ressource_path.c_str());

			header.addField(Field::ContentLenght(), Utils::integralToString(fileSize));
			header.addField(Field::ContentType(), mime);
			if (mime == MIME::ApplicationOctetStream())
			{
				header.addField(Field::ContentDisposition(), string("attachment; filename=\"").append(::basename(m_ressource_path.c_str())).append("\""));
				header.addField(Field::ContentTransferEncoding(), "binary");
			}
		}
		else if (m_request_type == AUTOINDEX)
		{
			header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);
			header.addField(Field::ContentLenght(), Utils::integralToString(m_page_to_send.size()));
			header.addField(Field::ContentType(), MIME::TextHtml());
		}
	}

	void	RequestHandler::_methodHeaderPost(ResponseHeader& header)
	{
		(void) header;
	}

	void	RequestHandler::_methodHeaderDelete(ResponseHeader& header)
	{
		(void) header;
	}

	void	RequestHandler::_methodHeaderError(ResponseHeader& header)
	{
		header.setReasonPhrase(StatusInfoPages::get()[m_status_code].phrase);
		header.addField(Field::ContentType(), MIME::TextHtml());
		header.addField(Field::ContentLenght(), Utils::integralToString(StatusInfoPages::get()[m_status_code].page.size()));
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
