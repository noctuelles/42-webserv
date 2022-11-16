/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodHeader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:38:57 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/16 16:21:14 by plouvel          ###   ########.fr       */
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
			size_t	fileSize = ::ft::io::getFileSize(m_header_info.uri.c_str());

			header.setReasonPhrase(StatusInfoPages::get()[OK].phrase);
			header.addField(Field::ContentLenght(), utils::integralToString(fileSize));
			header.addField(Field::ContentType(), getMimeFromFileExtension(m_header_info.uri.c_str()));
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
}
