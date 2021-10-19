/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/22 14:56:44 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:14:06 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "response.hpp"

std::string			response::get_allowed_functions_header(size_t & disabled_method) {
	std::string header_field("Allow: ");

	if (disabled_method == (GET + DELETE + POST))
		return (header_field + "\r\n");
	if (!(disabled_method & GET))
		header_field += "GET, ";
	if (!(disabled_method & DELETE))
		header_field += "DELETE, ";
	if (!(disabled_method & POST))
		header_field += "POST, ";
	header_field = header_field.substr(0, header_field.size() - 2);
	return (header_field + "\r\n");
}

std::string			response::get_last_time_modified_header(std::string path) {
    struct stat sstat;
        struct tm * ptm;
    std::string header("Last-Modified: ");
    stat(path.c_str(), &sstat);

    ptm = gmtime (&sstat.st_mtim.tv_sec);
    ptm->tm_hour += 2;
    ptm->tm_hour %= 24;
    header += asctime(ptm);
    return (header + "\r\n");
}

std::string			response::get_date_header() {
    time_t rawtime;
    time (&rawtime);
    struct tm * ptm;
    std::string header("Date: ");
    ptm = gmtime (&rawtime);
    ptm->tm_hour += 2;
    ptm->tm_hour %= 24;
    std::string tmp = asctime(ptm);
    tmp = tmp.substr(0, tmp.size() - 1);
    header += tmp;
    return (header + "\r\n");
}

std::string         response::get_server_header() {
    return ("Server: webserv/1\r\n");
}

std::string         response::get_return_location_header(std::string & return_location) {
    std::string header("Location: ");
    header += return_location;
    header += "\r\n";
    return (header);
}

long get_file_size(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

long            response::get_content_length(request & req) { // make sure it's complete
    if (!req.body_response.empty())
        return req.body_response.size();
    long file_size = get_file_size(req.conf->path_to_target);
    if (file_size == -1) {
        req.code_to_send = 500;
        req.body_response = response::generate_error_body(g_webserv.status_code.find(req.code_to_send)->second);
        return req.body_response.size();
    }
    else
        return (file_size);
}

std::string         response::get_content_length_header(int content_length) {
    std::string header("Content-Length: ");
    header += ft_itos(content_length);
    header += "\r\n";
    return (header);
}

std::string         response::generate_header(request & req) {
    std::string header = get_status_line(req);
    header += get_server_header();
    header += get_date_header();
    add_special_header(req, header);
    if (req.method == "GET") {
        header += get_content_length_header(get_content_length(req));
    }
    else if (req.method == "DELETE") {
        ;
    }
    else if (req.method == "POST") {
        header += get_content_length_header(get_content_length(req));
    }
    header += "\r\n";
    return (header);
}

std::string         response::get_status_line(request & req) {
    std::map<int, std::string>::iterator it = g_webserv.status_code.find(req.code_to_send);
    if (it == g_webserv.status_code.end()) {
        req.code_to_send = 500;
        req.close_csock = true;
    }
    std::string status_line("HTTP/1.1 ");
    status_line += g_webserv.status_code.find(req.code_to_send)->second;
    status_line += "\r\n";
    return (status_line);
}

std::string         response::generate_error_body(std::string & message) {
    std::string body_response = "<html><head><title>";
    body_response += message;
    body_response += "</title></head>\n<body>\n<center><h1>";
    body_response += message;
    body_response += "</h1></center>\n<hr><center>webserv/1</center>\n</body>\n</html>";
    body_response += "\r\n";
    return (body_response);
}

std::string         response::generate_autoindex_body(std::string & path, std::string & request_target) {
    std::string body_response = "<html><head><title>" + path + "</title></head>\n<body>\n<h1>";
   
    DIR *d;
    struct dirent *dir;
    d = opendir(path.c_str());
    body_response += "AUTOINDEXING<br>\n<table style=\"border-collapse: collapse; width: 100%;\" border=\"3\"><tbody>";

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            std::string file_name(dir->d_name);
			if (file_name == ".")
				continue;
            body_response += "<tr><td style=\"width: 100%;\"><a href=\"";
			body_response += request_target;
			if (request_target[request_target.size() - 1] != '/')
                body_response += "/";
            body_response += dir->d_name;
            if (is_directory(file_name) && file_name[file_name.size() - 1] != '/')
                body_response += "/";
            body_response += "\">";
            body_response += dir->d_name + std::string("</a><br></td></tr>\n");
        }
    closedir(d);
    }
    body_response += "</tbody></table</h1></center>\n<hr><center>webserv/1\n</body>\n</html>";
    body_response += "\r\n";
    return (body_response);
}

std::string         response::add_special_header(request & req, std::string & header) {
    if (req.conf->return_activated)
        header += response::get_return_location_header(req.conf->return_pair.second);
    else if (req.code_to_send == 405)
       header += response::get_allowed_functions_header(req.conf->disable_methods);
	else if (req.conf->cgi_activated == true)
		header += req.cgi->cgi_header_fields;
    return (header);
}
