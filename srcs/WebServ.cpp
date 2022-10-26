/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 14:56:45 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"
#include <vector>
#include <iostream>

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

EPoll&	WebServ::getPoller()
{
	return (m_poller);
}

bool	WebServ::loop()
{
	assert(m_listener_init);
	if (m_socks.size() == 0)
		return (false);
	std::cout << "Blocking on epoll_wait()...\n";
	m_poller.waitForEvents(EPoll::NOTIMEOUT);
	return (true);
}

WebServ::~WebServ()
{}
