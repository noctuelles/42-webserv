/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIScriptHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 19:29:34 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 19:29:36 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGISCRIPTHANDLER_HPP
# define CGISCRIPTHANDLER_HPP

# include <string>
# include <vector>
# include <deque>

# include "HeaderFieldParser.hpp"
# include "CGIProcess.hpp"

using std::string;
using std::vector;;

namespace HTTP
{
	class CGIScriptHandler
	{
	public:

		static const string			GatewayInterfaceVer;

		struct CGIScriptInfo
		{
			size_t												content_length;
			Buffer												output_buffer;
			HeaderFieldMap										header_field;
			std::pair<Buffer::const_iterator, Buffer::const_iterator>	body;
		};

		CGIScriptHandler();
		~CGIScriptHandler();

		void	start(const std::string& interpreter, const std::string& script_path, Method m);

		void	getOutput();

		void	initWriting(size_t content_length);
		bool	write(const Buffer& buff, Buffer::const_iterator begin);

		void						addMetaVar(const string& var, const string& value);
		const CGIScriptInfo&		getScriptInfo() {return (m_script_info);}

	private:

		CGIScriptHandler(const CGIScriptHandler& other);
		CGIScriptHandler	operator=(const CGIScriptHandler& rhs);

		std::vector<char>	_buildMetaVar(const string& var, const std::string& value);

		std::deque<std::vector<char> >	m_env;

		CGIScriptInfo					m_script_info;
		CGIProcessInfo					m_proc_info;
		CGIProcess*						m_proc;
		HeaderFieldParser				m_header_parser;

		size_t							m_body_len, m_wrote;
	};
}

#endif /* CGISCRIPTHANDLER_HPP */
