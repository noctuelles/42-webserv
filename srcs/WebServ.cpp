/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 23:16:06 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "ConnectionSocket.hpp"
#include "ListeningSocket.hpp"
#include "FileUtils.hpp"
#include "Http.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <exception>
#include <functional>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <memory>
#include <utility>
#include <iostream>
#include <signal.h>
#include "Log.hpp"

const std::string	WebServ::Version("webserv/0.1");

WebServ::WebServ(const char *config_filename)
	: m_poller(),
	m_socks(),
	m_virtserv_info(config_filename)
{
	struct sigaction	int_act;

	std::memset(&int_act, 0, sizeof(struct sigaction));
	::sigemptyset(&int_act.sa_mask);
	int_act.sa_handler = sigHandler;

	if (::sigaction(SIGINT, &int_act, NULL) < 0)
		throw (std::runtime_error("sigaction"));

	// Init listening_sockets and add them to watchlist
	VirtServInfo::iterator it  = m_virtserv_info.begin();
	VirtServInfo::iterator end = m_virtserv_info.end();
	for (; it != end; ++it)
	{
		m_socks.push_back(new IO::ListeningSocket(it->first));
		m_poller.add(m_socks.back()->get(), IO::EPoll::Event::In(), m_socks.back());
		::Log().get(INFO) << "Now listening on [" << ::inet_ntoa(it->first.sin_addr) << ':' << ::ntohs(it->first.sin_port) << "]...\n";
	}
}

void	WebServ::sigHandler(int signum)
{
	(void) signum;
}

int	WebServ::run()
{
	using namespace IO;

	assert(!m_socks.empty());
	while (true)
	{
		try
		{
			if (!m_poller.waitForEvents(TimeoutCheckOccurence))
				_removeTimeoutSocket();
		}
		catch (const std::runtime_error& e)
		{
			std::cerr << '\n';
			::Log().get(FATAL) << "Interrupted by SIGINT...\n";
			return (-1);
		}

		for (EPoll::iterator it = m_poller.begin(); it != m_poller.end(); it++)
		{
			InternetSocket*		inSockPtr = static_cast<InternetSocket*>(it->data.ptr);

			try
			{
				if (it->events & EPOLLIN)
				{
					int ret;

					switch ((ret = inSockPtr->recv()))
					{
						case ConnectionSocket::READING:
							break;
						case ConnectionSocket::FETCH_SEND_DATA:
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
					}
				}
				else if (it->events & EPOLLHUP)
					_removeSocket(inSockPtr);
			}
			catch (const std::exception& e)
			{
				::Log().get(FATAL) << "a major problem occured and the server couldn't fullfill the request: " << e.what() << ".\n";
				_removeSocket(inSockPtr);
			}
		}
	}
	return (0);
}

WebServ::~WebServ()
{
	std::for_each(m_socks.begin(), m_socks.end(), DeleteObj());
}

inline void	WebServ::_addConnection(int fd)
{
	IO::ConnectionSocket*	ptr = new IO::ConnectionSocket(fd, m_virtserv_info.m_virtserv_map);

	m_socks.push_back(ptr);
	m_poller.add(*ptr, IO::EPoll::Event::In(), ptr);
}

inline void	WebServ::_removeSocket(IO::InternetSocket* ptr)
{
	m_poller.remove(*ptr);
	delete ptr;
	m_socks.erase(std::find(m_socks.begin(), m_socks.end(), ptr));
}

inline void	WebServ::_removeTimeoutSocket()
{
	std::for_each(m_socks.begin(), m_socks.end(), DeleteAndNullifyTimeoutSocket());
	m_socks.erase(std::remove(m_socks.begin(), m_socks.end(), static_cast<IO::InternetSocket*>(0)), m_socks.end());
}
