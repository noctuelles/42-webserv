/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 11:11:22 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/08 11:24:45 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include "HTTP.hpp"
#include <ctime>

namespace ft
{
	namespace utils
	{
		size_t	DateBufferSize = 1024;

		std::string	getRFC822FormattedDate()
		{
			char		buffer[DateBufferSize];
			size_t		length;
			time_t		time;

			time = std::time(NULL); // this call can't fail.
			length = std::strftime(buffer, DateBufferSize, http::RFC822_DateFormat, std::gmtime(&time));
			return (std::string(buffer, length));
		}
	}
}
