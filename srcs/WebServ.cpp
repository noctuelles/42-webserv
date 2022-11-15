/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 14:46:38 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "ConnectionSocket.hpp"
#include "ListeningSocket.hpp"
#include "FileUtils.hpp"
#include "Http.hpp"
#include <algorithm>
#include <exception>
#include <functional>
#include <string>
#include <memory>
#include <utility>
#include <iostream>

namespace ft
{
	const std::string	WebServ::Version("webserv/0.1");

	WebServ::WebServ(const char *config_filename)
		: m_poller(),
		m_socks(),
		m_virtserv_info(config_filename)
	{
		// Init listening_sockets and add them to watchlist
		VirtServInfo::iterator it  = m_virtserv_info.begin();
		VirtServInfo::iterator end = m_virtserv_info.end();
		for (; it != end; ++it)
		{
			m_socks.push_back(new ListeningSocket(it->first));
			m_poller.add(m_socks.back()->getFd(), EPoll::Event::In(), m_socks.back());
		}
	}

	void	WebServ::addListener(in_port_t port)
	{
		ListeningSocket*	ptr = new ListeningSocket(INADDR_ANY, port);

		m_socks.push_back(ptr);
		ptr->listen(MaxPendingConnection); // CAN THROW
		m_poller.add(*ptr, EPoll::Event::In(), ptr); // CAN THROW
	}

	void	WebServ::run()
	{
		assert(!m_socks.empty());
		while (true)
		{
			if (!m_poller.waitForEvents(TimeoutCheckOccurence))
				_removeTimeoutSocket();
			for (EPoll::iterator it = m_poller.begin(); it != m_poller.end(); it++)
			{
				InternetSocket*		inSockPtr = static_cast<InternetSocket*>(it->data.ptr);

				if (it->events & EPOLLHUP || it->events & EPOLLERR)
				{ _removeSocket(inSockPtr); continue; }
				if (it->events & EPOLLIN)
				{
					int ret;

					switch ((ret = inSockPtr->recv()))
					{
						case ConnectionSocket::READING:
							break;
						case ConnectionSocket::WRITING:
							m_poller.modify(*inSockPtr, EPoll::Event::Out(), inSockPtr);
							break;
						case ConnectionSocket::DISCONNECT:
							_removeSocket(inSockPtr);
							break;
						default:
							_addConnection(ret);
					}
				}
				else if (it->events & EPOLLOUT)
				{
					switch (inSockPtr->send())
					{
						case ConnectionSocket::DISCONNECT:
							_removeSocket(inSockPtr);
							break;
						case ConnectionSocket::READING:
							// Keep-alive...
							break;
					}
				}
			}
		}
	}

	WebServ::~WebServ()
	{
		std::for_each(m_socks.begin(), m_socks.end(), DeleteObj());
	}

	inline void	WebServ::_addConnection(int fd)
	{
		ConnectionSocket*	ptr = new ConnectionSocket(fd, m_virtserv_info.m_virtserv_map);

		m_socks.push_back(ptr);
		m_poller.add(*ptr, EPoll::Event::In(), ptr);
	}

	inline void	WebServ::_removeSocket(InternetSocket* ptr)
	{
		m_poller.remove(*ptr);
		delete ptr;
		m_socks.erase(std::find(m_socks.begin(), m_socks.end(), ptr));
	}

	inline void	WebServ::_removeTimeoutSocket()
	{
		std::for_each(m_socks.begin(), m_socks.end(), DeleteAndNullifyTimeoutSocket());
		m_socks.erase(std::remove(m_socks.begin(), m_socks.end(), static_cast<InternetSocket*>(0)), m_socks.end());
	}
}
