/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Traits.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 12:24:35 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/12 12:24:50 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Traits.hpp"

std::ostream&	operator<<(std::ostream& out, const ci_string& str)
{
	out << str.c_str();
	return (out);
}
