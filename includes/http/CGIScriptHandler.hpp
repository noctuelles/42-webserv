#ifndef CGISCRIPTHANDLER_HPP
# define CGISCRIPTHANDLER_HPP

# include <sys/types.h>
# include <string>
#include <vector>

namespace HTTP
{
	class CGIScriptHandler
	{
	public:

		static const std::string	GatewayInterfaceVer;

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

				const std::string&	str() const {return (m_str);}

			private:

				MetaVariable();
				MetaVariable(const std::string& name);

				std::string	m_str;
		};

		CGIScriptHandler(const std::string& cgi_path);
		~CGIScriptHandler();

		void	setMetaVariable(const MetaVariable& var, const std::string& value);
		void	setScriptPath(const std::string& path);
		void	start();
		size_t	read();

	private:

		CGIScriptHandler(const CGIScriptHandler& other);
		CGIScriptHandler operator=(const CGIScriptHandler& rhs);

		std::string			m_cgi_path;
		std::string			m_script_path;
		pid_t				m_proc_pid;
		std::vector<std::vector<char> >	m_env;

		std::vector<char *>	m_cenv;
		std::vector<char *>	m_cargv;
		int					m_write_fd;
		int					m_read_fd;
	};
}

#endif
