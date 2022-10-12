/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketTypes.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 19:36:33 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/12 19:46:06 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETTYPES_HPP
# define SOCKETTYPES_HPP

template <class T>
	class SimpleSocket;

typedef SimpleSocket<struct sockaddr_in>	InternetSocket;

#endif
