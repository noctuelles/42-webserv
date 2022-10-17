/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EPoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 12:38:42 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/17 14:00:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EPoll.hpp"
#include <stdexcept>
#include <utility>
#include <unistd.h>


EPoll::EPoll()
	: _M_events(_M_initial_events), _M_curr_event()
{
	_M_epoll_instance = epoll_create(_M_poll_hint_size);
	if (_M_epoll_instance < 0)
		throw (std::runtime_error("epoll_create"));
}

void	EPoll::add(int fd)
{
	if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_ADD, fd, &_M_curr_event) < 0)
		throw (std::runtime_error("epoll_ctl"));
}

void	EPoll::remove(int fd)
{
	// Since Linux 2.6.9, last argument can be NULL.
	if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_DEL, fd, NULL) < 0)
		throw (std::runtime_error("epoll_ctl"));
}

void	EPoll::modify(int fd)
{
	if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_MOD, fd, &_M_curr_event) < 0)
		throw (std::runtime_error("epoll_ctl"));
}

void	EPoll::setEventData(int fd)
{
	_M_curr_event.data.fd = fd;
}

void	EPoll::setEventData(void *ptr)
{
	_M_curr_event.data.ptr = ptr;
}

void	EPoll::setEventMask(uint32_t events)
{
	_M_curr_event.events = events;
}

EPoll::event_iterator_pair	EPoll::waitForEvents(int timeout)
{
	int	rc;

	rc = epoll_wait(_M_epoll_instance, _M_events.data(), _M_events.size(), timeout);
	if (rc < 0)
		throw (std::runtime_error("epoll_wait"));
	return (std::make_pair(_M_events.begin(),
				EPoll::event_iterator(&_M_events[rc])));
}

EPoll::~EPoll()
{
	close(_M_epoll_instance);
}
