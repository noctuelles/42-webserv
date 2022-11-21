/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/19 17:33:04 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/21 18:05:16 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

#include "Http.hpp"
#include "VirtServ.hpp"
#include "VirtServInfo.hpp"
#include "Utils.hpp"
#include <list>
#include <ctime>

#include <dirent.h>

using std::string;
using std::vector;

namespace HTTP
{
	namespace AutoIndex
	{
		struct File
		{
			static bool compByDate(const File& lhs, const File& rhs)
			{
				if (lhs.is_a_dir && !rhs.is_a_dir)
					return (true);
				if (!lhs.is_a_dir && rhs.is_a_dir)
					return (false);
				if (lhs.last_modified.first < rhs.last_modified.first)
					return (true);
				return (false);
			}

			static bool compBySize(const File& lhs, const File& rhs)
			{
				if (lhs.is_a_dir && !rhs.is_a_dir)
					return (true);
				if (!lhs.is_a_dir && rhs.is_a_dir)
					return (false);
				else if (lhs.size.first < rhs.size.first)
					return (true);
				return (false);
			}

			static bool	compByName(const File& lhs, const File& rhs)
			{
				if (lhs.is_a_dir && !rhs.is_a_dir)
					return (true);
				if (!lhs.is_a_dir && rhs.is_a_dir)
					return (false);
				if (lhs.name_info.second < rhs.name_info.second)
					return (true);
				return (false);
			}

			File(const string& path, const string& name, time_t last_modif, bool is_a_dir, off_t size = 0);
			File();

			// size: size of the file in bytes.
			// returns a string containing the file unit and size.
			std::string	getFileUnits(off_t size);

			std::pair<string, string>	name_info;
			std::pair<time_t, string>	last_modified;
			std::pair<off_t, string>	size;
			bool						is_a_dir;

		};

		typedef bool (*cmpFnct)(const File&, const File&);

		struct Dir
		{
			operator DIR*() const;
			Dir(const char* path);
			~Dir() ;

			DIR*	ptr;

			private:

				Dir(const Dir& other);
				Dir operator=(const Dir& rhs);
		};

		const string	generatePage(const vector<VirtServ::RouteOptions>& routes, const VirtServ::RouteOptions& curr_route,
				const string& req_uri, const string& path, const string& query);
	}
}

#endif
