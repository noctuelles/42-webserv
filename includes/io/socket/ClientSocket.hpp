/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/09 15:53:32 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_CLASS_HTPP
# define CLIENT_CLASS_HTPP 

# include <cstring>
# include <exception>
# include <vector>
# include <list>
# include <fstream>
#include "HTTP.hpp"
# include "SocketTypes.hpp"
# include "RequestParser.hpp"
#include "VirtServ.hpp"

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

			typedef void (ClientSocket::*methodFnct)();

			static std::vector<uint8_t>	m_recv_buffer;
			static std::vector<uint8_t> m_send_buffer;

			ClientSocket(const ClientSocket& other);
			ClientSocket&	operator=(const ClientSocket& rhs);

			methodFnct								m_method_fnct[http::NbrAvailableMethod];
			ssize_t									m_recv_bytes , m_sent_bytes;
			http::RequestParser						m_parser;
			time_t									m_last_activity;
			State									m_state;
			http::StatusCode						m_status_code;
			const std::vector<http::StatusInfo>&	m_stat_info;
			VirtServ*								m_conn_info;

			http::HeaderFieldMap					m_header_fields;

			void	_matchServer();
			void	_methodGet();
			void	_methodPost();
			void	_methodDelete();
	};
}

#endif
