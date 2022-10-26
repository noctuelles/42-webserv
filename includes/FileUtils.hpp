/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileUtils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/26 16:38:10 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/26 17:25:34 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <limits>
#include <sys/types.h>

namespace ft
{

	size_t	getFileSize(const char *filename);

	std::vector<unsigned char>	loadFileContent(const char* filename, size_t maxsize = std::numeric_limits<size_t>::max());
}
