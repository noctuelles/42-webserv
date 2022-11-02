/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 16:39:03 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_CLASS_HTPP
# define CLIENT_CLASS_HTPP 

# include <cstring>
# include <exception>
# include <vector>
# include <list>
# include <fstream>
# include "SocketTypes.hpp"
# include "RequestParser.hpp"
# include "HTTP.hpp"

namespace ft
{
	class ListeningSocket;
	class WebServ;

	class Client : public InternetSocket
	{

		public:

			static const size_t		MaxRecvBufferSize = 1024 * 8;
			static const size_t		MaxSendBufferSize = 1024 * 8;

			/* CONNECTION_ESTABLISHED: ba
			 *
			 *
			 * READY_FOR_RESPONSE_BODY: ready to perform operation on the underlying file in a GET or DELETE method.
			 *                          usually, this involve opening the file or deleting it.
			 *
			 * SENDING_RESPONSE_BODY  : sending data to the client.
			 */

			enum	State
			{
				FETCHING_REQUEST_HEADER,
				FETCHING_REQUEST_BODY,
				SENDING_RESPONSE_HEADER,
				SENDING_RESPONSE_BODY,
				DISCONNECT,
				DONE
			};

			Client(int fd);
			Client(int fd, const struct sockaddr_in& sockaddr, socklen_t slen);
			Client(const Client& other);
			~Client();

			Client&	operator=(const Client& rhs);

			virtual int	recv();
			virtual int	send();

			void	disconnect();

		private:

			// Static buffer shared among all instances.
			static std::vector<uint8_t>	m_recv_buffer, m_send_buffer;       // treating all
			ssize_t						m_recv_bytes , m_sent_bytes;

			http::RequestParser			m_parser;

			State						m_state;

			http::StatusInfo			m_status_info;
			http::StatusCode			m_status_code;

			std::basic_ifstream<char>		m_file_handle;
	};
}

#endif
