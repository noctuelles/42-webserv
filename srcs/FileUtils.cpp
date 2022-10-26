/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 16:08:33 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 19:36:08 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <ios>
#include <vector>
#include <fstream>
#include <limits>
#include <iostream>

#include <sys/stat.h>
#include <unistd.h>

#include "FileUtils.hpp"

namespace ft
{
	namespace io
	{
		size_t	getFileSize(const char *filename)
		{
			struct stat	statbuf;

			if (stat(filename, &statbuf) < 0)
				throw (std::runtime_error("stat"));
			return (statbuf.st_size);
		}

		FileContent	loadFileContent(const char* filename, size_t maxsize = std::numeric_limits<size_t>::max())
		{
			using namespace	std;

			ifstream				ifs;
			size_t					fileSize;
			FileContent				v;

			ifs.exceptions(ifstream::badbit | ifstream::failbit);
			ifs.open(filename, ios_base::in | ios_base::binary);
			ifs.unsetf(ios::skipws);
			if ((fileSize = getFileSize(filename)) > maxsize)
					throw (std::logic_error("file too big."));
			v.resize(fileSize);
			ifs.read(reinterpret_cast<ifstream::char_type*>(&v.front()), fileSize);
			return (v);
		}
	}
}
