/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EPoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 11:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/19 17:44:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <vector>
# include <sys/epoll.h>
# include <stdexcept>

class EPoll
{
	public:

		typedef std::vector<struct epoll_event>::iterator		iterator;

		EPoll();

		/* Stream socket constructor. */
		template <class T>
			EPoll(int sockfd, uint32_t events, T data)
				: _M_events(_S_initial_events), _M_registred_fds(0), _M_returned_events_size(0)
			{
				_M_epoll_instance = epoll_create(_S_poll_hint_size);
				if (_M_epoll_instance < 0)
					throw (std::runtime_error("epoll_create"));
				add(sockfd, events, data);
			}

		~EPoll();

		/* ######################### EPoll wrapper function ######################### */

		/* Add an entry to the set of the epoll file descriptor. */
		template <class T>
			void	add(int fd, uint32_t events, T data)
			{
				struct epoll_event	event;

				event.events = events;
				_M_setEventData(event, data);
				if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_ADD, fd,  &event) < 0)
					throw (std::runtime_error("epoll_ctl"));
				_M_registred_fds++;
				if (_M_registred_fds > _M_events.size())
					_M_events.resize(_M_events.size() * 2, epoll_event());
			}

		/* Remove (deregister) the target file descriptor fd from the epoll set. */
		void	remove(int fd);

		/* Change the settings associated with fd in the interest list to the new settings.*/
		template <class T>
			void	modify(int fd, uint32_t events, T data)
			{
				struct epoll_event	event;

				event.events = events;
				_M_setEventData(event, data);
				if (epoll_ctl(_M_epoll_instance, EPOLL_CTL_MOD, fd, &event) < 0)
					throw (std::runtime_error("epoll_ctl"));
			}

		void	waitForEvents(int timeout);
		size_t	getEventsNbr() const;

		iterator		begin();
		iterator		end();

		static const int NOTIMEOUT = -1;

	private:

		EPoll(const EPoll& other);
		EPoll&	operator=(const EPoll& rhs);

		void	_M_setEventData(struct epoll_event& event, int fd);
		void	_M_setEventData(struct epoll_event& event, void *ptr);
		void	_M_setEventData(struct epoll_event& event, uint32_t u32);
		void	_M_setEventData(struct epoll_event& event, uint64_t u64);

		static const int	_S_poll_hint_size = 1024; // as a hint for the kernel of how many fds gonna be registered with epoll_create()
													  // note that since linux 2.6.8, the hint argument is ignored.
													  // this argument only exist today for backward compatibility with other kernel.
		static const size_t	_S_initial_events = 1024;

		std::vector<struct epoll_event>	_M_events;
		int								_M_epoll_instance; // is an fd.
		size_t							_M_registred_fds;

		size_t							_M_returned_events_size;
};

#endif
