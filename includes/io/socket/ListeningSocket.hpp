/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:53:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 16:36:42 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENINGSOCKET_CLASS_HPP
# define LISTENINGSOCKET_CLASS_HPP

# include "SimpleSocket.hpp"
# include "SocketTypes.hpp"
# include "EPoll.hpp"
# include "Client.hpp"
# include <set>

namespace ft
{
	class WebServ;

	class ListeningSocket : public InternetSocket // see SocketTypes.hpp
	{
		public:

			enum State
			{
				ADD_TO_POLLER
			};

			ListeningSocket();
			ListeningSocket(in_addr_t addr, in_port_t port);
			ListeningSocket(const ListeningSocket& other);
			ListeningSocket&	operator=(const ListeningSocket& rhs);
			~ListeningSocket();

			void	bind(in_addr_t addr, in_port_t port);
			void	listen(int backlog) const;

			virtual int	recv();
			virtual int	send();

	};
}

#endif
