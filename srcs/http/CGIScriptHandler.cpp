#include "FileDescriptor.hpp"
#include "Http.hpp"
#include "CGIScriptHandler.hpp"
#include "FileDescriptor.hpp"
#include "Http.hpp"
#include "RequestHandler.hpp"
#include "Utils.hpp"
#include "Log.hpp"
#include "DataInfo.hpp"

#include <cassert>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

using std::vector ;
using std::make_pair;

namespace HTTP
{
	const std::string	CGIScriptHandler::GatewayInterfaceVer = "CGI/1.1";

	CGIScriptHandler::CGIScriptHandler() :
		m_cgi_path(),
		m_cgi_pid(-1),
		m_env(),
		m_argv(),
		m_cenv(),
		m_cargv(),
		m_fds(),
		m_write_fd(m_fds[1].fd),
		m_read_fd(m_fds[0].fd),
		m_read_buffer(BufferSize),
		m_post_data_sent(0)
	{
		m_env.reserve(12);
		m_cenv.reserve(12);

		m_fds[1].fd = -1;
		m_fds[0].fd = -1;
		m_fds[1].events = POLLOUT;
		m_fds[0].events = POLLIN;
	}

	CGIScriptHandler::CGIScriptHandler(const std::string& cgi_path) :
		m_cgi_path(cgi_path),
		m_cgi_pid(-1),
		m_env(),
		m_argv(),
		m_cenv(),
		m_cargv(),
		m_fds(),
		m_write_fd(m_fds[1].fd),
		m_read_fd(m_fds[0].fd),
		m_read_buffer(BufferSize),
		m_post_data_sent(0)
	{
		m_env.reserve(12);
		m_cenv.reserve(12);

		m_argv.reserve(2);
		m_cargv.reserve(2);

		m_cargv.push_back((char*)m_cgi_path.data());

		m_fds[1].fd = -1;
		m_fds[0].fd = -1;
		m_fds[1].events = POLLOUT;
		m_fds[0].events = POLLIN;
	}

	void	CGIScriptHandler::addMetaVar(const string& var, const std::string& value)
	{
		m_env.push_back(_buildMetaVar(var, value));
		m_cenv.push_back(m_env.back().data());
	}

	void	CGIScriptHandler::addArg(const std::string& arg)
	{
		m_argv.push_back(Utils::getCStr(arg));
		m_cargv.push_back(m_argv.back().data());
	}

#define READ_END 0
#define WRITE_END 1

