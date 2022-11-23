/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EPoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 12:38:42 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/21 17:37:09 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EPoll.hpp"
#include <stdexcept>
#include <unistd.h>

namespace IO
{
	EPoll::EPoll()
		: FileDescriptor(epoll_create(HintSize)),
		  m_events(InitialEvents),
		  m_registred_fds(0),
		  m_returned_events_size(0),
		  m_expand(false), m_shrink(false)
	{
		if (m_fd < 0)
			throw (std::runtime_error("epoll_create"));
	}

	void	EPoll::remove(int fd)
	{
		// Since Linux 2.6.9, last argument can be NULL.
		if (epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL) < 0)
			throw (std::runtime_error("epoll_ctl"));
		m_registred_fds--;
		if (m_registred_fds < m_events.size() - InitialEvents)
			m_shrink = true;
	}

	bool EPoll::waitForEvents(int timeout)
	{
		int	rc;

		if (m_expand)
			m_events.resize(m_events.size() + InitialEvents, epoll_event()), m_expand = false;
		else if (m_shrink)
			m_events.resize(m_events.size() - InitialEvents, epoll_event()), m_shrink = false;
		rc = epoll_wait(m_fd, m_events.data(), m_events.size(), timeout);
		if (rc < 0)
			throw (std::runtime_error("epoll_wait"));
		m_returned_events_size = static_cast<size_t>(rc);
		return (m_returned_events_size > 0);
	}

	size_t	EPoll::getEventsNbr() const
	{
		return (m_returned_events_size);
	}

	EPoll::iterator	EPoll::begin()
	{
		return (m_events.begin());
	}

	EPoll::iterator	EPoll::end()
	{
		return (EPoll::iterator(&m_events[m_returned_events_size]));
	}

	EPoll::~EPoll()
	{}

	void	EPoll::_setEventData(struct epoll_event& event, int fd)
	{
		event.data.fd = fd;
	}

	void	EPoll::_setEventData(struct epoll_event& event, uint32_t u32)
	{
		event.data.u32 = u32;
	}

	void	EPoll::_setEventData(struct epoll_event& event, uint64_t u64)
	{
		event.data.u64 = u64;
	}

	void	EPoll::_setEventData(struct epoll_event& event, void *ptr)
	{
		event.data.ptr = ptr;
	}
}
