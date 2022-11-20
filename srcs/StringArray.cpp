#include "Env.hpp"
#include <cstring>
#include <ostream>
#include <sstream>
#include <vector>

void StringArray::addString(const string& str)
{
	m_sarray.push_back(new char[str.length() + 1]);
	std::strcpy(m_sarray.back(), str.c_str()); // Fine to use because c++ strings are guaranted to end with a '\0';
}

void StringArray::addEnvVar(const string& var_name, const string& value)
{
	m_sarray.push_back(new char[var_name.length() + 1 + value.length() + 1]);
	std::ostringstream oss;
	oss << var_name << '=' << value;
	std::strcpy(m_sarray.back(), oss.str().c_str()); // Fine to use because c++ strings are guaranted to end with a '\0';
}

char** StringArray::getData()
{
	return m_sarray.data();
}

StringArray::~StringArray()
{
	vector<char *>::const_iterator it = m_sarray.begin();
	vector<char *>::const_iterator end = m_sarray.end();
	for (; it != end ; ++it)
	{
		delete *it;
	}
}

ostream& operator<<(ostream& os, const StringArray& sa)
{
	vector<char *>::const_iterator it = sa.m_sarray.begin();
	vector<char *>::const_iterator end = sa.m_sarray.end();
	for (; it !=end ; ++it)
		os << *it << '\n';
	return os;
}

