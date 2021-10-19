/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 14:03:24 by gdupont           #+#    #+#             */
/*   Updated: 2021/10/19 15:23:58 by gdupont          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.hpp"

/* BRACKETS */

bool		webserv_parser::check_brackets(const std::string & config) {
	size_t 	position = 0;
	while (1) {
		position = config.find('{', position);
		if (position == std::string::npos)
			break ;
		position = get_next_closing_bracket(config, position + 1);
		if (position == std::string::npos)
			return (false);
	}
	return (check_closing_brackets(config));
}


size_t		webserv_parser::get_next_closing_bracket(const std::string & config, size_t position) {
	size_t next_open_bracket = config.find('{', position);
	size_t next_closing_bracket = config.find('}', position);
	
	if (next_closing_bracket == std::string::npos)
		return (std::string::npos);
	if (next_open_bracket < next_closing_bracket)
	{
		position = get_next_closing_bracket(config, next_open_bracket + 1);
		if (position == std::string::npos)
			return (std::string::npos);
		position = config.find('}', position + 1);
		if (position == std::string::npos)
			return (std::string::npos);
		else
			return (position);
	}
	return (next_closing_bracket);
}

bool 		webserv_parser::check_closing_brackets(const std::string & config) {
	
	int open_count = 0;
	int close_count = 0;
	int position = 0;

	while (config.find('{', position) != std::string::npos) {
		position = config.find('{', position) + 1;
		open_count++;
	}
	position = 0;
	while (config.find('}', position) != std::string::npos) {
		position = config.find('}', position) + 1;
		close_count++;
	}
	if (close_count != open_count)
		return (false);
	else
		return (true);	
}

/* END BRACKETS */

/* 		UTILS		*/

size_t		webserv_parser::parse_disabled_methods(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string method;
	size_t disabled_methods = 0;
	while (1) {
		method = get_word(line, i);
		if (method == "GET")
			disabled_methods |= GET;
		else if (method == "POST")
			disabled_methods |= POST;
		else if (method == "DELETE")
			disabled_methods |= DELETE;
		else
			throw (non_existing_disabled_methods());
		i = go_to_next_word(line, i);
		if (!line[i])
			return (disabled_methods);
	}
	return (disabled_methods);
}

std::string	webserv_parser::parse_one_word(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string path = get_word(line, i);
	i = go_to_next_word(line, i);
	if (line[i])
		throw (bad_nb_argument(""));
	return (path);
}

bool	webserv_parser::parse_auto_index(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string on_off = get_word(line, i);
	i = go_to_next_word(line, i);
	if (line[i])
		throw (bad_nb_argument("autoindex"));
	if (on_off == "on")
		return true;
	if (on_off == "off")
		return false;
	else
		throw (bad_auto_index_value());
}

void	webserv_parser::parse_cgi_extension(std::set< std::string > & cgi_ext, std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	while (line[i]) {
		cgi_ext.insert(get_word(line, i));
		i = go_to_next_word(line, i);
	}
}

size_t	webserv_parser::get_max_body_size(std::string & line) {
	int i = go_to_next_word(line, 0);
	std::string max_size = line.substr(i, line.find_first_of(WHITESPACE, i) - i);
	if (!ft_string_is_digit(max_size) || (line.find_first_of(WHITESPACE, i) != line.npos) )
		throw (bad_client_max_body_size());
	return (atoi(max_size.c_str()));
}


std::pair<int, std::string> webserv_parser::parse_return(std::string &line) {

	if (count_words(line) != 3)
		throw (bad_nb_argument("return"));
	int i = go_to_next_word(line, 0);
	std::string code_str = get_word(line, i);
	if (!ft_string_is_digit(code_str))
		throw (bad_error_page_value());
	int code = atoi(code_str.c_str());
	if (code < 0 || code > 999 )
		throw (bad_error_page_value());
	i = go_to_next_word(line, i);
	std::string path = get_word(line, i);
	return (std::pair<int, std::string>(code, path));
}

std::pair<int, std::string> webserv_parser::parse_error_page(std::string & line) {
	int i = go_to_next_word(line, 0);
	if (!line[i])
		throw (empty_declaration());
	std::string code_str = get_word(line, i);
	if (!ft_string_is_digit(code_str))
		throw (bad_error_page_value());
	int code = atoi(code_str.c_str());
	if (code < 300 || code > 599 )
		throw (bad_error_page_value());
	i = go_to_next_word(line, i);
	std::string path = get_word(line, i);
	if (path.empty())
		throw (empty_declaration());
	i = go_to_next_word(line, i);
	if (line[i])
		throw (bad_nb_argument("error_page"));
	return (std::pair<int, std::string>(code, path));	
	
}

