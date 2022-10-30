/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 16:08:33 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 19:31:02 by plouvel          ###   ########.fr       */
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
#include <iterator>

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


		std::string	loadFileContent(const char* filename, size_t maxSize)
		{
			using namespace	std;

			ifstream					ifs;
			size_t						fileSize;
			istreambuf_iterator<char>	it(ifs);
			string						s;

			ifs.exceptions(ifstream::badbit | ifstream::failbit);
			ifs.open(filename, ios_base::in);
			fileSize = getFileSize(filename);
			if (fileSize >= maxSize)
				throw (logic_error("file too big"));
			s.reserve(fileSize);
			s.assign(it, istreambuf_iterator<char>());
			return (s);
		}
	}
}
