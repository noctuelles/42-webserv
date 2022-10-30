/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 21:35:42 by plouvel          ###   ########.fr       */
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

				SENDING_RESPONSE_ERROR_HEADER,
				SENDING_RESPONSE_ERROR_BODY,

				SENDING_RESPONSE_HEADER,
				SENDING_RESPONSE_BODY,

				DONE
			};

			Client(int fd, ListeningSocket* sock);
			Client(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket* sock);
			Client(const Client& other);
			~Client();

			Client&	operator=(const Client& rhs);

			void	receive();
			State	send(const WebServ& servInstance);

			State	proceed();

			void										setIterator(const std::list<Client>::iterator& it);
			const std::list<Client>::iterator&			getIterator() const;
			ListeningSocket*							getBindedSocket();

			static const size_t							MaxBufferSize = 2048;

		private:

			// Static buffer shared among all instances.
			static std::vector<uint8_t>	m_recv_buffer;       // treating all

			static std::string			m_buffer;

			ssize_t						m_recv_bytes;
			http::RequestParser			m_parser;

			std::list<Client>::iterator	m_iterator;
			State						m_state;
			ListeningSocket*			m_socket;

			http::StatusCode			m_status_code;

			static std::vector<uint8_t>		m_file_buffer;
			std::basic_ifstream<char>	m_file_handle;
	};
}

#endif
