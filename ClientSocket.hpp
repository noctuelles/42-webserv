/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:13:51 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/11 19:28:18 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include "SimpleSocket.hpp"

class ClientSocket : public SimpleSocket<struct sockaddr_in>
{
	public:

		ClientSocket();
		~ClientSocket();

		void	setFd(int fd);
};

#endif
