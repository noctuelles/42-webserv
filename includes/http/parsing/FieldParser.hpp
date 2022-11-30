#ifndef FIELDPARSER_HPP
# define FIELDPARSER_HPP

# include "Parser.hpp"

namespace HTTP
{
	class FieldParser : public Parser<ContentInfo>
	{
		public:
			enum State
			{
				ST_VALUE,
				ST_SEMICOLON,
				ST_PARAM_KEY,
				ST_PARAM_VALUE,
				ST_PARAM_VALUE_QUOTE,
				ST_OWS,
				ST_DONE
			};

			FieldParser();
			~FieldParser();
			Buffer::const_iterator	operator()(const Buffer& buff, Buffer::const_iterator it) {(void) buff;return (it);}
			ContentInfo	operator()(const std::string& str);

		private:

			typedef void	(FieldParser::*callBackFnct)();

			void	transitionState(int new_state, int next_state, callBackFnct f = NULL);
			void	changeState(int new_state);
			void	insertParam();

			std::pair<std::string, std::string>		m_buffer;
			bool			m_eat;
			callBackFnct	m_f;
	};
}

#endif
