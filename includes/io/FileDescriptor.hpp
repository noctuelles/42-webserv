/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDescriptor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 08:13:13 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/08 15:48:37 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEDESCRIPTOR_CLASS_HPP
# define FILEDESCRIPTOR_CLASS_HPP

namespace ft
{
	class FileDescriptor
	{
		public:

			operator	int() const;

			FileDescriptor(int fd);
			FileDescriptor(const FileDescriptor& other);
			FileDescriptor&	operator=(const FileDescriptor& rhs);
			virtual ~FileDescriptor();

			int		getFd() const;
			int		getFdFlags() const;
			void	setFdFlags(int flags) const;

		protected:

			int				m_fd;

		private:

			mutable bool	m_should_close;
	};
}

#endif
