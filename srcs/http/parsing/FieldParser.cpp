#include "FieldParser.hpp"
#include "RequestHandler.hpp"

namespace HTTP
{
	FieldParser::FieldParser() :
		Parser(ST_VALUE),
		m_buffer(),
		m_eat(true),
		m_f(NULL)
	{}

	FieldParser::~FieldParser()
	{}

	ContentInfo	FieldParser::operator()(const std::string& str)
	{
		std::string::const_iterator it = str.begin();

		while (it != str.end())
		{
			switch (m_current_state)
			{
				case ST_VALUE:
					switch (*it)
					{
						case ' ':
						case '\t':
							transitionState(ST_OWS, ST_SEMICOLON);
							break;
						case ';':
							transitionState(ST_OWS, ST_PARAM_KEY);
							break;
						default:
							m_data.value.push_back(*it);
					}
				break;

				case ST_SEMICOLON:
					if (*it != ';')
						throw (RequestHandler::Exception(BadRequest));
					else
						changeState(ST_PARAM_KEY);
					break;

				case ST_PARAM_KEY:
					switch (*it)
					{
						case ' ':
						case '\t':
							throw (RequestHandler::Exception(BadRequest));
						case '=':
							changeState(ST_PARAM_VALUE);
							break;
						default:
							m_buffer.first.push_back(*it);
					}
					break;

				case ST_PARAM_VALUE:
					switch (*it)
					{
						case ' ':
						case '\t':
							transitionState(ST_OWS, ST_SEMICOLON, &FieldParser::insertParam);
							break;
						case ';':
							transitionState(ST_OWS, ST_PARAM_KEY, &FieldParser::insertParam);
							break;
						case '"':
							break;
						default:
							m_buffer.second.push_back(*it);
					}
					break;

				case ST_OWS:
					if (!isWS(*it))
					{
						m_eat = false;
						if (m_f)
							(this->*m_f)();
						changeState(m_next_state);
					}
				break;

				default:
					;
			}
			if (!m_eat)
				m_eat = true;
			else
				it++;
		}

		if (!m_buffer.first.empty() || !m_buffer.second.empty())
			m_data.param.insert(m_buffer);
		return (m_data);
	}

	void	FieldParser::insertParam()
	{
		m_data.param.insert(m_buffer);
		m_buffer.first.clear();
		m_buffer.second.clear();
	}

	void	FieldParser::transitionState(int new_state, int next_state, callBackFnct f)
	{
		m_previous_state = m_current_state;
		changeState(new_state);
		m_next_state = next_state;
		m_f = f;
	}

	void	FieldParser::changeState(int new_state)
	{
		m_previous_state = m_current_state;
		m_current_state = new_state;
	}
}
