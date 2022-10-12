/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 19:13:51 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/12 13:39:22 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTSOCKET_HPP
# define CLIENTSOCKET_HPP

# include "SimpleSocket.hpp"
# include <sys/select.h>

class ClientSocket : public SimpleSocket<struct sockaddr_in>
{
	public:

		ClientSocket();
		~ClientSocket();

		void	setFd(int fd);
};

#endif
