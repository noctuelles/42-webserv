/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 17:47:09 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include "FileUtils.hpp"
#include <vector>
#include <iostream>

namespace ft
{
	void WebServ::Logger::reason(const char* cause, const char* reason)
	{
		std::cerr << "webserv: " << cause << ": " << reason << ".\n";
	}

	WebServ::WebServ()
		: m_poller(), m_socks(), m_listener_init(false)
	{}

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

	void	WebServ::loadErrorPage(unsigned int errcode, const char* filename)
	{
		m_errtable.insert(std::make_pair(errcode, ft::loadFileContent(filename, MaxErrorPageSize)));
	}

	std::vector<unsigned char>&	WebServ::getErrorPage(unsigned int errcode)
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
		std::cout << "Blocking on epoll_wait()...\n";
		m_poller.waitForEvents(EPoll::NOTIMEOUT);
		return (true);
	}

	WebServ::~WebServ()
	{}
}
