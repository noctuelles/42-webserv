/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 23:52:18 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "FileUtils.hpp"
#include "HTTP.hpp"
#include <exception>
#include <vector>
#include <iostream>

namespace ft
{
	void WebServ::Logger::reason(const char* cause, const char* reason)
	{
		std::cerr << "webserv: " << cause << ": " << reason << ".\n";
	}

	WebServ::WebServ()
		: m_poller(), m_socks(), m_custom_status_page(), m_status_table(), m_listener_init(false)
	{
		m_custom_status_page.reserve(MaxStatusCode);

		m_status_table[http::BadRequest]          = http::InfoBadRequest;
		m_status_table[http::Forbidden]           = http::InfoForbidden;
		m_status_table[http::NotFound]            = http::InfoNotFound;
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

	const char*	WebServ::getStatusCodePage(http::StatusCode statuscode) const
	{
		return (m_status_table[statuscode].second);
	}

	const char*	WebServ::getStatusCodePhrase(http::StatusCode statuscode) const
	{
		return (m_status_table[statuscode].first);
	}

	EPoll&	WebServ::getPoller()
	{
		return (m_poller);
	}

	bool	WebServ::loop()
	{
		assert(m_listener_init);
		if (m_socks.size() == 0)
		{
			ft::WebServ::Logger::reason("fatal error", "no listening socket available");
			return (false);
		}
		m_poller.waitForEvents(EPoll::NOTIMEOUT);
		return (true);
	}

	WebServ::~WebServ()
	{}
}
