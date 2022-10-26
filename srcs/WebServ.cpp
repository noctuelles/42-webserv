/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 19:41:16 by plouvel          ###   ########.fr       */
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
		: m_poller(), m_socks(), m_custom_err(), m_errtable(), m_listener_init(false)
	{
		m_custom_err.reserve(50);
		m_errtable[http::BadRequest]          = http::DefaultPageBadRequest;
		m_errtable[http::Forbidden]           = http::DefaultPageForbidden;
		m_errtable[http::NotFound]            = http::DefaultPageNotFound;
		m_errtable[http::NotImplemented]      = http::DefaultPageNotImplemented;
		m_errtable[http::VersionNotSupported] = http::DefaultPageVersionNotSupported;
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

	void	WebServ::loadErrorPage(http::StatusCode errcode, const char* filename)
	{
		try
		{
			m_custom_err.push_back(ft::io::loadFileContent(filename, MaxErrorPageSize));
			m_errtable.at(errcode) = reinterpret_cast<const char *>(m_custom_err.back().data());
		}
		catch (const std::exception& e)
		{
			std::cerr << "webserv: cannot load error page '" << filename << "': default page is loaded instead.\n";
		}
	}

	const char*	WebServ::getErrorPage(http::StatusCode errcode)
	{
		return (m_errtable.at(errcode));
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
