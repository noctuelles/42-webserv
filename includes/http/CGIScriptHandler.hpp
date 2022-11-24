#ifndef CGISCRIPTHANDLER_HPP
# define CGISCRIPTHANDLER_HPP

# include <sys/types.h>
# include <string>
# include <poll.h>
# include <vector>

namespace HTTP
{
	class CGIScriptHandler
	{
	public:

		static const std::string	GatewayInterfaceVer;
		static const size_t			Timeout = 5000;

		class MetaVariable
		{
			public:

				operator const std::string&() const {return (m_str);}

				/* https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.3 */
				static inline MetaVariable	ContentType()		{return (MetaVariable("CONTENT_TYPE"));}
				/* https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.2 */
				static inline MetaVariable	ContentLenght()		{return (MetaVariable("CONTENT_LENGTH"));}
				/* https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.4 */
				static inline MetaVariable	GatewayInterface()	{return (MetaVariable("GATEWAY_INTERFACE"));}
				/* https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.5 */
				static inline MetaVariable	PathInfo()			{return (MetaVariable("PATH_INFO"));}
				/* https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.6 */
				static inline MetaVariable	PathTranslated()	{return (MetaVariable("PATH_TRANSLATED"));}
				static inline MetaVariable	QueryString()		{return (MetaVariable("QUERY_STRING"));}
				static inline MetaVariable	RemoteAddr()		{return (MetaVariable("REMOTE_ADDR"));}
				static inline MetaVariable	RequestMethod()		{return (MetaVariable("REQUEST_METHOD"));}
				static inline MetaVariable	ScriptName()		{return (MetaVariable("SCRIPT_NAME"));}
				static inline MetaVariable	ServerName()		{return (MetaVariable("SERVER_NAME"));}
				static inline MetaVariable	ServerProtocol()	{return (MetaVariable("SERVER_PROTOCOL"));}
				static inline MetaVariable	ServerPort()		{return (MetaVariable("SERVER_PORT"));}
				static inline MetaVariable	ServerSoftware()	{return (MetaVariable("SERVER_SOFTWARE"));}

				const std::string&	str() const {return (m_str);}

			private:

				MetaVariable();
				MetaVariable(const std::string& name);

				std::string	m_str;
		};

		CGIScriptHandler(const std::string& cgi_path);
		~CGIScriptHandler();

		void	addMetaVar(const MetaVariable& var, const std::string& value);
		void	addArg(const std::string& arg);

		void						start(Method m);
		std::vector<unsigned char>	read();
		void						write();

		void	setScriptPath(const std::string& path);

	private:

		CGIScriptHandler(const CGIScriptHandler& other);
		CGIScriptHandler operator=(const CGIScriptHandler& rhs);

		std::string			m_cgi_path;
		std::string			m_script_path;
		pid_t				m_proc_pid;

		std::vector<std::vector<char> >	m_env;
		std::vector<std::vector<char> >	m_argv;

		std::vector<char *>	m_cenv;
		std::vector<char *>	m_cargv;

		struct pollfd		m_fds[2];
		int					m_write_fd;
		int					m_read_fd;

		std::vector<char>	_buildMetaVar(const MetaVariable& var, const std::string& value);
	};
}

#endif
