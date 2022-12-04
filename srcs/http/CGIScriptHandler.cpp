/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIScriptHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/04 19:29:21 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 19:36:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"
#include "CGIScriptHandler.hpp"
#include "RequestHandler.hpp"

#include <cassert>

namespace HTTP
{
	const std::string	CGIScriptHandler::GatewayInterfaceVer = "CGI/1.1";

	CGIScriptHandler::CGIScriptHandler() :
		m_env(),
		m_script_info(),
		m_proc(NULL),
		m_header_parser(),
		m_body_len(0),
		m_wrote(0)
	{
		m_proc_info.env.reserve(20);
		m_script_info.output_buffer.reserve(1000*10);
	}

	void	CGIScriptHandler::addMetaVar(const string& var, const std::string& value)
	{
		m_env.push_back(_buildMetaVar(var, value));
		m_proc_info.env.push_back(m_env.back().data());
	}

	void	CGIScriptHandler::start(const std::string& interpreter, const std::string& script_path, Method m)
	{
		assert(!m_proc_info.env.empty());

		m_proc_info.env.push_back(NULL);
		m_proc_info.argv[0] = const_cast<char*>(interpreter.c_str());
		m_proc_info.argv[1] = const_cast<char*>(script_path.c_str());
		m_proc_info.argv[2] = NULL;
		m_proc_info.interpreter = interpreter;

		m_proc = new CGIProcess(m_proc_info, m);

	}

	void	CGIScriptHandler::getOutput()
	{
		assert(m_proc != NULL);

		try
		{
			while (1) // Read the whole CGI Output at once; we except EOF.
			{
				Buffer	buff = m_proc->recvFromChild();
				if (buff.empty())
					break ;
				else
					m_script_info.output_buffer.insert(m_script_info.output_buffer.end(), buff.begin(), buff.end());
			}
		}
		catch (const CGIProcess::Timeout& timeout)
		{
			throw (RequestHandler::Exception(InternalServerError));
		}

		m_script_info.body.first = m_header_parser(m_script_info.output_buffer, m_script_info.output_buffer.begin());
		m_script_info.body.second = m_script_info.output_buffer.end();
		m_script_info.header_field = m_header_parser.get();
		m_script_info.content_length = std::distance(m_script_info.body.first, m_script_info.body.second);
	}

	bool	CGIScriptHandler::write(const Buffer& buff, Buffer::const_iterator begin)
	{
		assert(m_proc != NULL);

		size_t	bytes_wrote = m_proc->writeToChild(buff, begin);

		m_wrote += bytes_wrote;

		if (m_wrote == m_body_len)
		{
			m_proc->triggerEOF();
			return (true);
		}
		else
			return (false);
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
		if (m_proc)
			delete (m_proc);
	}
}
