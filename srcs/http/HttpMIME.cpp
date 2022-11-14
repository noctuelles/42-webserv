#include <algorithm>
#include <stdexcept>

#include "HttpMIME.hpp"

using std::string;

namespace ft
{
	namespace http
	{
		typedef std::pair<const ci_string, const MIME>	MimePair;
		typedef std::map<ci_string, const MIME>			MimeMap;

		const MimePair	MIMETable[] =
		{
			std::make_pair(".html", MIME::TextHtml()),
			std::make_pair(".txt", MIME::TextPlain()),
			std::make_pair(".css", MIME::TextCss()),
			std::make_pair(".js", MIME::TextJavascript()),
			std::make_pair(".jpg", MIME::ImageXIcon()),
			std::make_pair(".jpg", MIME::ImageJpeg()),
			std::make_pair(".jpeg", MIME::ImageJpeg()),
			std::make_pair(".png", MIME::ImagePng()),
			std::make_pair(".bmp", MIME::ImageBmp()),
			std::make_pair(".gif", MIME::ImageGif())
		};

		const MimeMap	MIMEFileExtensionMap(MIMETable, MIMETable + sizeof(MIMETable) / sizeof(MIMETable[0]));

		const MIME	getMimeFromFileExtension(const string& filepath)
		{
			string::size_type	i = filepath.rfind('.');
			
			if (i == string::npos)
				return (MIME::ApplicationOctetStream());

			string::const_iterator			filepath_dot	= filepath.begin() + i;

			try
			{
				return (MIMEFileExtensionMap.at(ci_string(filepath_dot, filepath.end())));
			}
			catch (const std::logic_error& e)
			{
				return (MIME::ApplicationOctetStream());
			}
		}
	}
}
