/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:53:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/19 17:52:00 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENINGSOCKET_CLASS_HPP
# define LISTENINGSOCKET_CLASS_HPP

# include "SimpleSocket.hpp"
# include "SocketTypes.hpp"
# include "HTTPClient.hpp"
# include <vector>

class ListeningSocket : public InternetSocket // see SocketTypes.hpp
{
	public:

		ListeningSocket();
		ListeningSocket(in_addr_t addr, in_port_t port);
		~ListeningSocket();

		HTTPClient&	operator[](size_t i);

		void	bind(in_addr_t addr, in_port_t port);
		void	listen(int backlog);
		void	acceptConnections();

	private:

		static const size_t		_M_default_con_size = 1024;
		std::vector<HTTPClient>	_M_con;

};

#endif
