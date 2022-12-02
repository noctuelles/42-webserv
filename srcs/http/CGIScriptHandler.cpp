#include "FileDescriptor.hpp"
#include "Http.hpp"
#include "CGIScriptHandler.hpp"
#include "FileDescriptor.hpp"
#include "Http.hpp"
#include "RequestHandler.hpp"
#include "Utils.hpp"
#include "Log.hpp"
#include "StatusInfoPages.hpp"

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
#include <sstream>
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
		m_write_fd(-1),
		m_read_fd(-1),
		m_read_buffer(BufferSize),
		m_script_info(),
		m_header_parser(),
		m_body_len(),
		m_wrote()
	{
		m_cenv.reserve(15);

		m_fds[1].fd = m_write_fd;
		m_fds[0].fd = m_read_fd;
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

	static void	closePair(int fds[2])
	{
		close(fds[0]);
		close(fds[1]);
	}

	void	CGIScriptHandler::start(const std::string& interpreter, const std::string& script_path, Method m)
	{
		//NOTE: const_cast is safe here; the kernel will never attempt to modify argv.
		char* const	argv[3] = {
			const_cast<char*>(interpreter.c_str()),
			const_cast<char*>(script_path.c_str()),
			NULL
		};

		int			input_pipe[2]		= {-1, -1};
		int			output_pipe[2]		= {-1, -1};

		m_cenv.push_back(NULL);

		if (m == Post)
		{
			if (::pipe(input_pipe) < 0)
				throw (RequestHandler::Exception(InternalServerError));
		}
		if (::pipe(output_pipe) < 0)
		{
			closePair(input_pipe);
			throw (RequestHandler::Exception(InternalServerError));
		}

		m_cgi_pid = ::fork();
		if (m_cgi_pid < 0)
		{
			closePair(input_pipe); closePair(output_pipe);
			throw (RequestHandler::Exception(InternalServerError));
		}
		else if (m_cgi_pid == 0) // Child reads input pipe and write ouptut pipe
		{
			try
			{
				if (m == HTTP::Post)
				{ 
					close(input_pipe[WRITE_END]);
					if (::dup2(input_pipe[READ_END], STDIN_FILENO) < 0)
					{
						close(input_pipe[READ_END]); closePair(output_pipe);
						throw std::runtime_error("dup2");
					}
					close(input_pipe[READ_END]);
				}
				{
					close(output_pipe[READ_END]);
					if (dup2(output_pipe[WRITE_END], STDOUT_FILENO) < 0)
					{
						close(output_pipe[WRITE_END]); closePair(input_pipe);
						throw std::runtime_error("dup2");
					}
					if (dup2(output_pipe[WRITE_END], STDERR_FILENO) < 0)
					{
						close(output_pipe[WRITE_END]); closePair(input_pipe);
						throw std::runtime_error("dup2");
					}
					close(output_pipe[WRITE_END]);
				}

				execve(interpreter.c_str(), argv, m_cenv.data());

				std::cout << "Status: " << StatusInfoPages::get()[InternalServerError].phrase << "\r\n"
					<< "Content-Type: text/html\r\n\r\n"
					<< StatusInfoPages::get()[InternalServerError].page;

				::exit(42);
			}
			catch (std::exception& e)
			{
				Log().get(FATAL) << "Failed to set up CGI in forked process: "
									<< e.what() << ": "
									<< strerror(errno) << ". Exiting...\n";
				::exit(42);
			}
		}
		else
		{
			Log().get(INFO) << "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_cgi_pid << Color::Modifier::rst() << "]"
				<< " launched.\n";

			::close(input_pipe[READ_END]); // Parent does not read from input pipe
			::close(output_pipe[WRITE_END]); // Parent does not write to output pipe

			m_write_fd = m_fds[1].fd = input_pipe[WRITE_END];
			m_read_fd  = m_fds[0].fd = output_pipe[READ_END];
		}
	}

