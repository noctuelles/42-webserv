/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/17 20:56:45 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPCLIENT_CLASS_HPP
# define HTTPCLIENT_CLASS_HPP

# include <cstring>
# include "SocketTypes.hpp"

class HTTPClient : public InternetSocket
{
	public:

		HTTPClient();
		HTTPClient(int fd);
		~HTTPClient();

		void	appendToBuffer(const char *buffer, size_t bytes);
		const std::string&	getBuffer() const;
		bool	operator==(const HTTPClient& rhs);

	private:

		enum State
		{
			FETCHING_DATA,
			READY_FOR_RESPONSE
		};

		std::string	_M_buffer; // i'm excepting to receive text data !
};

#endif
