/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/14 22:18:14 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HANDLER_HPP
# define HTTPREQUEST_HANDLER_HPP 

# include <cstring>
# include <exception>
# include <vector>
# include <list>
# include <fstream>
# include "Http.hpp"
# include "ResponseHeader.hpp"
# include "SocketTypes.hpp"
#include "ConnectionSocket.hpp"
# include "RequestParser.hpp"
#include "VirtServ.hpp"
#include "VirtServInfo.hpp"

namespace ft
{
	namespace http
	{
		class HttpRequest : private ConnectionSocket 
		{
			public:

				class Exception
				{
					public:

						Exception(StatusCode code);

						StatusCode	what() const {return (m_code);}

					private:

						StatusCode m_code;
				};

				HttpRequest(int fd, const VirtServInfo::VirtServMap& virt_serv_map);

				int		recv();
				int		send();

			private:

				/* ################################ Typedefs ################################ */

				typedef void (HttpRequest::*methodInitFnct)();
				typedef void (HttpRequest::*methodHeaderFnct)(ResponseHeader&);
				typedef void (HttpRequest::*methodSendFnct)();

				/* ############################## Nested Class ############################## */

				class MatchingServerName : public std::unary_function<const std::vector<VirtServ*>::value_type, bool>
				{
					public:

						MatchingServerName(const std::string& hostField)
							: m_host_field(hostField) {}

						inline bool	operator()(const std::vector<VirtServ*>::value_type ptr)
						{
							for(std::vector<std::string>::const_iterator it = ptr->m_server_name_vec.begin();
								it != ptr->m_server_name_vec.end();
								++it)
							{
								if (*it == m_host_field)
									return true;
							}
							return false;
						}

					private:

						const std::string&	m_host_field;
				};

				static const methodInitFnct				m_method_init_fnct[http::NbrAvailableMethod];
				static const methodHeaderFnct			m_method_header_fnct[http::NbrAvailableMethod + 1];
				static const methodSendFnct				m_method_send_fnct[http::NbrAvailableMethod + 1];
				static const size_t						Error = 3;

				std::ifstream							m_file_handle;
				RequestParser::HeaderInfo				m_header_info;

				RequestParser							m_header_parser;
				StatusCode								m_status_code;
				const VirtServInfo::VirtServMap&		m_virtserv_map;


				/* ############################ Private function ############################ */

				/* These functions allow us to describe different behavior for each method.
				 *
				 * *Init* functions are called when the request parsing is done : they setup invariants for the rest of the
				 * sending process (ex: with GET, opening a file prior to reading).
				 *
				 * *Header* functions are called when the invariants are sucessfully established, and we can generate a correct
				 * response header (ex: with GET, fill Content-Type and Content-Length header fields).
				 *
				 * *Send* functions are called when we're sending an (optionnal) body to the client.
				 * They describe how we should send the content. */

				inline bool						_state(State state) {return (m_state == state);}
				inline void						_setState(State state, StatusCode code = OK)
				{
					m_state = state;
					m_status_code = code;
				}
				inline bool	_errorState() const
				{
					return (m_status_code != OK);
				}

				void							_setupDefaultHeaderField(ResponseHeader& respHeader);
				void							_setupErrorHeaderField(ResponseHeader& respHeader);
				void							_parseRequestHeaderFields();


				const std::vector<VirtServ*>&	_getBoundedVirtServs();

				void	_methodInitGet();
				void	_methodInitPost();
				void	_methodInitDelete();

				void	_methodHeaderGet(ResponseHeader& header);
				void	_methodHeaderPost(ResponseHeader& header);
				void	_methodHeaderDelete(ResponseHeader& header);
				void	_handleHeaderError(ResponseHeader& header);

				void	_methodSendGet();
				void	_methodSendPost();
				void	_methodSendDelete();
		};
	}
}

#endif
