/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 15:09:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/12 17:11:15 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <ios>
#include <vector>
#include <iostream>

#include "AutoIndex.hpp"
#include "Utils.hpp"
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

using std::vector;
using std::make_pair;

#define AUTOINDEX_START (\
"<html>\n"\
"	<head><title>Index of #PATH#</title></head>\n"\
"	<body>\n"\
"		<h1>Index of #PATH#</h1>\n"\
"		<table>\n"\
"			<tr>\n"\
"				<th>Name</th>\n"\
"				<th>Last modified</th>\n"\
"				<th>Size</th>\n"\
"			</tr>\n"\
"			<tr><th colspan=\"5\"><hr></th></tr>\n")

// "<tr><td><a href=\"#FN#\">#FN#</a></td><td align=\"right\">#DATE#</td><td align=\"right\">#SIZE#</td></tr>\n"

#define AUTOINDEX_FIELD_FILE (\
"			<tr>\n"\
"				<td><a href=\"#FN#\">#FN#</a></td>\n"\
"				<td align=\"right\">#DATE#</td>\n"\
"				<td align=\"right\">#SIZE#</td>\n"\
"			</tr>\n")

#define AUTOINDEX_FIELD_DIR (\
"			<tr>\n"\
"				<td><a href=\"#FN#\\\">#FN#</a></td>\n"\
"				<td align=\"right\">#DATE#</td>\n"\
"				<td align=\"right\">#SIZE#</td>\n"\
"			</tr>\n")

#define AUTOINDEX_END (\
"			<tr><th colspan=\"5\"><hr></th></tr>\n"\
"		</table>\n"\
"		<address>webserv/0.1</address>\n"\
"	</body>\n"\
"</html>\n")

namespace ft
{
	AutoIndexPage::AutoIndexPage(const FileMap& dirMap, const FileMap& regFileMap)
		: m_dir_map(dirMap), m_regfile_map(regFileMap), m_header(), m_footer()
	{
	}

	string	AutoIndexPage::operator()(const string& path) const
	{
		string	page = AUTOINDEX_START;

		string::size_type ret;
		while ((ret = page.find("#PATH#")) != string::npos)
			page.replace(ret, 6, path);

		for (FileMap::const_iterator it = m_dir_map.begin(); it != m_dir_map.end(); it++)
		{
			page.append(AUTOINDEX_FIELD_DIR);
			string::size_type ret;
			while ((ret = page.find("#FN#")) != string::npos)
				page.replace(ret, 4, it->first);
			page.replace(page.find("#DATE#"), 6, it->second.last_modified);
			page.replace(page.find("#SIZE#"), 6, it->second.size);
		}

		for (FileMap::const_iterator it = m_regfile_map.begin(); it != m_regfile_map.end(); it++)
		{
			page.append(AUTOINDEX_FIELD_FILE);
			string::size_type ret;
			while ((ret = page.find("#FN#")) != string::npos)
				page.replace(ret, 4, it->first);
			page.replace(page.find("#DATE#"), 6, it->second.last_modified);
			page.replace(page.find("#SIZE#"), 6, it->second.size);
		}

		page.append(AUTOINDEX_END);
		return (page);
	}

	string	generateFileIndex(const char* path)
	{
		DIR*				d = opendir(path);
		struct dirent*		dir = NULL;
		FileMap				directoryMap, regFileMap;

		if (!d)
			throw (std::ios_base::failure("::opendir"));
		while ((dir = readdir(d)) != NULL)
		{
			struct stat	statBuf;

			if (stat(dir->d_name, &statBuf) < 0)
				throw (std::ios_base::failure("::stat"));
			if (!(statBuf.st_mode & S_IRUSR))
				continue;
			switch (statBuf.st_mode & S_IFMT)
			{
				case S_IFDIR:
					directoryMap.insert(make_pair(dir->d_name, FileInfo(
									"-",
									utils::formatTimeToRFC822(std::gmtime(&statBuf.st_mtim.tv_sec))
									)));
					break;
				case S_IFREG:
					regFileMap.insert(make_pair(dir->d_name, FileInfo(
									utils::integralToString(statBuf.st_size),
									utils::formatTimeToRFC822(std::gmtime(&statBuf.st_mtim.tv_sec))
									)));
					break;
				default:
					;
			}
		}

		return (AutoIndexPage(directoryMap, regFileMap)(path));
	}
}
