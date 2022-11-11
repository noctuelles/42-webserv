/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MethodSend.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 13:41:28 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/11 15:17:37 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"

namespace ft
{
	void	ClientSocket::_methodSendGet()
	{
		// read can throw a failure.
		m_file_handle.read(reinterpret_cast<char *>(m_send_buffer.data()), m_send_buffer.size());
		if (::send(m_fd, m_send_buffer.data(), m_file_handle.gcount(), 0) < 0)
			m_state = DISCONNECT; // an error occured
		else if (m_file_handle.eof())
		{
			m_file_handle.close();
			m_state = DISCONNECT; // end-of-file is reached.
		}
	}

	void	ClientSocket::_methodSendPost()
	{

	}

	void	ClientSocket::_methodSendDelete()
	{

	}
}
