/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Process.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 19:29:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 19:29:55 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROCESS_HPP
# define PROCESS_HPP

# include <unistd.h>
# include <stdexcept>

/* T is a type, and Process has-a reference to that type.
 * That reference is shared by the child and the parent process.
 * Pure virtual function are defined so a client can define a specific behavior for a derived class. */

template <typename T>
	class Process
	{
		public:

			explicit Process(T& data) :
				m_data(data),
				m_pid(-1)
			{}

			virtual ~Process()
			{}

			virtual void			fork()
			{
				m_pid = ::fork();
				if (m_pid < 0)
					throw std::runtime_error("::fork");
				else if (m_pid == 0)
					inChild();
				else
					inParent();
			}

		private:

			Process(const Process& other);
			template <typename C>
				Process<T>	operator=(const Process<C>& rhs);

		protected:

			virtual void	inChild() = 0;
			virtual void	inParent() = 0;

			T&		m_data;
			pid_t	m_pid;
	};

#endif
