/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/27 14:20:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 13:31:19 by plouvel          ###   ########.fr       */
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

				ResponseHeader(const std::string& phrase = "");
				ResponseHeader(const ResponseHeader& other);
				~ResponseHeader();

				ResponseHeader&	operator=(const ResponseHeader& rhs);

				void			changeReasonPhrase(const std::string& phrase);

				void			addField(const char* name, const std::string& value = "");
				void			removeField(const char *name);
				void			modifyField(const char *name, const std::string& value);

				std::string&		searchField(const char* name);
				const std::string&	searchField(const char* name) const;

				const std::string&	toString();
				size_t				size() const;


			private:

				static const size_t	DefaultCacheSize = 2048;

				std::string							m_http_version;
				std::string							m_reason_phrase;

				HeaderFieldMap				m_header_field;
				std::string					m_cache;

				void	_buildCache();
				void	_updateDate();
		};
	}
}
