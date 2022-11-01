/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:43:06 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/01 15:32:31 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTP.hpp"
#include <utility>



namespace ft
{
	namespace http
	{
		const char*	CRLF = "\r\n";

		const char*	MethodTable[] =
		{
			"GET",
			"POST",
			"DELETE"
		};
	}
}
