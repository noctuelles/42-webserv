#include "MultiPartParser.hpp"
#include "RequestHandler.hpp"

namespace HTTP
{

	MultiPartParser::MultiPartParser(size_t content_lenght, const std::string& boundary) :
		Parser(ST_CHECK_BOUND_DASH1),
		m_hfield_parser(),
		m_content_lenght(content_lenght),
		m_boundary(boundary),
		cmp_it(m_boundary.begin())
	{}

	Buffer::const_iterator	MultiPartParser::operator()(const Buffer& buff, Buffer::const_iterator it)
	{
		if (m_current_state == ST_FILE_CONTENT)
			m_data.data_it.first = it;

		while (it != buff.end() && m_current_state != ST_DONE)
		{
			switch (m_current_state)
			{
				case ST_CHECK_BOUND_DASH1:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
						changeState(ST_CHECK_BOUND_DASH2);
					break;
				case ST_CHECK_BOUND_DASH2:
					if (*it != '-')
						throw (RequestHandler::Exception(BadRequest));
					else
					{
						cmp_it = m_boundary.begin();
						changeState(ST_CHECK_BOUND);
					}
					break;
				case ST_CHECK_BOUND:
					if (cmp_it == m_boundary.end())
					{
						if (!isCRLF(*it)) // Boundary doesn't match.
							throw (RequestHandler::Exception(BadRequest));
						transitionState(ST_CRLF, ST_PARSE_HEADER_FIELD);
					}
					else if(*it != *cmp_it)
						throw (RequestHandler::Exception(BadRequest));
					else
						cmp_it++;
					break;

				case ST_PARSE_HEADER_FIELD:
					it = m_hfield_parser(buff, it);
					if (m_hfield_parser.getState() == HeaderFieldParser::ST_DONE)
					{
						m_data.header_field = m_hfield_parser.get();
						m_eat = false;
						changeState(ST_FILE_ENTRY);
					}
					break;

				case ST_FILE_ENTRY:
					m_data.data_it.first = it;
					changeState(ST_FILE_CONTENT);
					break;

				case ST_FILE_CONTENT:
					break;

				default:
					;
			}
			if (!m_eat)
				m_eat = true;
			else
				it++;
		}
		return (it);
	}

	void	MultiPartParser::transitionState(int new_state, int next_state)
	{
		m_previous_state = m_current_state;
		changeState(new_state);
		m_next_state = next_state;
	}

	void	MultiPartParser::changeState(int new_state)
	{
		m_previous_state = m_current_state;
		m_current_state = new_state;
	}
}