std::string webserv_parser::parse_server_name(std::string &line) {

	if (count_words(line) != 2)
		throw (bad_nb_argument("server_name"));
	int i = go_to_next_word(line, 0);
	std::string str = get_word(line, i);
	return (str);
}

void	webserv_parser::parse_listen(std::string &line, vHost &host) {

    regex_t regex;
    int reti;
	int port;
	std::string addr("0.0.0.0");

	if (count_words(line) != 2)
		throw (bad_nb_argument("listen"));
	int i = go_to_next_word(line, 0);
	std::string str = get_word(line, i);
	if (is_ip(str) == 1) {
		addr = parse_ip(str);
		str = str.substr(str.find(':', 0) + 1, (str.size() - str.find(':', 0) + 1));
	}
	reti = regcomp(&regex, "^[0-9]\\{0,5\\}$", 0);
    reti = regexec(&regex, str.c_str(), 0, NULL, 0);
	if( !reti )
		port = std::atoi(str.c_str());
    else {
    	regfree(&regex);
		throw (bad_port());
    }
    regfree(&regex);
	host.getHost_Port().push_back(std::pair<std::string, int>(addr, port));
	return ;
}

std::string webserv_parser::parse_upload_pass(std::string &line) {

	if (count_words(line) != 2)
		throw (bad_nb_argument("upload_pass"));
	int i = go_to_next_word(line, 0);
	std::string str = get_word(line, i);
	return (str);
}

std::string	webserv_parser::parse_ip(std::string str) {

        regex_t regex;
        int reti;
		std::string addr;

        reti = regcomp(&regex, "^[0-9]\\{1,3\\}[.][0-9]\\{1,3\\}[.][0-9]\\{1,3\\}[.][0-9]\\{1,3\\}:", 0);
        reti = regexec(&regex, str.c_str(), 0, NULL, 0);
        if( !reti )
            addr = str.substr(0, str.find(':', 0));
        else {
			regfree(&regex);
			throw (bad_ip_address());
        }
    	regfree(&regex);
        return (addr);
}

void 		webserv_parser::check_server_line(std::string &line) {

	std::string	sd_word;
	size_t	i;

	if (count_words(line) != 2)
		throw (bad_nb_argument("server"));
	for (i = 0; i < line.size() && std::isspace(line[i]) != 0; i++) {}
	for (; i < line.size() && std::isspace(line[i]) == 0; i++) {}
	for (; i < line.size() && std::isspace(line[i]) != 0; i++) {}
	sd_word = line.substr(i, line.size() - i);
	if (sd_word != "{")
		throw bad_server_declaration();
}

