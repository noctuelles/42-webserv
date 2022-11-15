/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:28:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 14:15:53 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequestHandler.hpp"
#include "FileUtils.hpp"
#include <iostream>

namespace ft
{
	namespace http
	{
		void	HttpRequestHandler::_methodInitGet()
		{
			using std::ios;
			using std::ifstream;

			// TODO: detect if the file is a folder (in UNIX a folder is also a folder)
			// Code AutoIndex
			// For now, send a 404 error.

			m_file_handle.open(m_header_info.req_line.c_str(), ios::in | ios::binary);
			if (!m_file_handle.is_open())
				throw (std::ios_base::failure("can't open file"));
			else if (io::isADirectory(m_header_info.req_line.c_str()))
				throw (std::ios_base::failure("is a directory"));
		}

		void	HttpRequestHandler::_methodInitPost()
		{
		}

		void	HttpRequestHandler::_methodInitDelete()
		{
		}
	}
}
