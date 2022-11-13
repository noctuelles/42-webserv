#ifndef HTTPMIME_HPP
# define HTTPMIME_HPP

# include <map>

# include "Traits.hpp"

namespace ft
{
	namespace http
	{
		class MIME
		{
			public:

				operator	std::string()
				{
					return (str);
				}

				static MIME	TextHtml()					{return MIME("text/html");}
				static MIME	TextPlain()					{return MIME("text/plain");}
				static MIME	TextCss()					{return MIME("text/css");}
				static MIME	TextJavascript()			{return MIME("text/javascript");}
				static MIME	ImageXIcon()				{return MIME("image/x-icon");}
				static MIME	ImageJpeg()					{return MIME("image/jpeg");}
				static MIME	ImagePng()					{return MIME("image/png");}
				static MIME	ImageBmp()					{return MIME("image/bmp");}
				static MIME	ImageGif()					{return MIME("image/gif");}
				static MIME	ApplicationOctetStream()	{return MIME("application/octet-stream");}

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

		/* filepath must include the full path of the file */
		const MIME	getMimeFromFileExtension(const std::string& filepath);
	}
}

#endif
