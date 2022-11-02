/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 18:02:49 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/02 13:33:46 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHeader.hpp"
#include "FileUtils.hpp"
#include <sstream>
#include <utility>
#include <ctime>
#include <assert.h>

namespace ft
{
	namespace http
	{
		ResponseHeader::ResponseHeader(const std::string& phrase)
			: m_http_version("HTTP/1.1"), m_reason_phrase(phrase),
			  m_header_field(), m_cache()
		{
			m_cache.reserve(DefaultCacheSize);
			addField("Date");
		}

		ResponseHeader::ResponseHeader(const ResponseHeader& other)
		{
			*this = other;
		}

		ResponseHeader::~ResponseHeader()
		{}

		ResponseHeader&	ResponseHeader::operator=(const ResponseHeader& rhs)
		{
			if (this == &rhs)
				return (*this);
			m_http_version = rhs.m_http_version;
			m_reason_phrase = rhs.m_reason_phrase;
			m_header_field = rhs.m_header_field;
			m_cache = rhs.m_cache;
			return (*this);
		}

		void	ResponseHeader::addField(const char* name, const std::string& str)
		{
			m_header_field.insert(std::make_pair(name, str));
		}

		void	ResponseHeader::modifyField(const char *name, const std::string& str)
		{
			std::string& strRef = searchField(name);
			strRef = str;
		}

		void	ResponseHeader::removeField(const char* name)
		{
			m_header_field.erase(name);
		}

		std::string&	ResponseHeader::searchField(const char* name)
		{
			return (m_header_field.at(name));
		}

		const std::string&	ResponseHeader::searchField(const char* name) const
		{
			return (m_header_field.at(name));
		}

		const std::string&	ResponseHeader::toString()
		{
			_buildCache();
			return (m_cache);
		}

		size_t	ResponseHeader::size() const
		{
			return (m_cache.size());
		}

		void	ResponseHeader::_buildCache()
		{
			_updateDate();
			m_cache.clear();
			m_cache = m_http_version + ' ' + m_reason_phrase;
			for (HeaderFieldMap::const_iterator it = m_header_field.begin(); it != m_header_field.end(); it++)
			{
				m_cache
					.append(it->first)
					.append(": ")
					.append(it->second)
					.append(CRLF);
			}
			m_cache.append(CRLF);
		}

		void	ResponseHeader::_updateDate()
		{
			char		buffer[1024];
			time_t		time = std::time(NULL);
			struct tm*	gmt_time = std::gmtime(&time);

			// Format : https://www.rfc-editor.org/rfc/rfc822
			strftime(buffer, 1024, "%a, %d %b %Y %H:%M:%S %Z", gmt_time);
			modifyField("Date", buffer);
		}
	}
}
