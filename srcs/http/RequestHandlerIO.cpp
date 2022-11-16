/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlerIO.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/16 14:55:32 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/16 15:21:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include <sys/stat.h>

namespace ft
{
	namespace http
	{
		bool	RequestHandler::io::isAReadableRegFile(const char* path)
		{
			struct stat	stat_buf;

			if (::stat(path, &stat_buf) < 0)
			{
				if (errno != ENOENT)
					throw (Exception(InternalServerError));
			}
			else if (!S_ISDIR(stat_buf.st_mode) && (stat_buf.st_mode & S_IRUSR))
				return (true);
			return (false);
		}
	}
}
