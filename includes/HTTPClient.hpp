/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 14:23:54 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/17 18:35:32 by plouvel          ###   ########.fr       */
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

	private:

		std::string	_M_buffer;
};

#endif
