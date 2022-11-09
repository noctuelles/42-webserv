/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/17 18:34:52 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/09 16:14:09 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientSocket.hpp"
#include "ResponseHeader.hpp"
#include "Utils.hpp"
#include "WebServ.hpp"
#include "HTTP.hpp"
#include "RequestParser.hpp"
#include "env_g.hpp"

#include <ios>
#include <sstream>
#include <netinet/in.h>
#include <new>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <sstream>

#ifndef NDEBUG
# include "DebugColors.h"
#endif

namespace ft
{
	std::vector<uint8_t>	ClientSocket::m_recv_buffer(MaxRecvBufferSize);
	std::vector<uint8_t>	ClientSocket::m_send_buffer(MaxSendBufferSize);

	ClientSocket::ClientSocket(int fd, const std::vector<http::StatusInfo>& stat_info)
		: InternetSocket(fd),
		m_method_fnct(),
		m_recv_bytes(0),
		m_sent_bytes(0),
		m_parser(),
		m_last_activity(time(NULL)),
		m_state(FETCHING_REQUEST_HEADER),
		m_status_code(http::OK),
		m_stat_info(stat_info),
		m_conn_info(),
		m_header_fields()
	{
		setBlockingMode(false);
		this->m_len = sizeof(struct sockaddr_in);
		if (getsockname(fd, reinterpret_cast<struct sockaddr*>(&m_sockaddr), &m_len) < 0)
			throw (std::runtime_error("getsockname"));
	}

	int	ClientSocket::recv()
	{
		m_last_activity = time(NULL);
		m_recv_bytes = ::recv(*this, m_recv_buffer.data(), m_recv_buffer.size(), 0); // noexcept
		std::cout << "\t## " << UYEL << "Readed " << m_recv_bytes << " bytes" << CRST << " ##\n";

		if (m_recv_bytes <= 0)
			return (DISCONNECT);
#ifndef NDEBUG
		else
		{
			std::cout << "\t## " << UYEL << "Raw Data from the socket" << CRST << " ##\n";
			write(STDOUT_FILENO, m_recv_buffer.data(), m_recv_bytes);
			std::cout << "\n\t## " << UYEL << "End Raw Data" << CRST << " ##\n";
		}
#endif
		// When done, the socket should be registered to EPOLLOUT.
		switch (m_state)
		{
			case FETCHING_REQUEST_HEADER:
			{
				int ret = 0;

				try
				{ ret = m_parser.parse(m_recv_buffer, static_cast<size_t>(m_recv_bytes)); }
				catch (const std::bad_alloc& e)
				{ return (DISCONNECT); }

#ifndef NDEBUG
				m_parser.report();
#endif
				switch (ret) // parse could throw exception.
				{
					case http::RequestParser::P_DONE:
						try
						{
							// Maybe this can move into parsing ? Would it be hard to parse field directly into a map ?
							if (std::count_if(m_parser.getHeaderFields().begin(), m_parser.getHeaderFields().end(), http::IsHostField()) != 1)
								throw (std::logic_error("invalid number of host field"));
							m_header_fields.insert(m_parser.getHeaderFields().begin(), m_parser.getHeaderFields().end());
							//
							const std::string& host_val = m_header_fields[http::Field::Host().toLower()];

							// Get the list of virtual servers that `listen` to that host:port
							{
								const std::vector<VirtServ*>& vservers = env_g->getVirtServInfo()[m_sockaddr];
								std::vector<VirtServ*>::const_iterator first = vservers.begin();
								std::vector<VirtServ*>::const_iterator it = first;
								std::vector<VirtServ*>::const_iterator end = vservers.end();
								for(; it != end; ++it)
								{
									if (host_val == (*it)->m_server_name)
									{
										m_conn_info = *first; // Lucky us
										break;
									}
								}
								if ( it == first ) // Not even one candidate for that host:port. Let's try our luck further down
									;
								else if ( it == end ) // Matching vserver name not found. Default to first candidate
								{
									m_conn_info = *first;
									break;
								}
							}
							// Get the list of virtual servers that `listen` to 0.0.0.0:port
							{
								sockaddr_in sockaddr = m_sockaddr;
								sockaddr.sin_addr.s_addr = 0;
								const std::vector<VirtServ*>& vservers = env_g->getVirtServInfo()[sockaddr];
								std::vector<VirtServ*>::const_iterator first = vservers.begin();
								std::vector<VirtServ*>::const_iterator it = first;
								std::vector<VirtServ*>::const_iterator end = vservers.end();
								for(; it != end; ++it)
									if (host_val == (*it)->m_server_name)
										break;
								if ( it == first ) // Not even one candidate for 0.0.0.0:port. This is a lost cause.
								{
									m_state = DISCONNECT;
									break;
								}
								else if ( it == end ) // Matching vserver name not found. Default to first candidate
								{
									m_conn_info = *first;
									break;
								}
							}
						}
						catch (const std::bad_alloc& e)
						{
							return (DISCONNECT);
						}
						catch (const std::logic_error& e)
						{
							m_state = SENDING_RESPONSE_HEADER;
							m_status_code = http::BadRequest;
							break;
						}

						break;

					case http::RequestParser::P_DONE_ERR:
						m_state = SENDING_RESPONSE_HEADER;
						m_status_code = m_parser.getErrorCode();
						break;

					default: break;
				}
				break;
			}
			case FETCHING_REQUEST_BODY:
				// To do: chunked request. No parsing is involved otherwise.
				break;
			default:
				;
		}
		return (m_state);
	}

	int	ClientSocket::send()
	{
		m_last_activity = time(NULL);
		switch (m_state)
		{
			case SENDING_RESPONSE_HEADER:
			{
				http::ResponseHeader	respHeader(m_stat_info[m_status_code].phrase);

				respHeader.addField(http::Field::Server(), WebServ::Version);
				respHeader.addField(http::Field::Date(), utils::getRFC822NowDate());

				if (m_status_code != http::OK)
				{
					respHeader.addField(http::Field::ContentType(), http::MIME::TextHtml());
					respHeader.addField(http::Field::ContentLenght(), utils::integralToString(m_stat_info[m_status_code].page.first));
					respHeader.addField(http::Field::Connection(), "closed");
				}

				if (::send(this->getFd(), respHeader.toCString(), respHeader.size(), 0) < 0)
					m_state = DISCONNECT;
				else
					m_state = SENDING_RESPONSE_BODY;
			}
				break;
			case SENDING_RESPONSE_BODY:
				if (m_status_code != http::OK)
				{
					::send(*this, m_stat_info[m_status_code].page.second.c_str(), m_stat_info[m_status_code].page.first, 0);
					m_state = DISCONNECT;
				}
				else
				{
					// Sending with get !
				}
				break;
			default:
				;
		}
		return (m_state);
	}

	bool	ClientSocket::isTimeout()
	{
		return (time(NULL) - m_last_activity >= WebServ::ConnectionTimeout);
	}

	void	_methodGet()
	{
	}
}
