/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EPoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 12:38:42 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/18 13:34:09 by plouvel          ###   ########.fr       */
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

void	EPoll::add(int fd)
{
	if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_ADD, fd,  &_M_curr_event) < 0)
		throw (std::runtime_error("epoll_ctl"));
	_M_registred_fds++;
	if (_M_registred_fds > _M_events.size())
		_M_events.resize(_M_events.size() * 2, epoll_event());
}

void	EPoll::remove(int fd)
{
	// Since Linux 2.6.9, last argument can be NULL.
	if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw (std::runtime_error("epoll_ctl"));
	_M_registred_fds--;
}

void	EPoll::modify(int fd)
{
	// don't use this function
	if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_MOD, fd, &_M_curr_event) < 0)
		throw (std::runtime_error("epoll_ctl"));
}

void	EPoll::setEventData(int fd)
{
	_M_curr_event.data.fd = fd;
}

void	EPoll::setEventData(uint32_t u32)
{
	_M_curr_event.data.u32 = u32;
}

void	EPoll::setEventData(uint64_t u64)
{
	_M_curr_event.data.u64 = u64;
}

void	EPoll::setEventData(void *ptr)
{
	_M_curr_event.data.ptr = ptr;
}

void	EPoll::setEventMask(uint32_t events)
{
	_M_curr_event.events = events;
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

EPoll::iterator	EPoll::beginEvent()
{
	return (_M_events.begin());
}

EPoll::iterator	EPoll::endEvent()
{
	return (EPoll::iterator(&_M_events[_M_returned_events_size]));
}

EPoll::~EPoll()
{
	close(_M_epoll_instance);
}
