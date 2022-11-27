#include "CGIScriptHandler.hpp"
#include "FileDescriptor.hpp"
#include "RequestHandler.hpp"
#include "Utils.hpp"

#include <cassert>
#include <string>
#include <sys/wait.h>

namespace HTTP
{
	const std::string	CGIScriptHandler::GatewayInterfaceVer = "CGI/1.1";

	CGIScriptHandler::CGIScriptHandler() :
		m_cgi_path(),
		m_proc_pid(-1),
		m_env(),
		m_argv(),
		m_cenv(),
		m_cargv(),
		m_fds(),
		m_write_fd(-1),
		m_read_fd(-1)
	{
		m_env.reserve(12);
		m_cenv.reserve(12);

		m_fds[0].fd = m_read_fd;
		m_fds[1].fd = m_write_fd;
	}

	CGIScriptHandler::CGIScriptHandler(const std::string& cgi_path) :
		m_cgi_path(cgi_path),
		m_proc_pid(-1),
		m_env(),
		m_argv(),
		m_cenv(),
		m_cargv(),
		m_fds(),
		m_write_fd(-1),
		m_read_fd(-1)
	{
		m_env.reserve(12);
		m_cenv.reserve(12);

		m_argv.reserve(2);
		m_cargv.reserve(2);

		m_cargv.push_back((char*)m_cgi_path.data());

		m_fds[0].fd = m_read_fd;
		m_fds[1].fd = m_write_fd;
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

	void	CGIScriptHandler::start(Method )
	{
		// NULL terminate env
		m_cenv.push_back(NULL);
		m_cargv.push_back(NULL);

		int input_pipe[2];
		int	output_pipe[2];

		if (pipe(input_pipe) < 0)
			throw (RequestHandler::Exception(InternalServerError));
		if (pipe(output_pipe) < 0)
			throw (RequestHandler::Exception(InternalServerError));

		m_proc_pid = ::fork();
		if (m_proc_pid < 0)
			throw (RequestHandler::Exception(InternalServerError));
		else if (m_proc_pid == 0) // Child reads input pipe and write ouptut pipe
		{
			::close(output_pipe[READ_END]); // Child does not read from output pipe
			::close(input_pipe[WRITE_END]); // Child does write to input pipe
											
			// input pipe read end must become stdin
			if (::dup2(input_pipe[READ_END], STDIN_FILENO) < 0)
				throw (std::runtime_error("::dup2"));
			::close(input_pipe[READ_END]);

			// output pipe write end must become stdout
			if (::dup2(output_pipe[WRITE_END], STDOUT_FILENO) < 0)
				throw (std::runtime_error("::dup2"));
			::close(output_pipe[WRITE_END]);

			if (not m_cgi_path.empty())
				::execve(m_cgi_path.c_str(), m_cargv.data(), m_cenv.data());
			else
				::execve(*(m_cargv.data()), m_cargv.data(), m_cenv.data());

			throw (std::runtime_error("::execve"));
		}
		else // Parent writes to input pipe and reads from output pipe
		{
			::close(input_pipe[READ_END]); // Parent does not read from input pipe
			::close(output_pipe[WRITE_END]); // Parent does not write to output pipe

			m_write_fd = input_pipe[WRITE_END];
			m_read_fd = output_pipe[READ_END];
			/* set up m_fds for poll() */
			m_fds[0].fd = m_read_fd;
		}
	}
#undef READ_END
#undef WRITE_END

	std::vector<unsigned char>	CGIScriptHandler::read()
	{
		int	nfds;

		nfds = ::poll(m_fds, 2, Timeout);
		if (nfds == 0)
		{

		}
	}

	std::vector<char>	CGIScriptHandler::_buildMetaVar(const string& var, const std::string& value)
	{
		std::vector<char>	cvar(var.begin(), var.end());

		cvar.push_back('=');
		cvar.insert(cvar.end(), value.begin(), value.end());
		cvar.push_back('\0');
		return (cvar);
	}
}
