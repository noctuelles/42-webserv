/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 16:38:10 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 18:19:33 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <limits>
#include <sys/types.h>

namespace ft
{
	namespace io
	{
		typedef std::vector<unsigned char>	FileContent;

		size_t	getFileSize(const char *filename);

		FileContent	loadFileContent(const char* filename, size_t maxsize);
	}
}
