/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 16:08:33 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/27 13:06:41 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <ios>
#include <fstream>
#include <iostream>

#include <sys/stat.h>
#include <unistd.h>

#include "FileUtils.hpp"
#include "Http.hpp"
#include "Utils.hpp"
#include "RequestHandler.hpp"

namespace IO
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

	mode_t	getFileMode(const char* path)
	{
		struct stat	stat_buf;

		if (stat(path, &stat_buf) < 0)
		{
			if (errno == ENOENT)
				throw (HTTP::RequestHandler::Exception(HTTP::NotFound));
			else
				throw (HTTP::RequestHandler::Exception(HTTP::BadRequest));
		}
		return (stat_buf.st_mode);
	}

	struct stat	statWrapper(const char* filename)
	{
		struct stat	stat_buf;

		if (stat(filename, &stat_buf) < 0)
		{
			if (errno != EACCES)
				throw (HTTP::RequestHandler::Exception(HTTP::InternalServerError));
		}
		return (stat_buf);
	}

	std::string	getFileLastModifiedDate(const char* filename)
	{
		struct stat	sbuf;

		if (stat(filename, &sbuf) < 0)
			throw (std::runtime_error("stat"));
		return (Utils::formatTimeToRFC822(std::gmtime(&sbuf.st_mtim.tv_sec)));
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
