/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/09 16:19:39 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "ClientSocket.hpp"
#include "ListeningSocket.hpp"
#include "FileUtils.hpp"
#include "HTTP.hpp"
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
		m_status_table(),
		m_virtserv_info(config_filename)
	{
		m_status_table.resize(http::MaxStatusCode);

		m_status_table[http::BadRequest]			= HTTP_ERRPAGE("400 Bad Request");
		m_status_table[http::Forbidden]				= HTTP_ERRPAGE("403 Forbidden");
		m_status_table[http::NotFound]				= HTTP_ERRPAGE("404 Not Found");
		m_status_table[http::RequestTimeout]		= HTTP_ERRPAGE("408 Request Timeout");
		m_status_table[http::UriTooLong]			= HTTP_ERRPAGE("414 Uri Too Long");
		m_status_table[http::NotImplemented]		= HTTP_ERRPAGE("501 Not Implemented");
		m_status_table[http::VersionNotSupported]	= HTTP_ERRPAGE("505 HTTP Version Not Supported");

		// Init listening_sockets and add them to watchlist
		VirtServInfo::iterator it  = m_virtserv_info.begin();
		VirtServInfo::iterator end = m_virtserv_info.end();
		for (; it != end; ++it)
		{
			m_socks.push_back(new ListeningSocket(it->first));
			m_poller.add(m_socks.back()->getFd(), EPOLLIN, m_socks.back());
		}
	}

	void	WebServ::addListener(in_port_t port)
	{
		ListeningSocket*	ptr = new ListeningSocket(INADDR_ANY, port);

		m_socks.push_back(ptr);
		ptr->listen(MaxPendingConnection); // CAN THROW
		m_poller.add(*ptr, EPOLLIN, ptr); // CAN THROW
	}

	void	WebServ::setStatusCodePage(http::StatusCode statuscode, const char* filename)
	{
		try
		{
			m_status_table[statuscode].page.second = io::loadFileContent(filename);
		}
		catch (const std::exception& e)
		{
			std::cerr << "webserv: cannot load error page '" << filename << "': default page is used instead.\n";
		}
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

				if (it->events & EPOLLERR || it->events & EPOLLHUP)
				{ _removeSocket(inSockPtr); continue; }
				if (it->events & EPOLLIN)
				{
					int ret;

					switch ((ret = inSockPtr->recv()))
					{
						case ClientSocket::SENDING_RESPONSE_HEADER:
						case ClientSocket::SENDING_RESPONSE_BODY:
							m_poller.modify(*inSockPtr, EPOLLOUT, inSockPtr);
							break;
						case ClientSocket::DISCONNECT:
							std::cout << "Client (" << inet_ntoa(inSockPtr->getSockAddr().sin_addr) << ":" << htons(inSockPtr->getSockAddr().sin_port) << ") " << "disconnected.\n";
							_removeSocket(inSockPtr);
							break;
						case ClientSocket::FETCHING_REQUEST_BODY:
						case ClientSocket::FETCHING_REQUEST_HEADER:
							break;
						default:
							std::cout << "Listening (" << inet_ntoa(inSockPtr->getSockAddr().sin_addr) << ":" << htons(inSockPtr->getSockAddr().sin_port) << ") " << "disconnected.\n";
							_addClient(ret);
					}
				}
				else if (it->events & EPOLLOUT)
				{
					switch (inSockPtr->send())
					{
						case ClientSocket::DISCONNECT:
							_removeSocket(inSockPtr);
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

	inline void	WebServ::_addClient(int fd)
	{
		ClientSocket*	ptr = new ClientSocket(fd, m_status_table);

		m_socks.push_back(ptr);
		m_poller.add(*ptr, EPOLLIN, ptr);
	}

	inline void	WebServ::_removeSocket(InternetSocket* ptr)
	{
		delete ptr;
		m_socks.erase(std::find(m_socks.begin(), m_socks.end(), ptr));
	}

	inline void	WebServ::_removeTimeoutSocket()
	{
		std::for_each(m_socks.begin(), m_socks.end(), DeleteAndNullifyTimeoutSocket());
		m_socks.erase(std::remove(m_socks.begin(), m_socks.end(), static_cast<InternetSocket*>(0)), m_socks.end());
	}
}
