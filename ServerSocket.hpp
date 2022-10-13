/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:53:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/12 19:45:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP
# define SERVERSOCKET_HPP

# include "SimpleSocket.hpp"
# include "SocketTypes.hpp"

class ServerSocket : public InternetSocket // see SocketTypes.hpp
{
	public:

		ServerSocket();
		~ServerSocket();

		void	bind(in_addr_t addr, in_port_t port);
		void	listen(int backlog);
		int	accept();

};

#endif
