/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/28 14:52:07 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_CLASS_HTPP
# define CLIENTSOCKET_CLASS_HTPP 

# include <cstring>
#include <exception>
# include <list>
# include "SocketTypes.hpp"
# include "RequestParser.hpp"
# include "HTTP.hpp"

namespace ft
{
	class ListeningSocket;

	class ClientSocket : public InternetSocket
	{

		public:

			enum	State
			{
				FETCHING_DATA,
				READY_FOR_RESPONSE
			};

			class BadParsing
			{
				BadParsing(ft::http::StatusCode code);
			};

			class Disconnect : public std::exception
			{

			};

			ClientSocket(int fd, ListeningSocket* sock);
			ClientSocket(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket* sock);
			ClientSocket(const ClientSocket& other);
			ClientSocket&	operator=(const ClientSocket& rhs);
			~ClientSocket();
			State	getState() const;

			void										receive();
			void										setIterator(const std::list<ClientSocket>::iterator& it);
			const std::list<ClientSocket>::iterator&	getIterator() const;
			ListeningSocket*							getBindedSocket();

			static const size_t							BufferSize = 2048;

		private:

			static std::string					m_buffer;
			http::RequestParser					m_parser;
			std::list<ClientSocket>::iterator	m_iterator; // this iterator is used to provide constant time for erasing a client from a connection list.
			State								m_state;
			ListeningSocket*					m_socket;
	};
}

#endif
