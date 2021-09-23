/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/22 14:56:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/22 16:41:17 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <response.hpp>

response::response(request & request) {
    status_line = "HTTP/1.1 ";
    status_line += ft_itos(request._code_to_send);
    status_line += " " + g_webserv.status_code.find(request._code_to_send)->second + "\r\n\r\n\r\n";
    std::cout << status_line << std::endl;
}