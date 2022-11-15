/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodHeader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:38:57 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 15:38:50 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "FileUtils.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include "MIME.hpp"
#include "StatusInfoPages.hpp"

namespace ft
{
	namespace http
	{
		void	RequestHandler::_methodHeaderGet(ResponseHeader& header)
		{
			size_t	fileSize = io::getFileSize(m_header_info.req_line.c_str());

			header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);
			header.addField(Field::ContentLenght(), utils::integralToString(fileSize));
			header.addField(Field::ContentType(), getMimeFromFileExtension(m_header_info.req_line.c_str()));
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
			header.addField(Field::ContentLenght(), utils::integralToString(StatusInfoPages::get()[m_status_code].page.size()));
		}
	}
}
