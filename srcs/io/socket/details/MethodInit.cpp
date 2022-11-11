/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:28:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/11 16:53:21 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "FileUtils.hpp"

namespace ft
{
	void	ClientSocket::_methodInitGet()
	{
		using std::ios;
		using std::ifstream;

		// TODO: detect if the file is a folder (in UNIX a folder is also a folder)
		// Code AutoIndex
		// For now, send a 404 error.

		m_file_handle.open(m_parser.getRequestLine().c_str(), ios::in | ios::binary);
		if (!m_file_handle.is_open())
			throw (std::ios_base::failure("can't open file"));
		else if (io::isADirectory(m_parser.getRequestLine().c_str()))
			throw (std::ios_base::failure("is a directory"));
	}

	void	ClientSocket::_methodInitPost()
	{
	}

	void	ClientSocket::_methodInitDelete()
	{
	}
}
