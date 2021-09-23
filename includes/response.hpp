/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/22 14:51:32 by gdupont           #+#    #+#             */
/*   Updated: 2021/09/23 11:06:28 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ANSWER_HPP
# define ANSWER_HPP

# include <map>
# include <string>

# include "request.hpp"

class request;

class response {
    public:
        response(request & request);
    


    public:
        std::string     status_line;
};



// <html>
// <head><title>400 Bad Request</title></head>
// <body>
// <center><h1>400 Bad Request</h1></center>
// <hr><center>webserv/1</center>
// </body>
// </html>


# endif