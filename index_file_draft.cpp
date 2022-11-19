#include <bits/stdc++.h>
#include <ctime>
#include <dirent.h>
#include <iomanip>
#include <stdexcept>
#include <sys/stat.h>
#include <exception>

using namespace std;

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

string	formatTime(const struct tm* ptrTm)
{
	char		buffer[2048];
	size_t		length;

	length = std::strftime(buffer, 2048, "%a, %d %b %Y %H:%M:%S %Z", ptrTm);
	if (length == 0)
		throw (std::logic_error("std::strftime"));
	return (string(buffer, length));
}

static const char*	getSizeGr(double& size, unsigned int lvl)
{
	static const char*	weight[5] = {"B", "KB", "MB", "GB", "TB"};

	if (size > 1000)
	{
		size /= 1000;
		return (getSizeGr(size, lvl + 1));
	}
	return (weight[lvl]);
}

struct AutoIndexFile
{
	AutoIndexFile(const string& name, time_t last_modif, bool is_a_dir, off_t fsize = 0) :
		name(name), last_modified_raw(last_modif), last_modified(formatTime(std::gmtime(&last_modified_raw))), size_raw(fsize), size(), is_a_dir(is_a_dir)
	{
		stringstream	ss;
		double			d = fsize;

		size = getSizeGr(d, 0);
		ss << std::setprecision(3) << d;
		size.insert(0, ss.str());
	}

	string	name;

	time_t	last_modified_raw;
	string	last_modified;

	off_t	size_raw;
	string	size;

	bool	is_a_dir;

	static bool compByDate(const AutoIndexFile& lhs, const AutoIndexFile& rhs)
	{
		if (lhs.is_a_dir && !rhs.is_a_dir)
			return (true);
		if (!lhs.is_a_dir && rhs.is_a_dir)
			return (false);
		if (lhs.last_modified_raw < rhs.last_modified_raw)
			return (true);
		return (false);
	}

	static bool compBySize(const AutoIndexFile& lhs, const AutoIndexFile& rhs)
	{
		if (lhs.is_a_dir && !rhs.is_a_dir)
			return (true);
		if (!lhs.is_a_dir && rhs.is_a_dir)
			return (false);
		if (lhs.size_raw < rhs.size_raw)
			return (true);
		return (false);
	}

	static bool	compByName(const AutoIndexFile& lhs, const AutoIndexFile& rhs)
	{
		if (lhs.is_a_dir && !rhs.is_a_dir)
			return (true);
		if (!lhs.is_a_dir && rhs.is_a_dir)
			return (false);
		if (lhs.name < rhs.name)
			return (true);
		return (false);
	}

	string	toHtml() const
	{
		ostringstream is;

		is << "<tr>";
	}
};

ostream&	operator<<(ostream& out, const AutoIndexFile& file)
{
	out << left << setw(20) << file.name << ' ' << setw(40) << file.last_modified << ' ' << setw(20) << file.size << ' ' << boolalpha << file.is_a_dir << '\n';
	return (out);
}

/* RAII struct */
struct DirPtr
{
	operator DIR*() const
	{
		return (ptr);
	}

	DirPtr(DIR* dir) : ptr(dir)
	{
		if (!ptr)
			throw (std::runtime_error("::DirPtr"));
	}

	~DirPtr() {::closedir(ptr);}

	DIR*	ptr;
};



void	scan_cwd(list<AutoIndexFile>& file_list, const string& path)
{
	DirPtr			dir(::opendir(path.c_str()));
	struct dirent*	curr_dir = NULL;

	errno = 0;
	while ((curr_dir = ::readdir(dir)) != NULL)
	{
		struct stat	sbuf;

		if (fstatat(::dirfd(dir), curr_dir->d_name, &sbuf, 0) < 0)
			throw (std::runtime_error("::stat"));
		if (!(sbuf.st_mode & S_IRUSR))
			continue ;
		if (curr_dir->d_name[0] == '.')
				continue ;
		switch (sbuf.st_mode & S_IFMT)
		{
			case S_IFDIR:
				file_list.push_back(AutoIndexFile(curr_dir->d_name, sbuf.st_ctim.tv_sec, true));
			break;
			case S_IFREG:
				file_list.push_back(AutoIndexFile(curr_dir->d_name, sbuf.st_ctim.tv_sec, false, sbuf.st_size));
			break;
			default:
				;
		}
	}
	if (!curr_dir && errno != 0)
			throw (std::runtime_error("::readdir"));
}

int main(int argc, char**argv)
{
	list<AutoIndexFile>	curr_cwd;
	try
	{
		scan_cwd(curr_cwd,argv[1]);
		curr_cwd.sort(AutoIndexFile::compBySize);
	}
	catch (std::exception& e)
	{
		cerr << e.what() << ": " << strerror(errno) << '\n';
	}
	for (list<AutoIndexFile>::const_iterator it = curr_cwd.begin(); it != curr_cwd.end(); it++)
		cout << *it;
}
