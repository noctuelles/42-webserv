/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 17:45:38 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/02 20:55:59 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"
#include "Utils.hpp"
#include "VirtServ.hpp"
#include "RequestHandler.hpp"

#include <cstring>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <libgen.h>
#include <utility>

namespace HTTP
{
	namespace AutoIndex
	{
		FileInfo::FileInfo(const string& path, const string& name, time_t last_modif, bool is_a_dir, off_t size) :
			name_info(std::make_pair(path, name)),
			last_modified(std::make_pair(last_modif, Utils::formatTimeToRFC822(std::gmtime(&last_modif)))),
			size(std::make_pair(size, getFileInfoUnits(size))),
			is_a_dir(is_a_dir)
		{
			if (is_a_dir)
				this->size.second = "-";
		}

		FileInfo::FileInfo() :
			name_info(),
			last_modified(),
			size(),
			is_a_dir()
		{}

		std::string	FileInfo::getFileInfoUnits(off_t size)
		{
			static const char*	unit[5] = {"B", "KB", "MB", "GB", "TB"};
			size_t				i = 0;
			double				d = static_cast<double>(size);
			std::stringstream	ss;

			while (d >= 1000 && i < 5)
			{
				d /= 1000;
				i++;
			}
			ss << std::setprecision(3) << d << unit[i];
			return (ss.str());
		}

		Dir::Dir(const char* path) : ptr(::opendir(path))
		{
			if (!ptr)
			{
				if (errno == ENOTDIR || errno == ENOENT)
					throw (RequestHandler::Exception(BadRequest));
				else
					throw (RequestHandler::Exception(InternalServerError));
			}
		}

		Dir::~Dir()
		{
			::closedir(ptr);
		}

		Dir::operator DIR*() const
		{
			return (ptr);
		}

		static const string	generateHTML(const std::list<FileInfo>& files, const string& req_uri)
		{
			std::stringstream	ss;

			ss << "<html>\n"
				<< "	<head><title>Index of " << req_uri << "</title></head>\n"
				<< "	<body>\n"
				<< "		<h1>Index of " << req_uri << "</h1>\n"
				<< "		<table cellspacing=\"8px\" cellpadding=\"4px\">\n"
				<< "			<tr>\n"
				<< "				<th valign=\"top\"><img src=\"/img/down-arrow.png\" alt=\"[ICO]\">\n"
				<< "				<th><a href=\"?byName\">Name</a></th>\n"
				<< "				<th><a href=\"?byDate\">Last Modified</a></th>\n"
				<< "				<th><a href=\"?bySize\">Size</a></th>\n"
				<< "			</tr>\n"
				<< "			<tr><th colspan=\"5\"><hr></th></tr>\n";

			for (std::list<FileInfo>::const_iterator it = files.begin(); it != files.end(); it++)
			{
				ss << "			<tr>\n"
					<<"				<td valign=\"top\"><img src=\"/img/";
				if (it->is_a_dir)
					ss << "folder.png\" alt=\"[DIR]\"></td>\n";
				else
					ss << "file.png\" alt=\"[FILE]\"></td>\n";
				ss <<"				<td><a href=\"" << it->name_info.first << "\">" << it->name_info.second << "</a></td>\n"
					<<"				<td align=\"right\"><i>" << it->last_modified.second << "</i></td>\n"
					<<"				<td align=\"right\"><b>" << it->size.second << "</b></td>\n"
					<<"			</tr>\n";
			}

			ss << "				<tr><th colspan=\"5\"><hr></th></tr>\n"
				<<"		</table>\n"
				<<"		<address>webserv/0.1</address>\n"
				<<"	</body>\n"\
				<<"</html>\n";

			return (ss.str());
		}

		const string	generatePage(const VirtServ& virtserv, const VirtServ::RouteOptions& curr_route,
				const string& req_uri, const string& path, const string& query)
		{
			Dir					dir(path.c_str());
			struct dirent*		dir_stream = NULL;
			std::list<FileInfo>	files;
			FileInfo			parent_dir;
			cmpFnct				cmp_fnct = &FileInfo::compByName;

			if (query.compare("byDate") == 0)
				cmp_fnct = &FileInfo::compByDate;
			else if (query.compare("bySize") == 0)
				cmp_fnct = &FileInfo::compBySize;

			// Read the whole directory, sort the list.

			errno = 0;
			while ((dir_stream = ::readdir(dir)) != NULL)
			{
				struct stat	sbuf;

				if (fstatat(::dirfd(dir), dir_stream->d_name, &sbuf, 0) < 0)
					throw (HTTP::RequestHandler::Exception(HTTP::InternalServerError));
				if (!(sbuf.st_mode & S_IRUSR))
					continue ;
				if (dir_stream->d_name[0] == '.')
				{
					if (dir_stream->d_name[1] == '\0' || ::strcmp(&dir_stream->d_name[1], ".") != 0)
						continue ;
				}
				switch (sbuf.st_mode & S_IFMT)
				{
					case S_IFDIR:
					{
						if (::strcmp(dir_stream->d_name, "..") == 0)
						{
							vector<char>	creq_uri(req_uri.begin(), req_uri.end());
							creq_uri.push_back('\0');
							::dirname(creq_uri.data());
							creq_uri.resize(strlen(creq_uri.data()));
							std::string	path(creq_uri.begin(), creq_uri.end());

							if (path != "/")
								path.push_back('/');
							parent_dir = FileInfo(path, "Parent Directory", sbuf.st_ctim.tv_sec, true);
						}
						else
						{
							string	path = req_uri + dir_stream->d_name;

							path.push_back('/');
							files.push_back(FileInfo(path, dir_stream->d_name, sbuf.st_ctim.tv_sec, true));
						}
					}
					break;
					case S_IFREG:
						files.push_back(FileInfo(req_uri + dir_stream->d_name, dir_stream->d_name, sbuf.st_ctim.tv_sec, false, sbuf.st_size));
					break;
					default:
						;
				}
			}
			if (!dir_stream && errno != 0)
				throw (HTTP::RequestHandler::Exception(HTTP::InternalServerError));
			files.sort(cmp_fnct);

			// Determine if the parent directory should be displayed.

			if (req_uri != "/")
			{
				const VirtServ::RouteOptions&	parent_route = Utils::findRoute(parent_dir.name_info.first, virtserv);

				if (parent_dir.name_info.first.compare(0, curr_route.m_location_match.length(), curr_route.m_location_match) == 0)
					files.push_front(parent_dir);
				else if (parent_route.m_autoindex)
						files.push_front(parent_dir);
			}
			return (generateHTML(files, req_uri));
		}
	}
}
