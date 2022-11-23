#include <ostream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::ostream;

class StringArray 
{
public:
	friend ostream& operator<<(ostream& os, const StringArray& sa);

	~StringArray();

	void addString(const string& str);
	void addEnvVar(const string& field, const string& value);
	char** getData();

private:

	vector<char*> m_sarray;
};
