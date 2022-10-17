/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EPoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 11:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/17 14:02:48 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <vector>
# include <sys/epoll.h>

class EPoll
{
	public:

		typedef std::vector<struct epoll_event>::iterator	event_iterator;
		typedef std::pair<event_iterator, event_iterator>	event_iterator_pair;

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

		void	setEventData(int fd);
		void	setEventData(void *ptr);

		void	setEventMask(uint32_t events);

		std::pair<event_iterator, event_iterator>	waitForEvents(int timeout);

		static const int NOTIMEOUT = -1;

	private:


		EPoll(const EPoll& other);
		EPoll&	operator=(const EPoll& rhs);

		static const int _M_poll_hint_size = 2048;
		static const size_t _M_initial_events = 1024;

		std::vector<struct epoll_event>	_M_events;
		struct epoll_event	_M_curr_event;
		int _M_epoll_instance;
};

#endif