	void	CGIScriptHandler::start(Method m)
	{
		// NULL terminate env
		m_cenv.push_back(NULL);
		m_cargv.push_back(NULL);

		int input_pipe[2] = {-1, -1};
		int	output_pipe[2] = {-1, -1};

		if (m == HTTP::Post)
			if (::pipe(input_pipe) < 0)
				throw std::runtime_error("fork");
		if (::pipe(output_pipe) < 0)
			throw std::runtime_error("fork");

		m_cgi_pid = ::fork();
		if (m_cgi_pid < 0)
			throw std::runtime_error("fork");
		else if (m_cgi_pid == 0) // Child reads input pipe and write ouptut pipe
		{
			try {
				if (m == HTTP::Post)
				{ 
					// Child does write to input pipe
					::close(input_pipe[WRITE_END]);
					// input pipe read end must become stdin
					if (::dup2(input_pipe[READ_END], STDIN_FILENO) < 0)
						throw std::runtime_error("dup2 failed to redirect input pipe read end to stdin in CGI child process");
					::close(input_pipe[READ_END]);
				}

				 // Child does not read from output pipe
				::close(output_pipe[READ_END]);
				// output pipe write end must become stdout
				if (::dup2(output_pipe[WRITE_END], STDOUT_FILENO) < 0)
					throw std::runtime_error("dup2 failed to redirect output pipe write end to stdout in CGI child process");
				::close(output_pipe[WRITE_END]);
#ifdef DEBUG
				{
					std::cerr << "Child process before excve :\n";
					std::cerr << "m_cgi_path :" << m_cgi_path << "\n";
					std::cerr << "m_cargv.data() :\n" ;
					std::vector<char*>::const_iterator it = m_cargv.begin();
					std::vector<char*>::const_iterator end = m_cargv.end();
					for (; it != end ; ++it)
					{
						std::cerr << *it << "\n";
					}
				}
#endif
				if (not m_cgi_path.empty())
					::execve(m_cgi_path.c_str(), m_cargv.data(), m_cenv.data());
				else
					::execve(*(m_cargv.data()), m_cargv.data(), m_cenv.data());
				throw std::runtime_error("execve");
			}
			catch (std::exception& e)
			{
				Log().get(WARNING) << "Failed to set up CGI in forked process: "
									<< e.what() << ": "
									<< strerror(errno)
									<< "\nExiting forked process...\n";
				::exit(42);
			}
		}
		else // Parent writes to input pipe and reads from output pipe
		{
			::close(input_pipe[READ_END]); // Parent does not read from input pipe
			::close(output_pipe[WRITE_END]); // Parent does not write to output pipe

			m_write_fd = input_pipe[WRITE_END];
			m_read_fd = output_pipe[READ_END];

			int flags;
			// Setting non blocking mode server-side for writing
			//if ((flags = fcntl(m_write_fd, F_GETFL)) < 0)
			//    throw (std::runtime_error("fcntl(F_GETFL)"));
			//if (fcntl(m_write_fd, F_SETFL, flags | O_NONBLOCK) < 0)
			//    throw (std::runtime_error("fcntl(F_SETFL)"));
			// Setting non blocking mode server-side for reading
			if ((flags = fcntl(m_read_fd, F_GETFL)) < 0)
				throw (std::runtime_error("fcntl(F_GETFL)"));
			if (fcntl(m_read_fd, F_SETFL, flags | O_NONBLOCK) < 0)
				throw (std::runtime_error("fcntl(F_SETFL)"));
		}
	}
#undef READ_END
#undef WRITE_END

	DataInfo	CGIScriptHandler::read()
	{
		int	nfds;
		ssize_t r;
		ssize_t s;

		// Current policy is send all data then make non blocking reads on the ouptut
		while (1)
		{
			nfds = ::poll(m_fds, 2, -1);
			if (nfds == 0) // timed out
			{
				kill(m_cgi_pid, SIGTERM);
				throw RequestHandler::Exception(InternalServerError);
			}
			if (m_fds[1].revents)
			{
				if (m_fds[1].revents & POLLERR) // The other side has closed reading before we sent all data
				{
					::Log().get(WARNING) << "CGI script " << m_cgi_path << "did not consume all available POST data";
					m_fds[1].fd = -1;
				}
				else if (m_fds[1].revents & POLLOUT) // The other side is blocking, waiting for data
				{
					// poll guarantees that s > 0 right ?
					s = ::write(m_write_fd, m_post_data.data() + m_post_data_sent, m_post_data.size() - m_post_data_sent);
					m_post_data_sent += s;
					if (m_post_data_sent == m_post_data.size())
					{
						::close(m_write_fd);
						m_fds[1].fd = -1;
					}
				}
			}
			else if (m_fds[0].revents & POLLHUP) // When pipe read end reaches EOF, POLLHUP is set
			{
				wait(NULL);
				return make_pair(static_cast<void*>(0), 0);
			}
			else if (m_fds[0].revents & POLLIN)
			{
				// poll guarantees r > -1 because pipe has only one consumer
				r = ::read(m_read_fd, m_read_buffer.data(), BufferSize); 
				return make_pair(m_read_buffer.data(), r);
			}
		}
		throw std::logic_error("CGIScriptHandler::read");
	}

	std::vector<char>	CGIScriptHandler::_buildMetaVar(const string& var, const std::string& value)
	{
		std::vector<char>	cvar(var.begin(), var.end());

		cvar.push_back('=');
		cvar.insert(cvar.end(), value.begin(), value.end());
		cvar.push_back('\0');
		return (cvar);
	}

	CGIScriptHandler::~CGIScriptHandler()
	{
		close(m_write_fd);
		close(m_read_fd);
	}
}
