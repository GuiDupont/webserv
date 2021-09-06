/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exceptions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:09:27 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/06 14:57:17 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>

class config_file_error : public std::exception {
	
};

class config_file_not_open : public std::exception {
	public:
		virtual const char* what() const throw();
};

