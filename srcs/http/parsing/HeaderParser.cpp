/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 18:06:36 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/26 23:07:01 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HeaderParser.hpp"
#include "RequestHandler.hpp"

namespace HTTP
{
	HeaderParser::HeaderParser() :
		Parser(ST_START_REQUEST_LINE),
		m_index(0),
		m_hfield_parser()
	{}

	HeaderParser::~HeaderParser()
	{}

	Buffer::const_iterator	HeaderParser::operator()(const Buffer& buff, Buffer::const_iterator it)
	{
		unsigned char	ch = 0;

		while (it != buff.end() && m_current_state != ST_DONE)
		{
			ch = *it;
			switch (m_current_state)
			{
				case ST_START_REQUEST_LINE:
					switch (ch)
					{
						case 'G':
							m_data.method = Get;
							m_index = 1;
							changeState(ST_PARSE_METHOD);
							break;
						case 'P':
							m_data.method = Post;
							m_index = 1;
							changeState(ST_PARSE_METHOD);
							break;
						case 'D':
							m_data.method = Delete;
							m_index = 1;
							changeState(ST_PARSE_METHOD);
							break;
						default:
							throw (RequestHandler::Exception(NotImplemented));
					};
				break;

				case ST_PARSE_METHOD:
					if (ch != MethodTable[m_data.method][m_index])
					{
						if (isWS(ch) && MethodTable[m_data.method][m_index] == '\0')
							transitionState(ST_OWS, ST_PARSE_REQ_LINE);
						else
							throw (RequestHandler::Exception(BadRequest));
					} else m_index++;
				break;

				case ST_PARSE_REQ_LINE:
					if (m_data.request_line.size() >= MaxUriSize)
						throw (RequestHandler::Exception(UriTooLong));
					switch (ch)
					{
						case ' ':
						case '\t':
							this->parseURI(m_data.request_line);
							transitionState(ST_OWS, ST_HTTP), m_index = 0;
							break;
						/* https://httpwg.org/specs/rfc9112.html#request.target */
						case '\n':
						case '\f':
						case '\r':
							throw (RequestHandler::Exception(BadRequest));
							break;
						default:
							m_data.request_line.push_back(ch);
					}
				break;

				case ST_HTTP:
					if (ch != HttpSlash[m_index])
					{
						if (HttpSlash[m_index] == '\0')
						{
							m_eat = false;
							changeState(ST_HTTP_MAJOR_VER);
						}
						else
							throw (RequestHandler::Exception(BadRequest));
					} else m_index++;
				break;

				case ST_HTTP_MAJOR_VER:
					if (!isdigit(ch))
						throw (RequestHandler::Exception(BadRequest));

					m_data.ver_major = Utils::charToIntegral<int>(ch);
					if (m_data.ver_major != MajorVersionSupported)
						throw (RequestHandler::Exception(VersionNotSupported));

					changeState(ST_HTTP_DOT);
				break;

				case ST_HTTP_DOT:
					if (ch != '.')
						throw (RequestHandler::Exception(BadRequest));
					changeState(ST_HTTP_MINOR_VER);
				break;

				case ST_HTTP_MINOR_VER:
					if (!isdigit(ch))
						throw (RequestHandler::Exception(BadRequest));

					m_data.ver_minor = Utils::charToIntegral<int>(ch);
					if (m_data.ver_minor != MinorVersionSupported)
						throw (RequestHandler::Exception(VersionNotSupported));
					transitionState(ST_CRLF, ST_PARSE_HEADER_FIELD);
				break;

				case ST_PARSE_HEADER_FIELD:
					it = m_hfield_parser(buff, it);
					m_eat = false;
					if (m_hfield_parser.getState() == HeaderFieldParser::ST_DONE)
					{
						m_data.header_field = m_hfield_parser.get();
						changeState(ST_DONE);
					}
				break;

				case ST_CRLF:
					if (ch != '\r')
					{
						if (ch != '\n')
							throw (RequestHandler::Exception(BadRequest));
						changeState(m_next_state);
					}
				break;

				case ST_OWS:
					if (!isWS(ch))
					{
						m_eat = false;
						changeState(m_next_state);
					}
				break;
			}
			if (!m_eat)
				m_eat = true;
			else
			{
				it++;
				if (++m_nbr_parsed >= MaxHeaderSize)
					throw (RequestHandler::Exception(BadRequest));
			}
		}
		if (m_current_state == ST_DONE)
		{
			/* https://www.rfc-editor.org/rfc/rfc7230.html#section-5.4 */

			try
			{
				m_data.header_field.at(Field::Host());
			}
			catch (const std::logic_error& e)
			{
				throw (RequestHandler::Exception(BadRequest));
			}
		}
		return (it);
	}

	void	HeaderParser::parseURI(const string& uri)
	{
		char	ch = 0;

		m_current_state = ST_START_URI;
		for (string::const_iterator it = uri.begin(); it != uri.end(); it++)
		{
			ch = *it;
			switch (m_current_state)
			{
				case ST_START_URI:
					ch = std::tolower(ch);
					switch (ch)
					{
						case '/':
							/* https://httpwg.org/specs/rfc9112.html#origin-form */
							changeState(ST_ABSOLUTE_PATH);
							break;
						default:
							throw (RequestHandler::Exception(BadRequest));
					}
					break;

				case ST_ABSOLUTE_PATH:
					ch = *it;
					if (ch == '?')
						changeState(ST_QUERY);
					else
						m_data.uri.absolute_path.push_back(ch);
				break;

				case ST_QUERY:
					ch = *it;
					m_data.uri.query.push_back(ch);
				break;

				default:
					;
			}
		}
	}


	void	HeaderParser::transitionState(int new_state, int next_state)
	{
		m_previous_state = m_current_state;
		changeState(new_state);
		m_next_state = next_state;
	}

	void	HeaderParser::changeState(int new_state)
	{
		m_previous_state = m_current_state;
		m_current_state = new_state;
	}
}
