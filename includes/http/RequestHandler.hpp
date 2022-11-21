/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/21 18:26:14 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HANDLER_HPP
# define HTTPREQUEST_HANDLER_HPP 

# include <cstring>
# include <exception>
#include <functional>
# include <vector>
#include <sys/stat.h>
# include <list>
# include <fstream>
# include "Http.hpp"
# include "ResponseHeader.hpp"
# include "SocketTypes.hpp"
# include "RequestParser.hpp"
# include "VirtServ.hpp"
# include "VirtServInfo.hpp"
# include "AutoIndex.hpp"

using std::pair;
using std::vector;
using std::string;
using std::ifstream;

namespace HTTP
{
	class RequestHandler
	{
		public:

			enum State
			{
				FETCHING_REQUEST_HEADER,
				FETCHING_REQUEST_BODY,
				PROCESSING_RESPONSE_HEADER,
				PROCESSING_RESPONSE_BODY,
				DONE
			};

			enum Type
			{
				FILE,
				CGI,
				AUTOINDEX
			};

			typedef pair<const void*, size_t>	DataInfo;

			class Exception
			{
				public:

					Exception(StatusCode code) :
						m_code(code)
					{}

					StatusCode	what() const {return (m_code);}

				private:

					StatusCode m_code;
			};

			RequestHandler(const VirtServInfo::VirtServMap& virt_serv_map);
			~RequestHandler();

			State		fetchIncomingData(const std::vector<uint8_t>& data_buff, size_t recv_bytes);
			State		prepareOutcomingData();

			void			setConnectionBoundedSocket(const struct sockaddr_in& bounded_sock);

			DataInfo		getDataToSend() const;
			StatusCode		getStatusCode() const;
			const string&	getRequestLine() const;
			const string&	getAbsPath()    const;

		private:

			/* ################################ Typedefs ################################ */

			typedef void (RequestHandler::*methodInitFnct)();
			typedef void (RequestHandler::*methodHeaderFnct)(ResponseHeader&);
			typedef void (RequestHandler::*methodSendFnct)();

			/* ############################## Nested Class ############################## */

			class MatchingServerName : public std::unary_function<const std::vector<VirtServ*>::value_type, bool>
			{
				public:

					MatchingServerName(const string& hostField)
						: m_host_field(hostField) {}

					inline bool	operator()(const vector<VirtServ*>::value_type ptr)
					{
						for(vector<std::string>::const_iterator it = ptr->m_server_name_vec.begin();
							it != ptr->m_server_name_vec.end();
							++it)
						{
							if (*it == m_host_field)
								return true;
						}
						return false;
					}

				private:

					const string&	m_host_field;
			};

			class ValidIndexFile : public std::unary_function<const string&, bool>
			{
				public:

					ValidIndexFile(const string& prefix)
						: m_prefix(prefix) {}

					bool operator()(const string& index)
					{
						struct stat	stat_buf;
						string		path(m_prefix);

						if (::stat(path.append(index).c_str(), &stat_buf) < 0)
						{
							if (errno != ENOENT)
								throw (Exception(InternalServerError));
						}
						else if (!S_ISDIR(stat_buf.st_mode) && (stat_buf.st_mode & S_IRUSR))
							return (true);
						return (false);
					}

				private:

					const string	&m_prefix;
			};

			static const methodInitFnct				m_method_init_fnct[HTTP::NbrAvailableMethod];
			static const methodHeaderFnct			m_method_header_fnct[HTTP::NbrAvailableMethod + 1];
			static const methodSendFnct				m_method_send_fnct[HTTP::NbrAvailableMethod + 1];

			State								m_state;
			Type								m_request_type;
			const VirtServInfo::VirtServMap&	m_virtserv_map;
			const VirtServ*						m_virtserv;
			const VirtServ::RouteOptions*		m_route;
			struct sockaddr_in					m_bounded_sock;

			vector<uint8_t>				m_data_buff;
			const void*					m_data_to_send;
			size_t						m_data_to_send_size;
			string						m_page_to_send;

			ifstream					m_file_handle;
			RequestParser::HeaderInfo	m_header_info;
			RequestParser::UriInfo		m_uri_info;
			RequestParser				m_header_parser;
			StatusCode					m_status_code;
			string						m_ressource_path;

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
			inline void						_setState(State state)
			{
				m_state = state;
			}

			inline void	_setErrorState(State state, StatusCode code)
			{
				_setState(state);
				m_status_code = code;
				m_header_info.method = Err;
			}

			inline bool	_errorState() const
			{
				return (m_status_code != OK);
			}

			const vector<VirtServ*>&	_getBoundedVirtServ();
			void						_parseGeneralHeaderFields();

			bool	_isAReadableRegFile(const char* path);

			void	_methodInitGet();
			void	_methodInitPost();
			void	_methodInitDelete();

			void	_methodHeaderGet(ResponseHeader& header);
			void	_methodHeaderPost(ResponseHeader& header);
			void	_methodHeaderDelete(ResponseHeader& header);
			void	_methodHeaderError(ResponseHeader& header);

			void	_methodSendGet();
			void	_methodSendPost();
			void	_methodSendDelete();
			void	_methodSendError();
	};
}

#endif
