/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoundaryParser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/28 14:41:46 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/28 18:27:59 by plouvel          ###   ########.fr       */
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

			BoundaryParser(const std::string& boundary, bool crlf_prefix = false);

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
