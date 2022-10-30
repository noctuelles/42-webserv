/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 13:53:56 by plouvel          ###   ########.fr       */
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
	std::pair<const char*, const char*>	WebServ::m_status_table[http::MaxStatusCode];

	WebServ::WebServ()
		: m_poller(),
		m_socks(),
		m_custom_status_page(),
		m_forbidden_method(),
		m_listener_init(false)
	{
		m_custom_status_page.reserve(http::MaxStatusCode);

		m_status_table[http::BadRequest]          = http::InfoBadRequest;
		m_status_table[http::Forbidden]           = http::InfoForbidden;
		m_status_table[http::NotFound]            = http::InfoNotFound;
		m_status_table[http::RequestTimeout]      = http::InfoRequestTimeout;
		m_status_table[http::UriTooLong]          = http::InfoUriTooLong;
		m_status_table[http::NotImplemented]      = http::InfoNotImplemented;
		m_status_table[http::VersionNotSupported] = http::InfoVersionNotSupported;
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
			m_custom_status_page.push_back(ft::io::loadFileContent(filename, MaxErrorPageSize));
			m_status_table[statuscode].second = reinterpret_cast<const char *>(m_custom_status_page.back().data());
		}
		catch (const std::exception& e)
		{
			std::cerr << "webserv: cannot load error page '" << filename << "': default page is used instead.\n";
		}
	}

	const char*	WebServ::getStatusCodePage(http::StatusCode statuscode)
	{
		return (m_status_table[statuscode].second);
	}

	const char*	WebServ::getStatusCodePhrase(http::StatusCode statuscode)
	{
		return (m_status_table[statuscode].first);
	}
	
	bool	WebServ::isMethodAllowed(http::Method method) const
	{
		return !(m_forbidden_method[method]);
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
						// HERE we''ll be receiving things.
						clientPtr->receive();
						try
						{
							if (clientPtr->proceed() == Client::READY_FOR_RESPONSE_HEADER)
								m_poller.modify(*clientPtr, EPOLLOUT, clientPtr);
						}
						catch (const Exception& e)
						{

						}
					}
				}
				if (it->events & EPOLLOUT) // Only client are registered on EPOLLOUT
				{
				}
			}
		}
	}

	WebServ::~WebServ()
	{}
}
