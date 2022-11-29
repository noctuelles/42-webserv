/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoundaryParser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 14:41:46 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/29 13:45:56 by plouvel          ###   ########.fr       */
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
				ST_CRLF,
				ST_DONE
			};

			BoundaryParser(const std::string& boundary);

			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it);

			void	reset(bool crlf_prefix);

		private:

			void	transitionState(int new_state, int next_state);
			void	changeState(int new_state);

			std::string			m_boundary;
			std::string::const_iterator	m_cmp_it;
	};
}

#endif
