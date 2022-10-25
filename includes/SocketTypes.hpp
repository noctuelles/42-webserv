/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketTypes.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 19:36:33 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/17 14:25:15 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETTYPES_HPP
# define SOCKETTYPES_HPP

# include "SimpleSocket.hpp"

typedef SimpleSocket<struct sockaddr_in>	InternetSocket;

#endif
