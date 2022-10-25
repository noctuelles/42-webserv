/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/20 09:23:30 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCLIENT_CLASS_HPP
# define HTTPCLIENT_CLASS_HPP

# include <cstring>
# include <list>
# include "SocketTypes.hpp"

enum HTTPState
{
};

class HTTPClient : public InternetSocket
{
	public:

		HTTPClient(int fd);
		HTTPClient(int fd, const struct sockaddr_in& sockaddr, socklen_t slen);
		~HTTPClient();

		void				appendToBuffer(const char *buffer, size_t bytes);
		const std::string&	getBuffer() const;

		void				setState(HTTPState state);

		void									setIterator(const std::list<HTTPClient>::iterator& it);
		const std::list<HTTPClient>::iterator&	getIterator() const;

	private:

		std::string						_M_buffer; // i'm excepting to receive text data !
		std::list<HTTPClient>::iterator	_M_iterator; // this iterator is used to provide constant time for erasing a client from a connection list.
		HTTPState						_M_state;
};

#endif
