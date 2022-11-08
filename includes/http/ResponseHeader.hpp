/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/27 14:20:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/08 14:13:47 by plouvel          ###   ########.fr       */
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

				void			addField(const Field& field, const std::string& value = "");
				void			removeField(const Field& field);
				void			modifyField(const Field& field, const std::string& value);

				std::string&		searchField(const Field& field);
				const std::string&	searchField(const Field& field) const;

				const char*			toCString() const;
				const std::string&	toString() const;

				size_t				size() const;

			private:


				static const size_t	DefaultCacheSize = 2048;
				HeaderFieldMap				m_header_field;

				std::string					m_http_version;
				std::string					m_reason_phrase;

				mutable std::string					m_cache;
				mutable bool						m_build_cache;

				void		_buildCache() const;
		};
	}
}
