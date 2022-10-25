/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   FileDescriptor.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/20 08:13:13 by plouvel           #+#    #+#             */
/*   Updated: 2022/10/20 08:26:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILEDESCRIPTOR_CLASS_HPP
# define FILEDESCRIPTOR_CLASS_HPP

class FileDescriptor
{
	public:

		FileDescriptor(int fd);
		FileDescriptor(const FileDescriptor& other);
		virtual ~FileDescriptor();

		int		getFd() const;
		int		getFdFlags() const;
		void	setFdFlags(int flags) const;

	protected:

		int				_M_fd;

	private:

		FileDescriptor&	operator=(const FileDescriptor& rhs);
		mutable bool	_M_should_close;
};

#endif
