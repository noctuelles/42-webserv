#ifndef CGISCRIPTHANDLER_HPP
# define CGISCRIPTHANDLER_HPP

# include <cstddef>
# include <sys/types.h>
# include <string>
# include <poll.h>
# include <vector>

# include "Http.hpp"
# include "HeaderFieldParser.hpp"

using std::string;
using std::vector;;

namespace HTTP
{
	class CGIScriptHandler
	{
	public:

		static const string			GatewayInterfaceVer;
		static const size_t			Timeout = 5000;
		static const size_t			BufferSize = 1024*2;

		struct CGIScriptInfo
		{
			size_t												content_length;
			Buffer												output_buffer;
			HeaderFieldMap										header_field;
			std::pair<Buffer::const_iterator, Buffer::const_iterator>	body;
		};

		CGIScriptHandler();
		~CGIScriptHandler();

		void	addMetaVar(const string& var, const string& value);
		void	addArg(const string& arg);

		void				start(const std::string& interpreter, const std::string& script_path, Method m);

		void				readOutput();
		size_t				write(const Buffer& buff, Buffer::const_iterator begin);

		const CGIScriptInfo&	getScriptInfo() {return (m_script_info);}
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
		int					m_write_fd;
		int					m_read_fd;

		Buffer				m_read_buffer;
		CGIScriptInfo		m_script_info;
		HeaderFieldParser	m_header_parser;
	};
}

#endif /* CGISCRIPTHANDLER_HPP */
