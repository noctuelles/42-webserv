#include "CGIScriptHandler.hpp"
#include "FileDescriptor.hpp"
#include "RequestHandler.hpp"
#include "Utils.hpp"

#include <cassert>

namespace HTTP
{
	const std::string	CGIScriptHandler::GatewayInterfaceVer = "CGI/1.1";

	CGIScriptHandler::CGIScriptHandler(const std::string& cgi_path) :
		m_cgi_path(cgi_path),
		m_proc_pid(-1),
		m_env(),
		m_cenv(),
		m_cargv()
	{
		m_env.reserve(12);
		m_cenv.reserve(12);
		m_cargv.reserve(2);
	}

	void	CGIScriptHandler::setMetaVariable(const MetaVariable& var, const std::string& value)
	{
		std::vector<char>	cvar(var.str().begin(), var.str().end());

		cvar.push_back('=');
		cvar.insert(cvar.end(), value.begin(), value.end());
		cvar.push_back('\0');
		m_env.push_back(cvar);

		m_cenv.push_back(m_env.back().data());
	}

	void	CGIScriptHandler::setScriptPath(const std::string& path)
	{
		assert(m_script_path.empty());

		m_script_path = path;

		std::vector<char>	argv(m_script_path.begin(), m_script_path.end());
		argv.push_back('\0');

		m_cargv.push_back(argv.data());
	}

	void	CGIScriptHandler::start()
	{
		int parent_to_child[2] = {-1, -1};
		int	child_to_parent[2] = {-1, -1};

		if (pipe(parent_to_child) < 0)
			throw (RequestHandler::Exception(InternalServerError));
		if (pipe(child_to_parent) < 0)
			throw (RequestHandler::Exception(InternalServerError));

		m_cenv.push_back(NULL);
		m_cargv.push_back(NULL);

		m_proc_pid = ::fork();
		if (m_proc_pid < 0)
			throw (RequestHandler::Exception(InternalServerError));
		else if (m_proc_pid == 0)
		{
			::close(child_to_parent[0]);
			::close(parent_to_child[1]);
			if (::dup2(child_to_parent[1], STDOUT_FILENO) < 0)
				throw (std::runtime_error("::dup2"));
			if (::dup2(parent_to_child[0], STDIN_FILENO) < 0)
				throw (std::runtime_error("::dup2"));
			::close(child_to_parent[1]);
			::close(parent_to_child[0]);

			::execve(m_cgi_path.c_str(), m_cargv.data(), m_cenv.data());

			throw (std::runtime_error("::execve"));
		}
		else
		{
			::close(parent_to_child[0]);
			::close(child_to_parent[1]);
			m_write_fd = parent_to_child[1];
			m_read_fd = child_to_parent[0];
		}
	}
}
