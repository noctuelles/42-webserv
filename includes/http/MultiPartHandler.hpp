/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiPartHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 15:49:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/27 16:08:09 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPARTPARSER_HPP
# define MULTIPARTPARSER_HPP

# include <fstream>
# include "Parser.hpp"
# include "HeaderFieldParser.hpp"

namespace HTTP
{
	class MultiPartHandler : public Parser<MultiPartInfo>
	{
		public:

			enum State
			{
				ST_CHECK_BOUND_DASH1,
				ST_CHECK_BOUND_DASH2,
				ST_CHECK_BOUND,
				ST_PARSE_HEADER_FIELD,
				ST_FILE_ENTRY,
				ST_FILE_CONTENT,
				ST_CRLF,
				ST_DONE
			};

			MultiPartHandler(const std::string& ressource_path, size_t content_lenght, const std::string& boundary);

			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it);

		private:

			void	transitionState(int new_state, int next_state);
			void	changeState(int new_state);
			void	initFileWriting();
			void	writeDataBatch(Buffer::const_iterator end);

			HeaderFieldParser			m_hfield_parser;
			size_t						m_content_lenght;
			const std::string&			m_ressource_path;
			std::string					m_boundary;
			std::string::const_iterator	cmp_it;
			Buffer::const_iterator		m_start_it;

			std::ofstream				m_ofile_handle;
	};
}

#endif
