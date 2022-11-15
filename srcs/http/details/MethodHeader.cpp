/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodHeader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:38:57 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 14:33:37 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestHandler.hpp"
#include "FileUtils.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include "HttpMIME.hpp"
#include "StatusInfoPages.hpp"

namespace ft
{
	namespace http
	{
		void	HttpRequestHandler::_methodHeaderGet(ResponseHeader& header)
		{
			size_t	fileSize = io::getFileSize(m_header_info.req_line.c_str());

			header.addField(Field::ContentLenght(), utils::integralToString(fileSize));
			header.addField(Field::ContentType(), getMimeFromFileExtension(m_header_info.req_line.c_str()));
		}

		void	HttpRequestHandler::_methodHeaderPost(ResponseHeader& header)
		{
			(void) header;
		}

		void	HttpRequestHandler::_methodHeaderDelete(ResponseHeader& header)
		{
			(void) header;
		}

		void	HttpRequestHandler::_methodHeaderError(ResponseHeader& header)
		{
			header.addField(Field::ContentType(), MIME::TextHtml());
			header.addField(Field::ContentLenght(), utils::integralToString(StatusInfoPages::get()[m_status_code].page.size()));
		}
	}
}
