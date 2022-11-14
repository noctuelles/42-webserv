/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 11:11:22 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/09 12:59:27 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include "Http.hpp"
#include <ctime>
#include <stdexcept>

namespace ft
{
	namespace utils
	{
		size_t	DateBufferSize = 1024;

		std::string	getRFC822NowDate()
		{
			char		buffer[DateBufferSize];
			size_t		length;
			time_t		time;

			time = std::time(NULL); // this call can't fail.
			length = std::strftime(buffer, DateBufferSize, http::RFC822_DateFormat, std::gmtime(&time));
			return (std::string(buffer, length));
		}

		std::string	formatTimeToRFC822(struct tm* ptrTm)
		{
			char		buffer[DateBufferSize];
			size_t		length;

			length = std::strftime(buffer, DateBufferSize, http::RFC822_DateFormat, ptrTm);
			if (length == 0)
				throw (std::logic_error("std::strftime"));
			return (std::string(buffer, length));
		}
	}
}
