/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/11 16:37:38 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_CLASS_HTPP
# define CLIENT_CLASS_HTPP 

# include <cstring>
# include <exception>
# include <vector>
# include <list>
# include <fstream>
# include "HTTP.hpp"
# include "ResponseHeader.hpp"
# include "SocketTypes.hpp"
# include "RequestParser.hpp"
#include "VirtServ.hpp"
#include "VirtServInfo.hpp"

namespace ft
{
	class ClientSocket : public InternetSocket
	{

		public:

			static const size_t		MaxRecvBufferSize = 1024 * 8;
			static const size_t		MaxSendBufferSize = 1024 * 8;

			// State are negative to avoid overlapping with file descriptors.
			// See WebServ::run()
			enum	State
			{
				FETCHING_REQUEST_HEADER = -10,
				FETCHING_REQUEST_BODY,
				SENDING_RESPONSE_HEADER,
				SENDING_RESPONSE_BODY,
				DISCONNECT
			};

			/* CONNECTION_ESTABLISHED: ba
			 *
			 *
			 * READY_FOR_RESPONSE_BODY: ready to perform operation on the underlying file in a GET or DELETE method.
			 *                          usually, this involve opening the file or deleting it.
			 *
			 * SENDING_RESPONSE_BODY  : sending data to the client.
			 */

			ClientSocket(int fd, const std::vector<http::StatusInfo>& stat_info);

			int		recv();
			int		send();
			bool	isTimeout();

		private:

			/* ################################ Typedefs ################################ */

			typedef void (ClientSocket::*methodInitFnct)();
			typedef void (ClientSocket::*methodHeaderFnct)(http::ResponseHeader&);
			typedef void (ClientSocket::*methodSendFnct)();

			class MatchingServerName : public std::unary_function<const std::vector<VirtServ*>::value_type, bool>
			{
				public:

					MatchingServerName(const std::string& hostField)
						: m_host_field(hostField) {}

					inline bool	operator()(const std::vector<VirtServ*>::value_type ptr)
					{
						return (m_host_field == ptr->m_server_name);
					}

				private:

					const std::string&	m_host_field;
			};

			ClientSocket(const ClientSocket& other);
			ClientSocket&	operator=(const ClientSocket& rhs);

			static const methodInitFnct					m_method_init_fnct[http::NbrAvailableMethod];
			static const methodHeaderFnct				m_method_header_fnct[http::NbrAvailableMethod];
			static const methodSendFnct					m_method_send_fnct[http::NbrAvailableMethod];

			std::vector<uint8_t>					m_recv_buffer, m_send_buffer;
			ssize_t									m_recv_bytes , m_sent_bytes;
			std::ifstream							m_file_handle;

			http::RequestParser						m_parser;
			time_t									m_last_activity;
			State									m_state;
			http::StatusCode						m_status_code;
			const std::vector<http::StatusInfo>&	m_stat_info;
			const VirtServ*							m_conn_info;

			http::HeaderFieldMap					m_header_fields;

			const std::vector<VirtServ*>&	_getBoundedVirtServs();

			/* ############################ Private function ############################ */

			void	_methodInitGet();
			void	_methodInitPost();
			void	_methodInitDelete();

			void	_methodHeaderGet(http::ResponseHeader& header);
			void	_methodHeaderPost(http::ResponseHeader& header);
			void	_methodHeaderDelete(http::ResponseHeader& header);

			void	_methodSendGet();
			void	_methodSendPost();
			void	_methodSendDelete();
	};
}

#endif