void	webserv_parser::analyse_body(request &req) {

	std::string substr;
	size_t index = 0;

	g_logger.fd << g_logger.get_timestamp() << "We are parsing body from ccosk: " << req.csock << std::endl;

	if (req.header_fields.find("content-length") == req.header_fields.end() && !is_chunked(req)) {
		req.set_request_to_ended();
		if (req.left.size() != 0)
			req.left.clear();
	}
	else if (is_chunked(req)) {
		while (req.left.empty() != 1) {
			if (req.next_chunk > -1) {
				if (static_cast<int>(req.left.size()) < req.next_chunk + 2)
					break;
				else {
					if (req.next_chunk == 0) {
						if (req.left[req.next_chunk] != '\r' && req.left[req.next_chunk + 1] != '\n') {
							req.code_to_send = 400;
							req.set_request_to_ended();
							return ;
						}
						req.left = req.left.substr(2, req.left.size());
						req.next_chunk = -2;
						if (req.nb_trailer_to_received == 0) {
							req.set_request_to_ended();
							return ;
						}
					}
					else {
						req.body_request += req.left.substr(0, req.next_chunk);
						if (static_cast<int>(req.body_request.size()) > req.conf->client_max_body_size) {
								req.code_to_send = 413;
								req.set_request_to_ended();
								return ;
						}
						if (req.left[req.next_chunk] != '\r' || req.left[req.next_chunk + 1] != '\n') {
							req.code_to_send = 400;
							req.set_request_to_ended();
							return ;
						}
						req.left = req.left.substr(req.next_chunk + 2, req.left.size() - (req.next_chunk + 2));
						req.next_chunk = -1;
					}
				}
			}
			else if (req.next_chunk == -1) {
				if (req.left.find("\r\n", 0) != std::string::npos) {
					substr = req.left.substr(0, req.left.find("\r\n", 0));
					if (req.is_valid_chunk_size(substr) == 0) {
						req.code_to_send = 400;
						req.set_request_to_ended();
						return ;
					}
					for (std::string::iterator it = substr.begin(); it != substr.end(); it++) {
						*it = std::tolower(*it);
					}
					req.next_chunk = ft_atoi_base(substr.c_str(), "0123456789abcdef");
					req.left = req.left.substr(req.left.find("\r\n", 0) + 2, req.left.size() - (req.left.find("\r\n", 0) + 2));
				}
				else
					break;
			}
			else {
				if (req.left.find("\r\n", 0) != std::string::npos) {
					std::pair<std::string, std::string> header_field;
					std::string header_field_raw = get_word(req.left, index, std::string("\r\n"));
					size_t semi_colon_index =  header_field_raw.find(":", 0);
					if (semi_colon_index == std::string::npos) {
						req.code_to_send = 400;
						req.set_request_to_ended();
						return ;
					}
					header_field = std::pair<std::string, std::string>(header_field_raw.substr(0, semi_colon_index), 
					header_field_raw.substr(semi_colon_index + 1, header_field_raw.size() - semi_colon_index));
					header_field.second = trims(header_field.second, " \t");
					if (!header_field.first.size() || !header_field.second.size() 
					|| header_field.first[header_field.first.size() - 1] == '\t'
					|| !is_token(header_field.first) || !is_field_content(header_field.second) || req.find_trailer_in_list(header_field.first) == 0) {
						req.code_to_send = 400;
						req.set_request_to_ended();
						return ;
					}
					req.header_fields.insert(header_field);
					req.nb_trailer_to_received = req.nb_trailer_to_received - 1;
					req.trailer.remove(header_field.first);
					req.left = req.left.substr(req.left.find("\r\n", 0) + 2, req.left.size() - (req.left.find("\r\n", 0) + 2));
					if (req.nb_trailer_to_received == 0) {
						req.set_request_to_ended();
						return ;
					}
				}
				else
					break;
			}
		}
	}
	else if (req.header_fields.find("content-length") != req.header_fields.end()) {
		size_t length = std::atoi(req.header_fields.find("content-length")->second.c_str());
		req.body_request += req.left;
		req.left.clear();
		if (static_cast<int>(req.body_request.size()) > req.conf->client_max_body_size) {
			req.code_to_send = 413;
			req.set_request_to_ended();
			return ;
		}
		if (req.body_request.size() >= length) {
			req.body_request = req.body_request.substr(0, length);
			req.set_request_to_ended();
			req.left.clear();
		}
	}
}

