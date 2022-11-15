/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/25 18:54:18 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 15:48:42 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CLASS_HPP
# define WEBSERV_CLASS_HPP

# include <functional>
# include <vector>
# include <algorithm>
# include <assert.h>
#include <iostream>

# include "Http.hpp"
# include "EPoll.hpp"
# include "FileUtils.hpp"
# include "VirtServInfo.hpp"

namespace ft
{
	class WebServ
	{
		public:

			typedef std::vector<InternetSocket*>	InSockVector;

			static const size_t			MaxErrorPageSize		= 2097152; // 2MB
			static const unsigned int	MaxPendingConnection	= 5;
			static const time_t			ConnectionTimeout		= 20; // in seconds.
			static const std::string	Version;

			WebServ(const char *config_filename);
			~WebServ();

			void	addListener(in_port_t port);
			void	run();

			const VirtServInfo&		getVirtServInfo() const { return m_virtserv_info; }

		private:

			static const int	TimeoutCheckOccurence = 1000;

			struct DeleteAndNullifyTimeoutSocket : std::unary_function<InternetSocket*&, void>
			{
				inline void	operator()(InternetSocket*& ptr)
				{
					if (ptr->isTimeout())
					{
						delete ptr;
						ptr = 0;
					}
				}
			};

			struct DeleteObj
			{
				template <class T>
					inline void	operator()(T* ptr)
					{
						delete ptr;
					}
			};

			EPoll				m_poller;
			InSockVector		m_socks;
			VirtServInfo		m_virtserv_info;

			WebServ(const WebServ& other);
			WebServ&	operator=(const WebServ& rhs);

			inline void	_addConnection(int fd);
			inline void	_removeSocket(InternetSocket* ptr);
			inline void	_removeTimeoutSocket();
	};
}

#endif
