#include "FileDescriptor.hpp"
#include "Http.hpp"
#include "CGIScriptHandler.hpp"
#include "FileDescriptor.hpp"
#include "Http.hpp"
#include "RequestHandler.hpp"
#include "Utils.hpp"
#include "Log.hpp"

#include <cassert>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <iterator>
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
		m_cgi_pid(-1),
		m_env(),
		m_cenv(),
		m_fds(),
		m_write_fd(),
		m_read_fd(),
		m_read_buffer(BufferSize),
		m_script_info(),
		m_header_parser()
	{
		m_env.reserve(12);
		m_cenv.reserve(12);

		m_fds[1].fd = m_write_fd = -1;
		m_fds[0].fd = m_read_fd = -1;
		m_fds[1].events = POLLOUT;
		m_fds[0].events = POLLIN;
	}

	void	CGIScriptHandler::addMetaVar(const string& var, const std::string& value)
	{
		m_env.push_back(_buildMetaVar(var, value));
		m_cenv.push_back(m_env.back().data());
	}

#define READ_END 0
#define WRITE_END 1

	void	CGIScriptHandler::start(const std::string& interpreter, const std::string& script_path, Method m)
	{
		char* const argv[3] = {
			"/usr/local/bin/php-cgi",
			"www/demo_website/info.php",
			NULL
		};

		// NULL terminate env
		m_cenv.push_back(NULL);

		int input_pipe[2] = {-1, -1};
		int	output_pipe[2] = {-1, -1};

		if (m == HTTP::Post)
			if (::pipe(input_pipe) < 0)
				throw std::runtime_error("pipe");
		if (::pipe(output_pipe) < 0)
			throw std::runtime_error("pipe");

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
						throw std::runtime_error("dup2");
					::close(input_pipe[READ_END]);
				}

				 // Child does not read from output pipe
				::close(output_pipe[READ_END]);
				// output pipe write end must become stdout
				if (::dup2(output_pipe[WRITE_END], STDOUT_FILENO) < 0)
					throw std::runtime_error("dup2");
				::close(output_pipe[WRITE_END]);
				::execve(interpreter.c_str(), argv, m_cenv.data());
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
			m_fds[1].fd = m_write_fd;
			m_fds[0].fd = m_read_fd;
		}
	}

#undef READ_END
#undef WRITE_END

	void	pullInfo(short revents)
	{
		if (revents & POLLIN)
			std::cerr << "POLLIN ";
		if (revents & POLLHUP)
			std::cerr << "POLLHUP ";
		if (revents & POLLERR)
			std::cerr << "POLLERR ";
		std::cerr << '\n';
	}

	void	CGIScriptHandler::readOutput()
	{
		int	nfds;
		ssize_t r;

		// while loop is here to send all data before making non blocking reads on the ouptut
		while (1)
		{
			nfds = ::poll(m_fds, 2, -1);
			if (nfds == 0) // timed out
			{
				kill(m_cgi_pid, SIGTERM);
				throw RequestHandler::Exception(InternalServerError);
			}
			if (m_fds[0].revents) // Must before POLLHUP 
			{
				if (m_fds[0].revents & POLLIN)
				{
					// poll guarantees r > -1 because pipe has only one consumer
					m_read_buffer.resize(BufferSize);
					r = ::read(m_read_fd, m_read_buffer.data(), m_read_buffer.size()); 
					if (r == 0)
					{
						::kill(m_cgi_pid, SIGTERM);
						break ;
					}
					else if (r < 0)
						;
					else
					{
						m_read_buffer.resize(r);
						m_script_info.output_buffer.insert(m_script_info.output_buffer.end(), m_read_buffer.begin(), m_read_buffer.end());
					}
				}
				else if (m_fds[0].revents & POLLHUP)
				{
					::close(m_read_fd);
					break ;
				}
			}
		}
		m_script_info.body.first = m_header_parser(m_script_info.output_buffer, m_script_info.output_buffer.begin());
		m_script_info.body.second = m_script_info.output_buffer.end();
		m_script_info.header_field = m_header_parser.get();
		m_script_info.content_length = std::distance(m_script_info.body.first, m_script_info.body.second);
	}

	size_t	CGIScriptHandler::write(const Buffer& buff, Buffer::const_iterator begin)
	{
		int		nfds;
		ssize_t	bytes_wrote = 0;
		std::iterator_traits<Buffer::const_iterator>::difference_type	to_write = std::distance(begin, buff.end());

		if (to_write == 0)
			return (0);
		nfds = ::poll(m_fds, 2, -1);
		if (nfds == 0) // timed out
		{
			kill(m_cgi_pid, SIGTERM);
			throw RequestHandler::Exception(InternalServerError);
		}
		if (m_fds[1].revents) // Must before POLLHUP 
		{
			if (m_fds[1].revents & POLLOUT)
			{
				bytes_wrote = ::write(m_write_fd, reinterpret_cast<const void*>(begin.base()), to_write);
			}
		}

		return (bytes_wrote);
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
