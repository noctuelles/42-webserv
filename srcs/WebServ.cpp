/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 22:11:54 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "FileUtils.hpp"
#include "HTTP.hpp"
#include "Client.hpp"
#include <exception>
#include <string>
#include <utility>
#include <iostream>

namespace ft
{
	const std::string	WebServ::Version("webserv/0.1");

	WebServ::WebServ()
		: m_poller(),
		m_socks(),
		m_status_table()
	{
		m_status_table.resize(http::MaxStatusCode);

		m_status_table[http::BadRequest]			= HTTP_ERRPAGE("400 Bad Request");
		m_status_table[http::Forbidden]				= HTTP_ERRPAGE("403 Forbidden");
		m_status_table[http::NotFound]				= HTTP_ERRPAGE("404 Not Found");
		m_status_table[http::RequestTimeout]		= HTTP_ERRPAGE("408 Request Timeout");
		m_status_table[http::UriTooLong]			= HTTP_ERRPAGE("414 Uri Too Long");
		m_status_table[http::NotImplemented]		= HTTP_ERRPAGE("501 Not Implemented");
		m_status_table[http::VersionNotSupported]	= HTTP_ERRPAGE("505 HTTP Version Not Supported");
	}

	void	WebServ::addListener(in_port_t port)
	{
		ListeningSocket*	ptr = new ListeningSocket(INADDR_ANY, port);
		ptr->listen(MaxPendingConnection);
		m_poller.add(*ptr, EPOLLIN, ptr);
		m_socks.push_back(ptr);
	}

	void	WebServ::addClient(int fd)
	{
		Client*	ptr = new Client(fd, m_status_table);
		m_poller.add(*ptr, EPOLLIN, ptr);
		m_socks.push_back(ptr);
	}

	void	WebServ::removeSocket(int fd)
	{
		for (InSockVector::iterator i = m_socks.begin(); i != m_socks.end(); i++)
		{
			InternetSocket*	ptr = *i;

			if (ptr->getFd() == fd)
			{
				delete ptr;
				m_socks.erase(i);
				return ; // because fd's are unique.
			}
		}
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
		while (true)
		{
			m_poller.waitForEvents(EPoll::NOTIMEOUT);
			for (EPoll::iterator it = m_poller.begin(); it != m_poller.end(); it++)
			{
				InternetSocket*		inSockPtr = static_cast<InternetSocket*>(it->data.ptr);

				if (it->events & EPOLLERR || it->events & EPOLLHUP)
				{	removeSocket(*inSockPtr); continue; }
				if (it->events & EPOLLIN)
				{
					int ret;

					switch ((ret = inSockPtr->recv()))
					{
						case Client::SENDING_RESPONSE_HEADER:
						case Client::SENDING_RESPONSE_BODY:
							m_poller.modify(*inSockPtr, EPOLLOUT, inSockPtr);
							break;
						case Client::DISCONNECT:
							std::cout << "Client (" << inet_ntoa(inSockPtr->getSockAddr().sin_addr) << ":" << htons(inSockPtr->getSockAddr().sin_port) << ") " << "disconnected.\n";
							removeSocket(*inSockPtr);
							break;
						case Client::FETCHING_REQUEST_BODY:
						case Client::FETCHING_REQUEST_HEADER:
							break;
						default:
							addClient(ret);
					}
				}
				else if (it->events & EPOLLOUT)
				{
					switch (inSockPtr->send())
					{
						case Client::DISCONNECT:
							removeSocket(*inSockPtr);
							break;
					}
				}
			}
		}
	}

	WebServ::~WebServ()
	{
		for (InSockVector::iterator it = m_socks.begin(); it != m_socks.end(); it++)
			delete *it;
	}
}
