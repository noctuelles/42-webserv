/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 19:10:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 13:20:30 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ()
	: m_poller(), m_socks()
{}

void	WebServ::addListener(in_port_t port)
{
	m_socks.push_back(ListeningSocket(INADDR_ANY, port));
}

void	WebServ::initListener()
{
	std::for_each(m_socks.begin(), m_socks.end(), ListeningSocketInit(m_poller));
}

bool	WebServ::isListener(int fd)
{
	return (std::find_if(m_socks.begin(), m_socks.end(), SocketComp(fd)) != m_socks.end());
}


EPoll&	WebServ::getPoller()
{
	return (m_poller);
}

bool	WebServ::loop()
{
	m_poller.waitForEvents(EPoll::NOTIMEOUT);
	return (true);
}

WebServ::~WebServ()
{}
