#ifndef MULTIPARTPARSER_HPP
# define MULTIPARTPARSER_HPP

# include "Parser.hpp"
# include "HeaderFieldParser.hpp"

namespace HTTP
{
	class MultiPartParser : public Parser<MultiPartInfo>
	{
		public:

			enum State
			{
				ST_CHECK_BOUND_DASH1,
				ST_CHECK_BOUND_DASH2,
				ST_CHECK_BOUND,
				ST_PARSE_HEADER_FIELD,
				ST_FILE_ENTRY,
				ST_FILE_CONTENT,
				ST_CRLF,
				ST_DONE
			};

			MultiPartParser(size_t content_lenght, const std::string& boundary);

			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it) = 0;

		private:

			void	transitionState(int new_state, int next_state);
			void	changeState(int new_state);

			HeaderFieldParser	m_hfield_parser;
			std::string::const_iterator	cmp_it;
			std::string	m_boundary;
	};
}

#endif
