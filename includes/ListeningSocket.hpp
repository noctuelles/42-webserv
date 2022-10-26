/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:53:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 13:23:42 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENINGSOCKET_CLASS_HPP
# define LISTENINGSOCKET_CLASS_HPP

# include "SimpleSocket.hpp"
# include "SocketTypes.hpp"
# include "HTTPClient.hpp"
# include "EPoll.hpp"
# include <vector>
# include <list>

class ListeningSocket : public InternetSocket // see SocketTypes.hpp
{
	public:

		ListeningSocket();
		ListeningSocket(in_addr_t addr, in_port_t port);
		~ListeningSocket();

		void	bind(in_addr_t addr, in_port_t port);
		void	listen(int backlog);
		void	acceptConnection(EPoll& epollInstance);
		void	removeConnection(HTTPClient* clientPtr);

	private:

		std::list<HTTPClient>	m_con;

};

#endif
