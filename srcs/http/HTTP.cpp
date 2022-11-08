/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTP.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 18:43:06 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/08 11:16:08 by plouvel          ###   ########.fr       */
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

		const char*	RFC822_DateFormat = "%a, %d %b %Y %H:%M:%S %z";
	}
}
