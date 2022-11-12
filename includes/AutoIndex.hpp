/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 15:03:09 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/12 16:59:31 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <string>
# include <map>

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

			AutoIndexPage(const FileMap& dirMap, const FileMap& regFileMap);

			string	operator()(const string& path) const;

		private:

			const FileMap&	m_dir_map, m_regfile_map;
			string	m_header;
			string	m_footer;
	};

	struct AutoIndexElem
	{
	};


	string	generateFileIndex(const char* path);
}


#endif
