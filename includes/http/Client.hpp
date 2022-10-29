/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/29 19:35:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_CLASS_HTPP
# define CLIENT_CLASS_HTPP 

# include <cstring>
# include <exception>
# include <vector>
# include <list>
# include "SocketTypes.hpp"
# include "RequestParser.hpp"
# include "HTTP.hpp"

namespace ft
{
	class ListeningSocket;

	class Client : public InternetSocket
	{

		public:

			enum	State
			{
				CONNECTION_ESTABLISHED,
				FETCHING_REQUEST_HEADER,
				FETCHING_REQUEST_BODY, // dismissed if the method is GET or DELETE !
				READY_FOR_RESPONSE_HEADER,
				READY_FOR_RESPONSE_BODY
			};

			Client(int fd, ListeningSocket* sock);
			Client(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket* sock);
			Client(const Client& other);
			~Client();

			Client&	operator=(const Client& rhs);

			void	receive();
			void	send(const std::vector<uint8_t>& buffer);
			State	proceed();

			void										setIterator(const std::list<Client>::iterator& it);
			const std::list<Client>::iterator&	getIterator() const;
			ListeningSocket*							getBindedSocket();
			const http::RequestParser::t_parse_info&	getParseInfo();

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
	};
}

#endif