void	webserv_parser::analyse_header(request &req) {
	// g_logger.fd << g_logger.get_timestamp() << "We are parsing header from ccosk: " << req.csock << std::endl;
	if (req.left.find(std::string("\r\n\r\n"), 0) != std::string::npos) {
		size_t index = 0;
		std::string request_line = req.left.substr(0, req.left.find(std::string("\r\n")));
		if (request_line.size() > 8000) {
			req.code_to_send = 400;
			req.set_request_to_ended();
			req.conf = new config(req);
			return ;
		}
		req.method = get_word(req.left, index, std::string(" "));
		if (req.method != "GET" && req.method != "DELETE" && req.method != "POST")
		{
			req.code_to_send = 400;
			req.set_request_to_ended();
			req.conf = new config(req);
			return ;
		}
		req.request_target = get_word(req.left, index , std::string(" "));
		if (req.request_target.empty() || req.request_target[0] != '/' || is_valid_request_target(req.request_target) == 0)  // test nginx with charset of segment wrong https://datatracker.ietf.org/doc/html/rfc3986#section-3.3
		{
			req.code_to_send = 400;
			req.set_request_to_ended();
			req.conf = new config(req);
			return ;
		}
		cancel_double_dots_in_request_target(req.request_target);
		req.HTTP_version = get_word(req.left, index, std::string("\r\n"));
		if (req.HTTP_version.size() == 0 || ft_string_has_whitespace(req.HTTP_version))
		{
			req.code_to_send = 400;
			req.set_request_to_ended();
			req.conf = new config(req);
			return ;
		}
		if (req.HTTP_version != "HTTP/1.1" && req.HTTP_version != "HTTP/1.0") {
			req.code_to_send = 505;
			req.set_request_to_ended();
			req.conf = new config(req);
			return ;
		}
		while (index < req.left.size())
		{
			std::pair<std::string, std::string> header_field;
			std::string header_field_raw = get_word(req.left, index, std::string("\r\n"));
			if (header_field_raw.size() == 0)
				break ;
			size_t semi_colon_index =  header_field_raw.find(":", 0);
			if (semi_colon_index == std::string::npos) {
				req.code_to_send = 400;
				req.set_request_to_ended();	
				req.conf = new config(req);
				return ;
			}
			header_field = std::pair<std::string, std::string>(header_field_raw.substr(0, semi_colon_index), 
			header_field_raw.substr(semi_colon_index + 1, header_field_raw.size() - semi_colon_index)); 
			for (std::string::iterator it = header_field.first.begin(); it != header_field.first.end(); it++) {
				*it = std::tolower(*it);
			}
			header_field.second = trims(header_field.second, " \t");
			if (!header_field.first.size() || !header_field.second.size() 
			|| header_field.first[header_field.first.size() - 1] == '\t'
			|| !is_token(header_field.first) || !is_field_content(header_field.second))
			{
				req.code_to_send = 400;
				req.set_request_to_ended();
				req.conf = new config(req);
				return ;
			}
			if (req.header_fields.find(header_field.first) != req.header_fields.end())
			{
				req.code_to_send = 400;
				req.set_request_to_ended();
				req.conf = new config(req);
				return ;
			}
			req.header_fields.insert(header_field);
		}
		if (req.header_fields.find("host") == req.header_fields.end()) {
			g_logger.fd << g_logger.get_timestamp() << "Pb de Host sur csock : " << req.csock << std::endl;
			req.code_to_send = 400;
			req.set_request_to_ended();
			req.conf = new config(req);
			return ;
		}
		if (req.header_fields.find("content-length") != req.header_fields.end()) {
			if (is_valid_content_length(req.header_fields.find("content-length")->second) == 0) {
				req.code_to_send = 400;
				req.set_request_to_ended();
				req.conf = new config(req);
				return ;
			}
		}
		if (req.header_fields.find("trailer") != req.header_fields.end()) {
			req.param_trailer(req.header_fields.find("trailer")->second);
		}
		req.left = req.left.substr(index, req.left.size() - index);
		req.stage = 1;
		req.conf = new config(req);
		analyse_body(req);
	}
}

void webserv_parser::parse_cgi_body_response(request &req) {
	
	std::string status_code;
	size_t		status_pos;

	size_t end_of_hf = req.body_response.find("\r\n\r\n");
	if (end_of_hf == std::string::npos) {
		req.close_csock = true;
		req.code_to_send = 500;
		req.conf->cgi_activated = false;
		return ;
	}	
	req.cgi->cgi_header_fields = req.body_response.substr(0, end_of_hf + 2);
	req.body_response = req.body_response.substr(end_of_hf + 4, req.body_response.size() - (end_of_hf + 4));
	if (req.cgi->cgi_header_fields.substr(0, 7) == "Status:") {
		status_code = get_word(req.cgi->cgi_header_fields, go_to_next_word(req.cgi->cgi_header_fields, 7));
		if (ft_string_is_digit(status_code))
			req.code_to_send = std::atoi(status_code.c_str());
		else
			req.code_to_send = 200;
	}
	else if ((status_pos = req.cgi->cgi_header_fields.find("\r\nStatus:", 0)) != std::string::npos) {
		status_code = get_word(req.cgi->cgi_header_fields, go_to_next_word(req.cgi->cgi_header_fields, status_pos + 9));
		if (ft_string_is_digit(status_code))
			req.code_to_send = std::atoi(status_code.c_str());
		else
			req.code_to_send = 200;
	}
	else
		req.code_to_send = 200;
}

void	webserv_parser::cancel_double_dots_in_request_target(std::string & target) {
	size_t pos_next_location;
	size_t begin_previous_location;

	while (1) {
		pos_next_location = target.find("/../", 0);
		if (pos_next_location == std::string::npos) {
			pos_next_location = target.find("/..", 0);
			if (pos_next_location != std::string::npos && (pos_next_location + 3 == target.size())) {
				target += '/';
			}
			else
				return ;
		}
		begin_previous_location = pos_next_location - 1;
		pos_next_location += 3;
		
		while (target[begin_previous_location]) {
			if (target[begin_previous_location] == '/')
				break ;
			begin_previous_location--;		
		}
		target = target.substr(0, begin_previous_location) + target.substr(pos_next_location, target.size() - pos_next_location);
	}
}