#undef READ_END
#undef WRITE_END

	void	CGIScriptHandler::readOutput()
	{
		int	nfds;
		ssize_t r;

		while (1) // Read the whole CGI Output at once; we except EOF.
		{
			nfds = ::poll(m_fds, 2, Timeout);
			if (nfds == 0)
			{
				Log().get(WARNING) << "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_cgi_pid << Color::Modifier::rst() << "]"
					<< " took too long to send data.\n";
				throw (RequestHandler::Exception(InternalServerError));
			}
			if (m_fds[0].revents)
			{
				if (m_fds[0].revents & POLLERR)
					throw (RequestHandler::Exception(InternalServerError));
				else if (m_fds[0].revents & POLLIN)
				{
					m_read_buffer.resize(BufferSize);
					r = read(m_read_fd, m_read_buffer.data(), m_read_buffer.size()); 
					if (r < 0)
						throw (RequestHandler::Exception(InternalServerError));
					m_read_buffer.resize(r);
					m_script_info.output_buffer.insert(m_script_info.output_buffer.end(), m_read_buffer.begin(), m_read_buffer.end());
				}
				else if (m_fds[0].revents & POLLHUP)
				{
					int	wstatus;

					waitpid(m_cgi_pid, &wstatus, WNOHANG);
					if (!WIFEXITED(wstatus))
						Log().get(WARNING) << "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_cgi_pid << Color::Modifier::rst() << "]"
							<< " finished sending data but didn't end.\n";
					else
					{
						Log().get(INFO) << "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_cgi_pid << Color::Modifier::rst() << "]"
							<< " successfully exited with return code " << Color::Modifier(1, Color::FG_YELLOW) << WEXITSTATUS(wstatus) << Color::Modifier::rst() << ".\n";
						m_cgi_pid = -1;
					}
					break ;
				}
			}
		}
		m_script_info.body.first = m_header_parser(m_script_info.output_buffer, m_script_info.output_buffer.begin());
		m_script_info.body.second = m_script_info.output_buffer.end();
		m_script_info.header_field = m_header_parser.get();
		m_script_info.content_length = std::distance(m_script_info.body.first, m_script_info.body.second);
	}

	bool	CGIScriptHandler::write(const Buffer& buff, Buffer::const_iterator begin)
	{
		int																nfds;
		std::iterator_traits<Buffer::const_iterator>::difference_type	to_write = std::distance(begin, buff.end());
		ssize_t															wrote;

		if (to_write == 0)
			return (false);
		nfds = ::poll(m_fds, 2, -1);
		if (nfds == 0) 
		{
			Log().get(WARNING) << "Child [" << Color::Modifier(2, Color::FG_RED, Color::UNDERLINE) << m_cgi_pid << Color::Modifier::rst() << "]"
					<< " took too long to accept data.\n";
			throw (RequestHandler::Exception(InternalServerError));
		}
		if (m_fds[1].revents)
		{
			if (m_fds[1].revents & POLLERR)
				throw (RequestHandler::Exception(InternalServerError));
			else if (m_fds[1].revents & POLLOUT)
			{
				wrote = ::write(m_write_fd, reinterpret_cast<const void*>(begin.base()), to_write);
				if (wrote < 0)
					throw (RequestHandler::Exception(InternalServerError));
				else
					m_wrote += wrote;
			}
		}
		if (m_wrote == m_body_len)
		{
			close(m_write_fd);
			m_write_fd = m_fds[1].fd = -1;
		}
		return (m_wrote == m_body_len);
	}

	void	CGIScriptHandler::initWriting(size_t content_length)
	{
		m_body_len = content_length;
		m_wrote = 0;
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
		if (m_cgi_pid != -1)
			::kill(m_cgi_pid, SIGKILL);
		if (m_write_fd != -1)
			::close(m_write_fd);
		if (m_read_fd != -1)
			::close(m_read_fd);
	}
}
