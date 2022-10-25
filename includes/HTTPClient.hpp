/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 00:13:52 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCLIENT_CLASS_HPP
# define HTTPCLIENT_CLASS_HPP

# include <cstring>
# include <list>
# include "SocketTypes.hpp"
# include "RequestParser.hpp"

class ListeningSocket;

class HTTPClient : public InternetSocket
{

	public:

		HTTPClient(int fd, ListeningSocket& sock);
		HTTPClient(int fd, const struct sockaddr_in& sockaddr, socklen_t slen, ListeningSocket& sock);
		~HTTPClient();

		void									setIterator(const std::list<HTTPClient>::iterator& it);
		const std::list<HTTPClient>::iterator&	getIterator() const;
		ListeningSocket&	getBindedSocket();

	private:

		std::list<HTTPClient>::iterator	_M_iterator; // this iterator is used to provide constant time for erasing a client from a connection list.
		ListeningSocket&				m_socket;
};

#endif
