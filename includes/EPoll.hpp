/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EPoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 11:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/18 13:34:20 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <vector>
# include <sys/epoll.h>

class EPoll
{
	public:

		typedef std::vector<struct epoll_event>::iterator		iterator;
		typedef std::vector<struct epoll_event>::const_iterator	const_iterator;

		EPoll();
		~EPoll();

		/* ######################### EPoll wrapper function ######################### */

		/* Add an entry to the set of the epoll file descriptor. */
		void	add(int fd);

		/* Remove (deregister) the target file descriptor fd from the epoll set. */
		void	remove(int fd);

		/* Change the settings associated with fd in the interest list to the new settings.*/
		void	modify(int fd);

		/* ########################### Event modification ########################### */

		/* Different overload for each union members. */

		void	setEventData(int fd);
		void	setEventData(void *ptr);
		void	setEventData(uint32_t u32);
		void	setEventData(uint64_t u64);
		void	setEventMask(uint32_t events);

		/* The T parameter should be initialize to the following types: int, void *, uint32_t, uint64_t. */
		template <class T>
			void setEvent(uint32_t events, T data)
			{
				setEventMask(events);
				setEventData(data);
			}

		void	waitForEvents(int timeout);
		size_t	getEventsNbr() const;

		iterator		beginEvent();
		iterator		endEvent();
		const_iterator	beginEvent() const;
		const_iterator	endEvent() const;

		static const int NOTIMEOUT = -1;

	private:

		EPoll(const EPoll& other);
		EPoll&	operator=(const EPoll& rhs);

		static const int	_S_poll_hint_size = 1024; // as a hint for the kernel of how many fds gonna be registered with epoll_create()
													  // note that since linux 2.6.8, the hint argument is ignored.
													  // this argument only exist today for backward compatibility with other kernel.
		static const size_t	_S_initial_events = 1024;

		std::vector<struct epoll_event>	_M_events;
		struct epoll_event				_M_curr_event;
		int								_M_epoll_instance; // is an fd.
		size_t							_M_registred_fds;

		size_t							_M_returned_events_size;
};

#endif
