/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodHeader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:38:57 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/11 14:28:51 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "FileUtils.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include "HttpMIME.hpp"

namespace ft
{
	using namespace http;

	void	ClientSocket::_methodHeaderGet(ResponseHeader& header)
	{
		size_t	fileSize = io::getFileSize(m_parser.getRequestLine().c_str());

		header.addField(Field::ContentLenght(), utils::integralToString(fileSize));
		header.addField(Field::ContentType(), getMimeFromFileExtension(m_parser.getRequestLine()).toStr());
	}

	void	ClientSocket::_methodHeaderPost(ResponseHeader& header)
	{
		(void) header;
	}

	void	ClientSocket::_methodHeaderDelete(ResponseHeader& header)
	{
		(void) header;
	}
}
