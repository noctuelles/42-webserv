/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 16:50:04 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "FileUtils.hpp"
#include "HTTP.hpp"
#include "UniquePtr.hpp"
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
		m_socks.push_back(unique_ptr<InternetSocket>(new ListeningSocket(INADDR_ANY, port)));
		static_cast<ListeningSocket*>(m_socks.back().get())->listen(5);
		m_poller.add(m_socks.back()->getFd(), EPOLLIN, m_socks.back().get());
	}

	void	WebServ::addClient(int fd)
	{
		m_socks.push_back(unique_ptr<InternetSocket>(new Client(fd)));
		m_poller.add(m_socks.back()->getFd(), EPOLLIN, m_socks.back().get());
	}

	void	WebServ::removeSocket(int fd)
	{
		for (std::vector<unique_ptr<InternetSocket> >::iterator i = m_socks.begin(); i != m_socks.end(); i++)
		{
			if (i->get()->getFd() == fd)
			{
				m_socks.erase(i);
				return ;
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
				std::cout << "prout\n";
				// Note: dynamic cast incur runtime costs.
				InternetSocket*		inSockPtr       = static_cast<InternetSocket*>(it->data.ptr);

				if (it->events & EPOLLERR || it->events & EPOLLHUP)
				{
					removeSocket(*inSockPtr); continue;
				}
				if (it->events & EPOLLIN)
				{
					int ret;

					switch ((ret = inSockPtr->recv()))
					{
						case Client::SENDING_RESPONSE_BODY:
							m_poller.modify(*inSockPtr, EPOLLOUT, inSockPtr);
							break;
						case Client::DISCONNECT:
							removeSocket(*inSockPtr);
							break;
						default:
							addClient(ret);
					}
				}
				if (it->events & EPOLLOUT) // Only client are registered on EPOLLOUT. Can safely downcast.
				{
				}
			}
		}
	}

	WebServ::~WebServ()
	{}
}
