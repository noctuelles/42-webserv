/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:53:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 14:54:44 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENINGSOCKET_CLASS_HPP
# define LISTENINGSOCKET_CLASS_HPP

# include "SimpleSocket.hpp"
# include "ClientSocket.hpp"
# include "SocketTypes.hpp"
# include "EPoll.hpp"
# include <vector>
# include <list>

class ListeningSocket : public InternetSocket // see SocketTypes.hpp
{
	public:

		ListeningSocket();
		ListeningSocket(in_addr_t addr, in_port_t port);
		ListeningSocket(const ListeningSocket& other);
		ListeningSocket&	operator=(const ListeningSocket& rhs);
		~ListeningSocket();

		void	bind(in_addr_t addr, in_port_t port);
		void	listen(int backlog);
		void	acceptConnection(EPoll& epollInstance);
		void	removeConnection(ClientSocket* clientPtr);

	private:

		std::list<ClientSocket>	m_con;

};

#endif
