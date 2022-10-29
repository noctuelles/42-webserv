/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketTypes.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 19:36:33 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/28 14:49:59 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETTYPES_HPP
# define SOCKETTYPES_HPP

# include "SimpleSocket.hpp"

namespace ft
{
	typedef SimpleSocket<struct sockaddr_in>	InternetSocket;
}

#endif
