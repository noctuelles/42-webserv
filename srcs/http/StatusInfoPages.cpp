/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusInfoPages.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/15 14:09:14 by plouvel           #+#    #+#             */
/*   Updated: 2022/11/15 14:11:58 by plouvel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StatusInfoPages.hpp"

namespace ft
{
	namespace http
	{
		vector<StatusInfo>* StatusInfoPages::m_singleton = NULL;
	}
}
