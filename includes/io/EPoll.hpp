/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EPoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 11:53:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/21 17:37:19 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <vector>
# include <sys/epoll.h>
# include <stdexcept>

# include "FileDescriptor.hpp"

namespace IO
{
	class EPoll : public FileDescriptor
	{
		public:

			typedef std::vector<struct epoll_event>::iterator	iterator;

			static const int	NoTimeout = -1;

			class	Event
			{
				public:

					operator	uint32_t() const {return (m_event);}

					static Event	In()	{return Event(EPOLLIN);}
					static Event	Out()	{return Event(EPOLLOUT);}
					static Event	Hup()	{return Event(EPOLLHUP);}
					static Event	Err()	{return Event(EPOLLERR);}

					Event	operator&(const Event& rhs) {return (Event(m_event & rhs.m_event));}
					Event	operator|(const Event& rhs) {return (Event(m_event | rhs.m_event));}
					Event	operator^(const Event& rhs) {return (Event(m_event ^ rhs.m_event));}

					Event	operator~() {return (Event(~m_event));};

				private:

					Event(uint32_t event)
						: m_event(event)
					{}

					uint32_t m_event;
			};

			EPoll();
			~EPoll();

			/* ######################### EPoll wrapper function ######################### */

			/* Add an entry to the set of the epoll file descriptor. */
			template <class T>
				void	add(int fd, const Event& events, T data)
				{
					struct epoll_event	event;

					event.events = events;
					_setEventData(event, data);
					if (epoll_ctl(m_fd, EPOLL_CTL_ADD, fd,  &event) < 0)
						throw (std::runtime_error("epoll_ctl"));
					m_registred_fds++;
					if (m_registred_fds > m_events.size())
						m_expand = true;
				}

			/* Remove (deregister) the target file descriptor fd from the epoll set. */
			void	remove(int fd);

			/* Change the settings associated with fd in the interest list to the new settings.*/
			template <class T>
				void	modify(int fd, const Event& events, T data)
				{
					struct epoll_event	event;

					event.events = events;
					_setEventData(event, data);
					if (epoll_ctl(m_fd, EPOLL_CTL_MOD, fd, &event) < 0)
						throw (std::runtime_error("epoll_ctl"));
				}

			bool	waitForEvents(int timeout);
			size_t	getEventsNbr() const;

			iterator		begin();
			iterator		end();


		private:

			EPoll(const EPoll& other);
			EPoll&	operator=(const EPoll& rhs);

			void	_setEventData(struct epoll_event& event, int fd);
			void	_setEventData(struct epoll_event& event, void *ptr);
			void	_setEventData(struct epoll_event& event, uint32_t u32);
			void	_setEventData(struct epoll_event& event, uint64_t u64);


			static const size_t	InitialEvents	= 1024;
			static const int	HintSize		= InitialEvents; // as a hint for the kernel of how many fds gonna be registered with epoll_create()

			std::vector<struct epoll_event>	m_events;
			size_t							m_registred_fds;
			size_t							m_returned_events_size;
			bool							m_expand, m_shrink;
	};
}

#endif
