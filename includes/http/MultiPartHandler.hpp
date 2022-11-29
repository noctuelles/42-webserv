/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultiPartHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 15:49:47 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/29 13:45:12 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPARTPARSER_HPP
# define MULTIPARTPARSER_HPP

# include <fstream>
# include "Parser.hpp"
# include "HeaderFieldParser.hpp"
# include "BoundaryParser.hpp"

namespace HTTP
{
	class MultiPartHandler : public Parser<MultiPartInfo>
	{
		public:

			enum State
			{
				ST_START_BOUND,
				ST_PARSE_HEADER_FIELD,
				ST_FILE_CONTENT,
				ST_FILE_BOUND,
				ST_FILE_BOUND_END,
				ST_FILE_BOUND_END_DASH1,
				ST_FILE_BOUND_END_DASH2,
				ST_CRLF,
				ST_DONE
			};

			MultiPartHandler(const std::string& ressource_path, size_t content_lenght, const std::string& boundary);
			~MultiPartHandler();

			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it);

		private:

			void	transitionState(int new_state, int next_state);
			void	changeState(int new_state);
			void	initFileWriting();
			void	writeDataBatch(Buffer::const_iterator begin, Buffer::const_iterator end);

			Buffer::const_iterator	checkEnd(const Buffer& buff, Buffer::const_iterator it);

			HeaderFieldParser			m_hfield_parser;
			std::string					m_boundary;
			BoundaryParser				m_boundary_parser;
			size_t						m_content_lenght;
			const std::string&			m_ressource_path;

			Buffer::const_iterator		m_data_it;

			std::ofstream				m_ofile_handle;
	};
}

#endif
