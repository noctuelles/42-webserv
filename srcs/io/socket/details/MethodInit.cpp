/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodInit.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:28:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/11 15:00:23 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"

namespace ft
{
	void	ClientSocket::_methodInitGet()
	{
		using std::ios;
		using std::ifstream;

		m_file_handle.open(m_parser.getRequestLine().c_str(), ios::in | ios::binary);
		if (!m_file_handle.is_open())
			throw (std::ios_base::failure("can't open file"));
	}

	void	ClientSocket::_methodInitPost()
	{
	}

	void	ClientSocket::_methodInitDelete()
	{
	}
}
