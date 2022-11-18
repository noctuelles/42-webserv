/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 15:03:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/18 09:40:35 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <string>
# include <map>
# include "dirent.h"

# include "VirtServInfo.hpp"
# include "VirtServ.hpp"

using std::string;
using std::map;

namespace ft
{
	struct FileInfo;

	typedef map<string, FileInfo>	FileMap;

	struct FileInfo
	{
		FileInfo(const string& size, const string& last_modified)
			: size(size), last_modified(last_modified) {}
		string	size;
		string	last_modified;
	};

	class AutoIndexPage
	{
		public:

			AutoIndexPage(const VirtServ::RouteOptions& route, const string& path);
			~AutoIndexPage();

			string	operator()(const string& path) const;

		private:

			static int	_filterWrapper(const struct dirent* dir);

			int	_filter(const struct dirent* dir);

			const VirtServ::RouteOptions&	m_route;
			FileMap							m_dir_map, m_regfile_map;
			DIR*							m_dirp;
	};

	struct AutoIndexElem
	{
	};


	string	generateFileIndex(const char* path);
}


#endif
