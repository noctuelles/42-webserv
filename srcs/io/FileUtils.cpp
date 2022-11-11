/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 16:08:33 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/11 16:52:32 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <ios>
#include <fstream>
#include <iostream>

#include <sys/stat.h>
#include <unistd.h>

#include "FileUtils.hpp"
#include "Utils.hpp"

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

		bool	isADirectory(const char* filename)
		{
			struct stat	statbuf;

			if (stat(filename, &statbuf) < 0)
				throw (std::runtime_error("stat"));
			return (S_ISDIR(statbuf.st_mode));
		}

		std::string	getFileLastModifiedDate(const char* filename)
		{
			struct stat	sbuf;

			if (stat(filename, &sbuf) < 0)
				throw (std::runtime_error("stat"));
			return (utils::formatTimeToRFC822(std::gmtime(&sbuf.st_mtim.tv_sec)));
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
