/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EPoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 12:38:42 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/19 17:44:42 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EPoll.hpp"
#include <cwchar>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <typeinfo>
#include <algorithm>
#include <unistd.h>


EPoll::EPoll()
	: _M_events(_S_initial_events), _M_registred_fds(0), _M_returned_events_size(0)
{
	_M_epoll_instance = epoll_create(_S_poll_hint_size);
	if (_M_epoll_instance < 0)
		throw (std::runtime_error("epoll_create"));
}

void	EPoll::remove(int fd)
{
	// Since Linux 2.6.9, last argument can be NULL.
	if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw (std::runtime_error("epoll_ctl"));
	_M_registred_fds--;
}

void	EPoll::waitForEvents(int timeout)
{
	int	rc;

	rc = epoll_wait(_M_epoll_instance, _M_events.data(), _M_events.size(), timeout);
	if (rc < 0)
		throw (std::runtime_error("epoll_wait"));
	_M_returned_events_size = static_cast<size_t>(rc);
}

size_t	EPoll::getEventsNbr() const
{
	return (_M_returned_events_size);
}

EPoll::iterator	EPoll::begin()
{
	return (_M_events.begin());
}

EPoll::iterator	EPoll::end()
{
	return (EPoll::iterator(&_M_events[_M_returned_events_size]));
}

EPoll::~EPoll()
{
	close(_M_epoll_instance);
}

void	EPoll::_M_setEventData(struct epoll_event& event, int fd)
{
	event.data.fd = fd;
}

void	EPoll::_M_setEventData(struct epoll_event& event, uint32_t u32)
{
	event.data.u32 = u32;
}

void	EPoll::_M_setEventData(struct epoll_event& event, uint64_t u64)
{
	event.data.u64 = u64;
}

void	EPoll::_M_setEventData(struct epoll_event& event, void *ptr)
{
	event.data.ptr = ptr;
}

