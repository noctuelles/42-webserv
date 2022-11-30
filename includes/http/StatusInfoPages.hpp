/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusInfoPages.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 13:55:35 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/30 11:05:12 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSINFOPAGES_HPP
# define STATUSINFOPAGES_HPP

# include "Http.hpp"

# include <vector>

using std::vector;

namespace HTTP
{
	class StatusInfoPages
	{
		public:

			static const vector<StatusInfo>&	get()
			{
				static vector<StatusInfo>	vec(MaxStatusCode);
				static bool					init = false;

				if (!init)
				{
					vec[OK]						= HTTP_STATUS("200 OK");
					vec[Created]				= HTTP_STATUS("201 Created");
					vec[SeeOther]				= HTTP_STATUS("303 See Other");
					vec[BadRequest]				= HTTP_ERRPAGE("400 Bad Request");
					vec[Forbidden]				= HTTP_ERRPAGE("403 Forbidden");
					vec[NotFound]				= HTTP_ERRPAGE("404 Not Found");
					vec[MethodNotAllowed]		= HTTP_ERRPAGE("405 Method Not Allowed");
					vec[RequestTimeout]			= HTTP_ERRPAGE("408 Request Timeout");
					vec[ContentTooLarge]		= HTTP_ERRPAGE("413 Content Too Large");
					vec[UriTooLong]				= HTTP_ERRPAGE("414 Uri Too Long");
					vec[UnsupportedMediaType]	= HTTP_ERRPAGE("415 Unsupported Media Type");
					vec[InternalServerError]	= HTTP_ERRPAGE("500 Internal Server Error");
					vec[NotImplemented]			= HTTP_ERRPAGE("501 Not Implemented");
					vec[VersionNotSupported]	= HTTP_ERRPAGE("505 HTTP Version Not Supported");
					init = true;
				}

				return (vec);
			}

		private:

			StatusInfoPages();
			StatusInfoPages(const StatusInfoPages& other);
			StatusInfoPages	operator=(const StatusInfoPages& rhs);
	};
}

#endif
