/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusInfoPages.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 13:55:35 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 14:12:28 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSINFOPAGES_HPP
# define STATUSINFOPAGES_HPP

# include "Http.hpp"

# include <vector>

using std::vector;

namespace ft
{
	namespace http
	{
		class StatusInfoPages
		{
			public:

				static const vector<StatusInfo>&	get()
				{
					if (!m_singleton)
					{
						m_singleton = new vector<StatusInfo>(MaxStatusCode);

						(*m_singleton)[OK]					= HTTP_STATUS("200 OK");
						(*m_singleton)[BadRequest]			= HTTP_ERRPAGE("400 Bad Request");
						(*m_singleton)[Forbidden]			= HTTP_ERRPAGE("403 Forbidden");
						(*m_singleton)[NotFound]			= HTTP_ERRPAGE("404 Not Found");
						(*m_singleton)[RequestTimeout]		= HTTP_ERRPAGE("408 Request Timeout");
						(*m_singleton)[UriTooLong]			= HTTP_ERRPAGE("414 Uri Too Long");
						(*m_singleton)[NotImplemented]		= HTTP_ERRPAGE("501 Not Implemented");
						(*m_singleton)[VersionNotSupported]	= HTTP_ERRPAGE("505 HTTP Version Not Supported");
					}
					return (*m_singleton);
				}

			private:

				StatusInfoPages();
				StatusInfoPages(const StatusInfoPages& other);
				StatusInfoPages	operator=(const StatusInfoPages& rhs);

				static vector<StatusInfo>*	m_singleton;
		};
	}
}

#endif
