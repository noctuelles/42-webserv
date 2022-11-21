/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/27 14:20:16 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/21 17:43:47 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP

# include "Http.hpp"
# include <map>
# include <string>

namespace HTTP
{
	class ResponseHeader
	{
		public:

			ResponseHeader();
			ResponseHeader(const ResponseHeader& other);
			~ResponseHeader();

			ResponseHeader&	operator=(const ResponseHeader& rhs);

			void			setReasonPhrase(const std::string& phrase);

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

#endif
