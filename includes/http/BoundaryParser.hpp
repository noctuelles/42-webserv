/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoundaryParser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 14:41:46 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/28 15:08:29 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOUNDARYPARSER_HPP
# define BOUNDARYPARSER_HPP

# include "Parser.hpp"

namespace HTTP
{
	class BoundaryParser : public Parser<Buffer>
	{
		public:

			enum State
			{
				ST_DASH1,
				ST_DASH2,
				ST_BOUNDARY,
				ST_ENDING,
				ST_ENDING_DASH,
				ST_CRLF,
				ST_DONE_CRLF,
				ST_DONE_DASH
			};

			BoundaryParser(const std::string& boundary);

			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it);

		private:

			void	transitionState(int new_state, int next_state);
			void	changeState(int new_state);

			bool						m_eat;
			std::string					m_boundary;
			std::string::const_iterator	m_cmp_it;
	};
}

#endif
