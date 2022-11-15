/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:28:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 15:27:00 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "FileUtils.hpp"
#include <iostream>

namespace ft
{
	namespace http
	{
		void	RequestHandler::_methodInitGet()
		{
			using std::ios;
			using std::ifstream;

			// TODO: detect if the file is a folder (in UNIX a folder is also a folder)
			// Code AutoIndex
			// For now, send a 404 error.

			std::clog << "Trying to open " << m_header_info.req_line << "...\n";
			m_file_handle.open(m_header_info.req_line.c_str(), ios::in | ios::binary);
			if (!m_file_handle.is_open())
				throw (RequestHandler::Exception(NotFound));
			else if (io::isADirectory(m_header_info.req_line.c_str()))
				throw (RequestHandler::Exception(NotFound));
		}

		void	RequestHandler::_methodInitPost()
		{
		}

		void	RequestHandler::_methodInitDelete()
		{
		}
	}
}
