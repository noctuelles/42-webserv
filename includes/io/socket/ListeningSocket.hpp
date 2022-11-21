/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListeningSocket.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 18:53:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/21 17:38:00 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LISTENINGSOCKET_CLASS_HPP
# define LISTENINGSOCKET_CLASS_HPP

# include "SocketTypes.hpp"

namespace IO
{
	class ListeningSocket : public InternetSocket // see SocketTypes.hpp
	{
		public:

			ListeningSocket();
			ListeningSocket(in_addr_t addr, in_port_t port);
			ListeningSocket(sockaddr_in sockaddr);
			ListeningSocket(const ListeningSocket& other);
			ListeningSocket&	operator=(const ListeningSocket& rhs);
			~ListeningSocket();

			void	bind(in_addr_t addr, in_port_t port);
			void	bind(sockaddr_in); // Should go into SimpleSocket btw
			void	listen(int backlog) const;

			int		recv();
			int		send();
			bool	isTimeout();

	};
}

#endif
