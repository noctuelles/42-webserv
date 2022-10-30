/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/27 14:20:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 21:03:33 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "HTTP.hpp"
# include <map>
# include <stdint.h>
#include <string>

namespace ft
{
	namespace http
	{
		class ResponseHeader
		{
			public:

				ResponseHeader(uint8_t major_ver, uint8_t minor_ver, const std::string& phrase);
				ResponseHeader(const ResponseHeader& other);
				~ResponseHeader();

				ResponseHeader&	operator=(const ResponseHeader& rhs);

				void			addField(const char* name, const std::string& value);
				void			removeField(const char *name);
				void			modifyField(const char *name, const std::string& value);

				std::string&		searchField(const char* name);
				const std::string&	searchField(const char* name) const;

				const std::string&	toString() const;
				size_t				lenght() const;


			private:

				static const size_t	DefaultCacheSize = 2048;

				std::string							m_status_line;
				HeaderFieldMap						m_header_field;
				mutable bool						m_cache_change;
				mutable std::string					m_cache;

				void	_buildCache() const;
		};
	}
}
