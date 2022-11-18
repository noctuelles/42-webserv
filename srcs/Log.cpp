#include "Log.hpp"
#include <ios>
#include <sstream>
#include <cstdarg>
#include <iomanip>
#include <ctime>

#include <cstdio>

using std::string;

const string Log::AppName = "WebServ";

const Log::LevelTable	Log::m_table[NBR_LEVEL] =
{
	{.color = Color::Modifier(1, Color::FG_GREEN), .str = "INFO"},
	{.color = Color::Modifier(1, Color::FG_YELLOW), .str = "WARNING"},
	{.color = Color::Modifier(2, Color::FG_BRED, Color::BOLD), .str = "FATAL"}
};

std::ostream&	operator<<(std::ostream& out, const Log::LevelTable& lvlt)
{
	out << lvlt.color << lvlt.str << lvlt.color.rst();
	return (out);
}

Log::Log()
	: os()
{}

Log::~Log()
{
	std::fprintf(stderr, "%s", os.str().c_str());
	std::fflush(stderr);
}

std::string	Log::nowTime(const char* formatting)
{
	time_t t = std::time(NULL);
	struct tm* tm = std::localtime(&t);
	char		buffer[2048];

	std::strftime(buffer, 2048, formatting, tm);

	return (std::string(buffer));
}

std::ostringstream&	Log::get(LogLevel lvl)
{
	Color::Modifier	title(1, Color::FG_CYAN);
	Color::Modifier	bold(1, Color::BOLD);

	os << '[' << bold << nowTime("%d/%m/%Y %H:%M:%S") << bold.rst() << ']';
	os << '[' << title << AppName << title.rst() << ']';
	os << '[' << m_table[lvl] << ']';
	os << "\t: ";

	return (os);
}

namespace Color
{
	Modifier::Modifier(size_t n, ...) :
		m_colors(n)
	{
		std::va_list	args;

		va_start(args, n);
		for (size_t i = 0; i < n; i++)
			m_colors[i] = va_arg(args, int);
		va_end(args);
	}

	std::ostream& operator<<(std::ostream& out, const Modifier& modf)
	{
		out << "\033[";
		for (std::vector<int>::const_iterator it = modf.m_colors.begin(); it != modf.m_colors.end(); it++)
		{
			out << *it;
			if (it != modf.m_colors.end() - 1)
				out << ';';
		}
		out << "m";
		return (out);
	}

}
