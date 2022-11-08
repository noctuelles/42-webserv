/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHeader.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 18:02:49 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/08 13:40:33 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseHeader.hpp"
#include "HTTP.hpp"
#include "FileUtils.hpp"
#include <sstream>
#include <string>
#include <utility>
#include <ctime>
#include <algorithm>
#include <assert.h>

namespace ft
{
	namespace http
	{
		ResponseHeader::ResponseHeader(const std::string& phrase)
			: m_header_field(), m_http_version("HTTP/1.1"), m_reason_phrase(phrase), m_cache(), m_build_cache(true)
		{
			m_cache.reserve(DefaultCacheSize);
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

		void	ResponseHeader::addField(const Field& field, const std::string& str)
		{
			if (m_header_field.insert(std::make_pair(field.str(), str)).second)
				m_build_cache = true;
		}

		void	ResponseHeader::modifyField(const Field& field, const std::string& str)
		{
			std::string& strRef = searchField(field);
			strRef = str;
			m_build_cache = true;
		}

		void	ResponseHeader::removeField(const Field& field)
		{
			m_header_field.erase(field.str());
		}

		std::string&	ResponseHeader::searchField(const Field& field)
		{
			return (m_header_field.at(field.str()));
		}

		const std::string&	ResponseHeader::searchField(const Field& field) const
		{
			return (m_header_field.at(field.str()));
		}

		const std::string&	ResponseHeader::toString() const
		{
			if (m_build_cache)
				_buildCache();
			return (m_cache);
		}

		const char*	ResponseHeader::toCString() const
		{
			if (m_build_cache)
				_buildCache();
			return (m_cache.c_str());
		}

		size_t	ResponseHeader::size() const
		{
			return (m_cache.size());
		}

		void	ResponseHeader::_buildCache() const
		{
			m_cache.clear();
			m_cache = m_http_version + ' ' + m_reason_phrase;
			HeaderFieldMap::value_type sd = *m_header_field.begin();

			std::for_each(m_header_field.begin(), m_header_field.end(), std::bind2nd(std::mem_fun(&ResponseHeader::_appendHeaderField), ));
			m_cache.append(CRLF);
			m_build_cache = false;
		}

		void	ResponseHeader::_appendHeaderField(HeaderFieldMap::value_type& val)
		{
				m_cache
					.append(val.first)
					.append(": ")
					.append(val.second)
					.append(CRLF);
		}
	}
}
