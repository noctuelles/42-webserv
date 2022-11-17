/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:28:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/17 14:25:17 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "FileUtils.hpp"
#include <algorithm>
#include <iostream>
#include <sys/stat.h>

using std::find_if;

namespace ft
{
	namespace http
	{
		void	RequestHandler::_methodInitGet()
		{
			using std::ios;
			using std::ifstream;

			if (*m_uri_info.absolute_path.rbegin() == '/')
			{
				if (!m_route->m_autoindex)
				{
					const vector<string>&			index_vec		= m_route->m_index_vec;
					vector<string>::const_iterator	selected_file	= find_if(index_vec.begin(), index_vec.end(), ValidIndexFile(m_uri_info.absolute_path));

					if (selected_file != index_vec.end())
						m_uri_info.absolute_path.append(*selected_file);
					else
						throw (Exception(NotFound));
				}
				else // autoindex
				{
					throw (Exception(NotImplemented));
				}
			}
			else
			{
				if (!io::isAReadableRegFile(m_uri_info.absolute_path.c_str()))
					throw (Exception(NotFound));
			}

			m_file_handle.open(m_uri_info.absolute_path.c_str(), ios::in | ios::binary);
			if (!m_file_handle.is_open()) // at this point, the program MUST be able to open the file.
				throw (RequestHandler::Exception(InternalServerError));
		}

		void	RequestHandler::_methodInitPost()
		{
		}

		void	RequestHandler::_methodInitDelete()
		{
		}
	}
}
