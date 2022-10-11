/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:53:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/11 19:23:15 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include "SimpleSocket.hpp"
# include "ClientSocket.hpp"

class ServerSocket : public SimpleSocket<struct sockaddr_in>
{
	public:

		ServerSocket();
		~ServerSocket();

		void			bind(in_addr_t addr, in_port_t port);
		void			listen(int backlog);
		void			accept(ClientSocket& client_sock);
};

#endif
