/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:36:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/06 14:56:19 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/exceptions.hpp"

const char* config_file_not_open::what() const throw() {
	return ("We can't open the config file");
}
