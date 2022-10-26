/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 14:51:47 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_CLASS_HTPP
# define CLIENTSOCKET_CLASS_HTPP 

# include <cstring>
# include <list>
# include "SocketTypes.hpp"
# include "RequestParser.hpp"

class ListeningSocket;

class ClientSocket : public InternetSocket
{

	public:

		ClientSocket(int fd, ListeningSocket* sock);
		ClientSocket(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket* sock);
		ClientSocket(const ClientSocket& other);
		ClientSocket&	operator=(const ClientSocket& rhs);
		~ClientSocket();

		void										setIterator(const std::list<ClientSocket>::iterator& it);
		const std::list<ClientSocket>::iterator&	getIterator() const;
		ListeningSocket*							getBindedSocket();

	private:

		std::list<ClientSocket>::iterator	m_iterator; // this iterator is used to provide constant time for erasing a client from a connection list.
		ListeningSocket*					m_socket;
};

#endif
