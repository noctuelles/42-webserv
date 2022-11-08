/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/08 11:10:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/08 11:25:15 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>

namespace ft
{
	namespace utils
	{
		extern size_t	DateBufferSize;

		// Return an RFC 822-compliant date format.
		std::string	getRFC822FormattedDate();
	}
}

#endif
