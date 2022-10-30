/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 21:17:04 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "FileUtils.hpp"
#include "ResponseHeader.hpp"
#include "HTTP.hpp"
#include <exception>
#include <vector>
#include <iostream>

namespace ft
{
	WebServ::WebServ()
		: m_poller(),
		m_socks(),
		m_status_table(),
		m_listener_init(false)
	{
		m_status_table.resize(http::MaxStatusCode);

		m_status_table[http::BadRequest]	= HTTP_ERRPAGE("400 Bad Request");
		m_status_table[http::NotFound]		= HTTP_ERRPAGE("404 Not Found");
	}

	void	WebServ::addListener(in_port_t port)
	{
		m_socks.push_back(ListeningSocket(INADDR_ANY, port));
	}

	void	WebServ::initListener()
	{
		std::for_each(m_socks.begin(), m_socks.end(), ListeningSocketInit(m_poller));
		m_listener_init = true;
	}

	void	WebServ::removeListener(int fd)
	{
		std::vector<ListeningSocket>::iterator it = std::find_if(m_socks.begin(), m_socks.end(), SocketComp(fd));
		if (it == m_socks.end())
			return ;
		m_socks.erase(it);
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

	const http::StatusInfo&	WebServ::getStatusCodeInfo(http::StatusCode statusCode) const
	{
		return (m_status_table[statusCode]);
	}

	void	WebServ::run()
	{
		while (true)
		{
			m_poller.waitForEvents(EPoll::NOTIMEOUT);
			for (EPoll::iterator it = m_poller.begin(); it != m_poller.end(); it++)
			{
				// Note: dynamic cast incur runtime costs.
				InternetSocket*		inSockPtr       = static_cast<InternetSocket*>(it->data.ptr);
				ListeningSocket*	listenSockPtr   = dynamic_cast<ListeningSocket*>(inSockPtr);
				Client*				clientPtr       = dynamic_cast<Client*>(inSockPtr);

				// Check if error condition happened on the associated file descriptor watched by epoll.
				if (it->events & EPOLLERR || it->events & EPOLLHUP)
				{
					if (listenSockPtr)
						removeListener(listenSockPtr->getFd());
					else
						clientPtr->getBindedSocket()->removeConnection(clientPtr);
				}
				if (it->events & EPOLLIN)
				{
					if (listenSockPtr)
					{
						std::cout << "Accepting new connection\n";
						listenSockPtr->acceptConnection(m_poller);
					}
					else
					{
						clientPtr->receive();
						switch (clientPtr->proceed())
						{
							case Client::SENDING_RESPONSE_ERROR_HEADER:
							case Client::SENDING_RESPONSE_HEADER:
								m_poller.modify(*clientPtr, EPOLLOUT, clientPtr);
								break;
							default:
								;
						}
					}
				}
				if (it->events & EPOLLOUT) // Only client are registered on EPOLLOUT
				{
					if (clientPtr->send(*this) == Client::DONE)
						clientPtr->getBindedSocket()->removeConnection(clientPtr);
				}
			}
		}
	}

	WebServ::~WebServ()
	{}
}
