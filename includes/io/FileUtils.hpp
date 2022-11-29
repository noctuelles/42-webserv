/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 16:38:10 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/27 13:04:34 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEUTILS_HPP
# define FILEUTILS_HPP

# include <sstream>
# include <vector>
# include <limits>
# include <string>
# include <sys/types.h>

namespace IO
{
	size_t		getFileSize(const char *filename);
	std::string	getFileLastModifiedDate(const char* filename);
	bool		isADirectory(const char* filename);
	mode_t		getFileMode(const char* path);

	std::string	loadFileContent(const char* filename, size_t maxSize = std::numeric_limits<size_t>::max());
}

#endif
