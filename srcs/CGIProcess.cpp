#include "CGIProcess.hpp"
#include "Utils.hpp"
#include "StatusInfoPages.hpp"
#include "Log.hpp"

#include <sys/wait.h>
#include <string.h>
#include <iostream>
#include <stdexcept>
#include <cassert>

namespace HTTP
{
	CGIProcess::CGIProcess(const CGIProcessInfo& info, Method m) :
		Process(info),
		m_method(m),
		m_fds(),
		m_child_to_parent(IO::FileDescriptor(-1), IO::FileDescriptor(-1)),
		m_parent_to_child(IO::FileDescriptor(-1), IO::FileDescriptor(-1))
	{
		m_fds[0].fd = -1;
		m_fds[1].fd = -1;
		m_fds[0].events = POLLIN;
		m_fds[1].events = POLLOUT;
		fork();
	}

	CGIProcess::~CGIProcess()
	{
		if (m_pid > 0)
		{
			int	status;

			::waitpid(m_pid, &status, WNOHANG);
			if (WIFEXITED(status))
			{
				Log().get(INFO)
					<< "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_pid << Color::Modifier::rst() << "]"
					<< " has exited with return code (" << Color::Modifier(1, Color::FG_CYAN) << WEXITSTATUS(status) << Color::Modifier::rst() <<  ").\n";
			}
			else if (WIFSIGNALED(status))
			{
				if (WCOREDUMP(status))
					Log().get(WARNING)
						<< "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_pid << Color::Modifier::rst() << "]"
						<< " crashed.\n";
				else
					Log().get(WARNING)
						<< "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_pid << Color::Modifier::rst() << "]"
						<< " has been terminated by signal.\n";
			}
			else
			{
				Log().get(WARNING)
					<< "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_pid << Color::Modifier::rst() << "]"
					<< " has been killed.\n";
				::kill(m_pid, SIGKILL);
			}
		}
	}

	void	CGIProcess::fork()
	{
		m_child_to_parent = Utils::pipe();
		if (m_method == Post)
			m_parent_to_child = Utils::pipe();

		Process::fork();

		Log().get(INFO)
			<< "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_pid << Color::Modifier::rst() << "]"
			<< " launched.\n";
	}

	void	CGIProcess::inChild()
	{
		try
		{
			if (m_method == Post)
			{
				m_parent_to_child.second.release();
				if (::dup2(m_parent_to_child.first, STDIN_FILENO) < 0)
					throw std::runtime_error("::dup2");
				m_parent_to_child.first.release();
			}

			m_child_to_parent.first.release();
			if (::dup2(m_child_to_parent.second, STDOUT_FILENO) < 0)
				throw std::runtime_error("::dup2");
			if (::dup2(m_child_to_parent.second, STDERR_FILENO) < 0)
				throw std::runtime_error("::dup2");
			m_child_to_parent.second.release();
		}
		catch (const std::runtime_error& e)
		{
			::exit(1);
		}

		::execve(m_data.interpreter.c_str(), m_data.argv, m_data.env.data());

		std::cout << "Status: " << StatusInfoPages::get()[InternalServerError].phrase << "\r\n"
			<< "Content-Type: text/html\r\n\r\n"
			<< StatusInfoPages::get()[InternalServerError].page;

		::exit(1);
	}

	void	CGIProcess::inParent()
	{
		m_child_to_parent.second.release();
		m_parent_to_child.first.release();

		m_fds[0].fd = m_child_to_parent.first;
		m_fds[1].fd = m_parent_to_child.second;
	}

	Buffer CGIProcess::recvFromChild()
	{
		assert(m_pid > 0);

		Buffer	buff(2048);
		int		nfds = ::poll(m_fds, 2, TimeoutThreshold);

		if (nfds == 0)
		{
			Log().get(FATAL)
				<< "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_pid << Color::Modifier::rst() << "]"
				<< " has timeout.\n";
			throw Timeout();
		}
		else if (nfds < 0)
			throw std::runtime_error("::poll");
		else
		{
			if (m_fds[0].revents & POLLIN)
			{
				ssize_t	bytes_readed = ::read(m_child_to_parent.first, buff.data(), buff.size());
				if (bytes_readed < 0)
					throw std::runtime_error("::read");
				buff.resize(bytes_readed);
			}
			else if (m_fds[0].revents & POLLHUP)
			{
				m_fds[0].fd = -1;
				buff.resize(0);
			}
		}
		return (buff);
	}

	size_t	CGIProcess::writeToChild(const Buffer& buff, Buffer::const_iterator beg)
	{
		assert(m_pid > 0);

		std::iterator_traits<Buffer::const_iterator>::difference_type	to_write = std::distance(beg, buff.end());
		if (to_write == 0)
			return (0);

		int		nfds = ::poll(m_fds, 2, -1);
		ssize_t	bytes_wrote = 0;

		if (nfds < 0)
			throw std::runtime_error("::poll");
		else
		{
			if (m_fds[1].revents & POLLOUT)
			{
				bytes_wrote = ::write(m_parent_to_child.second, reinterpret_cast<const char*>(beg.base()), to_write);
				if (bytes_wrote < 0)
					throw std::runtime_error("::write");
			}
		}
		return (bytes_wrote);
	}

	void	CGIProcess::triggerEOF()
	{
		m_parent_to_child.second.release();
		m_fds[1].fd = -1;
	}
}
