/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/01 16:59:33 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/01 16:59:40 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ostream>
#include <sstream>
#include <vector>

#ifndef LOG_HPP
# define LOG_HPP

namespace Color
{
	enum Code
	{
		RST				= 0,
		BOLD			= 1,
		UNDERLINE		= 4,
        FG_RED			= 31,
        FG_GREEN		= 32,
		FG_YELLOW		= 33,
        FG_BLUE			= 34,
		FG_CYAN			= 36,
        FG_DEFAULT		= 39,
		FG_BRED			= 91,
        BG_RED			= 41,
        BG_GREEN		= 42,
        BG_BLUE			= 44,
        BG_DEFAULT		= 49
	};

	class Modifier
	{
		public:
			Modifier(size_t n, ...);

			static Modifier rst()
			{
				return (Modifier(1, RST));
			}

			friend std::ostream& operator<<(std::ostream& out, const Modifier& modf);

		private:
			std::vector<int>	m_colors;
	};
}

enum LogLevel
{
	INFO,
	WARNING,
	FATAL,
	NBR_LEVEL
};

class Log
{
	public:

		struct LevelTable
		{
			Color::Modifier	color;
			std::string		str;
		};

		static const std::string	AppName;

		static std::string	nowTime(const char* formatting);

		Log();
		virtual ~Log();

		std::ostringstream&	get(LogLevel lvl);

	protected:

		static const LevelTable	m_table[NBR_LEVEL];

		std::ostringstream	os;

	private:


};

#endif
