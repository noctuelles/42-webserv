/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDescriptor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 08:13:13 by plouvel           #+#    #+#             */
/*   Updated: 2022/12/04 16:34:03 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEDESCRIPTOR_CLASS_HPP
# define FILEDESCRIPTOR_CLASS_HPP

# include <utility>

namespace IO
{
	class FileDescriptor
	{
		public:

			operator	int() const;

			explicit FileDescriptor(int fd);
			FileDescriptor(const FileDescriptor& other);
			FileDescriptor&	operator=(const FileDescriptor& rhs);
			virtual ~FileDescriptor();

			void	release() const;
			int		get() const;

			int		getFlags() const;
			void	setFlags(int flags) const;
			void	setBlockingMode(bool blocking) const;
			bool	isBlocking() const;
			void	setCloseOnExecMode(bool blocking) const;

		protected:

			mutable int				m_fd;
	};

	typedef std::pair<FileDescriptor, FileDescriptor>	FdPair;
}

#endif
