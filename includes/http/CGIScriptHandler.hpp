#ifndef CGISCRIPTHANDLER_HPP
# define CGISCRIPTHANDLER_HPP

# include <cstddef>
# include <sys/types.h>
# include <string>
# include <poll.h>
# include <vector>

# include "Http.hpp"

using std::string;
using std::vector;;

namespace HTTP
{
	class CGIScriptHandler
	{
	public:

		static const string			GatewayInterfaceVer;
		static const size_t			Timeout = 5000;
		static const size_t			BufferSize = 512;

		CGIScriptHandler();
		~CGIScriptHandler();

		void	addMetaVar(const string& var, const string& value);
		void	addArg(const string& arg);

		void						start(const std::string& interpreter, const std::string& script_path, Method m);
		const Buffer&				read();
		size_t						write(const Buffer& buff, Buffer::const_iterator begin);

		void						closeWriteEnd();

		void	setScriptPath(const string& path);

	private:

		CGIScriptHandler(const CGIScriptHandler& other);
		CGIScriptHandler operator=(const CGIScriptHandler& rhs);

	std::vector<char>	_buildMetaVar(const string& var, const std::string& value);

		pid_t			m_cgi_pid;

		std::vector<std::vector<char> >	m_env;
		std::vector<char *>				m_cenv;

		struct pollfd		m_fds[2];
		int				m_write_fd;
		int				m_read_fd;

		Buffer				m_read_buffer;
		Buffer				m_cgi_script_output;

		vector<char>		m_post_data;
		size_t				m_post_data_sent;
	};
}

#endif /* CGISCRIPTHANDLER_HPP */
