/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:53:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/12 14:46:44 by plouvel          ###   ########.fr       */
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
		void			allowReusable() const;

};

#endif
