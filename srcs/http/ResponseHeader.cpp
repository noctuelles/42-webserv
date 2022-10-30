/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 18:02:49 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/30 21:20:49 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHeader.hpp"
#include <sstream>
#include <utility>
#include <assert.h>

namespace ft
{
	namespace http
	{
		ResponseHeader::ResponseHeader(uint8_t major_ver, uint8_t minor_ver, const std::string& phrase)
			: m_status_line("HTTP/M.m"), m_header_field(), m_cache_change(true), m_cache()
		{
			m_cache.reserve(DefaultCacheSize);
			m_status_line[5] = major_ver + '0';
			m_status_line[7] = minor_ver + '0';
			m_status_line
				.append(" ")
				.append(phrase)
				.append(CRLF);
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
			m_status_line = rhs.m_status_line;
			m_header_field = rhs.m_header_field;
			m_cache = rhs.m_cache;
			m_cache_change = rhs.m_cache_change;
			if (m_cache_change)
				_buildCache();
			return (*this);
		}

		void	ResponseHeader::addField(const char* name, const std::string& str)
		{
			if (m_header_field.insert(std::make_pair(name, str)).second)
				m_cache_change = true;
		}

		void	ResponseHeader::modifyField(const char *name, const std::string& str)
		{
			std::string& strRef = searchField(name);
			strRef = str;
		}

		void	ResponseHeader::removeField(const char* name)
		{
			if (m_header_field.erase(name))
				m_cache_change = true;
		}

		std::string&	ResponseHeader::searchField(const char* name)
		{
			return (m_header_field.at(name));
		}

		const std::string&	ResponseHeader::searchField(const char* name) const
		{
			return (m_header_field.at(name));
		}

		const std::string&	ResponseHeader::toString() const
		{
			if (m_cache_change)
				_buildCache();
			return (m_cache);
		}

		size_t	ResponseHeader::lenght() const
		{
			return (toString().length());
		}

		void	ResponseHeader::_buildCache() const
		{
			m_cache.clear();
			m_cache.append(m_status_line);
			for (HeaderFieldMap::const_iterator it = m_header_field.begin(); it != m_header_field.end(); it++)
			{
				m_cache
					.append(it->first)
					.append(": ")
					.append(it->second)
					.append(CRLF);
			}
			m_cache.append(CRLF);
			m_cache_change = false;
		}
	}
}
