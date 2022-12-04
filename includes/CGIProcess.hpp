/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIProcess.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 19:30:02 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 19:30:04 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIPROCESS_HPP
# define CGIPROCESS_HPP

# include "Process.hpp"
# include "Http.hpp"
# include "FileDescriptor.hpp"
# include <poll.h>

# include <vector>

namespace HTTP
{
	struct CGIProcessInfo
	{
		std::string			interpreter;
		std::vector<char*>	env;
		char* 				argv[3];
	};

	class CGIProcess : public Process<const CGIProcessInfo>
	{
		public:

			static const size_t	TimeoutThreshold = 2000;

			class Timeout : public std::exception
			{
				const char*	what() const throw() {return "Process has timeout.";}
			};

			CGIProcess(const CGIProcessInfo& info, Method m);
			~CGIProcess();

			void	fork();

			Buffer	recvFromChild();
			size_t	writeToChild(const Buffer& buff, Buffer::const_iterator beg);

			void	triggerEOF();

		private:

			void	inChild();
			void	inParent();

			Method			m_method;
			struct pollfd	m_fds[2];
			IO::FdPair		m_child_to_parent;
			IO::FdPair		m_parent_to_child;
	};
}

#endif
