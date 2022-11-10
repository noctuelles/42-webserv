#include <algorithm>
#include <iostream>
#include <cctype>
#include <functional>
#include <set>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include <memory>


using std::toupper;

struct	ci_char_traits : public std::char_traits<char>
{
	static bool	eq(char c1, char c2)
	{
		return (toupper(c1) == toupper(c2));
	}

	static bool	lt(char c1, char c2)
	{
		return (toupper(c1) < toupper(c2));
	}

	static int	compare(const char* s1, const char* s2, std::size_t n)
	{
		while (n-- != 0)
		{
			if (toupper(*s1) < toupper(*s2))
				return (-1);
			if (toupper(*s1) > toupper(*s2))
				return (1);
			++s1; ++s2;
		}
		return (0);
	}
    static const char* find(const char* s, std::size_t n, char a) {
        char const ua (toupper(a));
        while ( n-- != 0 ) 
        {
            if (toupper(*s) == ua)
                return s;
            s++;
        }
        return NULL;
    }
};

typedef	std::basic_string<char, ci_char_traits> ci_string;

struct LowerCase : std::unary_function<const char, char>
{
	inline char operator()(const char c)
	{
		return (std::tolower(c));
	}
};

char	lower(const char c)
{
	return (std::tolower(c));
}

class MIME
{
	public:

		operator	std::string()
		{
			return (str);
		}

		static MIME	TextHtml() {return MIME("text/html");}
		static MIME	ImageJpeg() {return MIME("image/jpeg");}
		static MIME	ImagePng() {return MIME("image/png");}
		static MIME	ApplicationOctetStream() {return MIME("application/octet-stream");}

		const std::string&	toStr() const
		{
			return (str);
		}

	private:

		MIME();
		MIME(const std::string& s)
			: str(s) {}

		std::string str;
};

typedef std::pair<const ci_string, const MIME>	MimePair;
typedef std::vector<MimePair>					MimePairVector;
typedef std::map<ci_string, const MIME>			MimeMap;

MimePair	table[] = 
{
	std::make_pair(".html", MIME::TextHtml()),
	std::make_pair(".jpg", MIME::ImageJpeg()),
	std::make_pair(".png", MIME::ImagePng())
};

const MimePairVector v(table, table + sizeof(table) / sizeof(table[0]));
const MimeMap		m(table, table + sizeof(table) / sizeof(table[0]));

class	CompareExtension : public std::unary_function<const MimePairVector::value_type&, bool>
{
	public:

		CompareExtension(const ci_string& str)
			: m_str(str) {}

		inline bool	operator()(const MimePairVector::value_type& val)
		{
			return (m_str == val.first);
		}

	private:

		ci_string	m_str;
};

int main()
{
	using namespace std;

	string s("/page.jPG");

	// pas de foret de if !!
	
	std::cout << m.at(".hTML").toStr() << '\n';

	string::size_type	r = s.rfind('.');
	if (r == string::npos)
	{
		cout << "this is binary data\n";
	}
	else
	{
		std::string	sub(s, r);

		MimePairVector::const_iterator it = std::find_if(v.begin(), v.end(), CompareExtension(ci_string(sub.begin(), sub.end())));
		if (it != v.end())
		{
			std::cout << "It's " << it->first.c_str() << " " << it->second.toStr() << '\n';
		}
		else
			std::cout << "It's binary data";
	}
}
