/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/19 17:04:59 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCLIENT_CLASS_HPP
# define HTTPCLIENT_CLASS_HPP

# include <cstring>
# include "SocketTypes.hpp"

enum HTTPState
{
	NO_CONN,
	FETCHING_DATA,
	READY_FOR_RESPONSE
};

class HTTPClient : public InternetSocket
{
	public:

		HTTPClient();
		~HTTPClient();

		void	init(int fd, bool blocking);
		void	terminate();

		void	appendToBuffer(const char *buffer, size_t bytes);
		const std::string&	getBuffer() const;
		void				setState(HTTPState state);
		bool	operator==(const HTTPClient& rhs);

	private:

		std::string	_M_buffer; // i'm excepting to receive text data !
		HTTPState	_M_state;
};

#endif
