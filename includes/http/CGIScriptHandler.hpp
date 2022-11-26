#ifndef CGISCRIPTHANDLER_HPP
# define CGISCRIPTHANDLER_HPP

# include <sys/types.h>
# include <string>
# include <poll.h>
# include <vector>

# include "Http.hpp"

using std::string;

namespace HTTP
{
	class CGIScriptHandler
	{
	public:

		static const string	GatewayInterfaceVer;
		static const size_t			Timeout = 5000;

		CGIScriptHandler();
		CGIScriptHandler(const string& cgi_path);

		void	addMetaVar(const string& var, const string& value);
		void	addArg(const string& arg);

		void						start(Method m);
		std::vector<unsigned char>	read();
		void						write();

		void	setScriptPath(const string& path);

	private:

		CGIScriptHandler(const CGIScriptHandler& other);
		CGIScriptHandler operator=(const CGIScriptHandler& rhs);

		string			m_cgi_path;
		string			m_script_path;
		pid_t				m_proc_pid;

		std::vector<std::vector<char> >	m_env;
		std::vector<std::vector<char> >	m_argv;

		std::vector<char *>	m_cenv;
		std::vector<char *>	m_cargv;

		struct pollfd		m_fds[2];
		int					m_write_fd;
		int					m_read_fd;

		std::vector<char>	_buildMetaVar(const string& var, const string& value);
	};
}

#endif
