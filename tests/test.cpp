#include <unistd.h>
#include <string>
#include <map>
#include "../includes/request.hpp"

class CGI {

public:

	CGI() {}
	~CGI() {}

	void param_CONTENT_LENGTH();
	void param_CONTENT_TYPE();
	void param_GATEWAY_INTERFACE();
	void param_PATH_INFO();
	void param_PATH_TRANSLATED();
	void param_QUERY_STRING();
	void param_REMOTE_ADDR();
	void param_REQUEST_METHOD();
	void param_SCRIPT_NAME();
	void param_SERVER_NAME();
	void param_SERVER_PORT();
	void param_SERVER_PROTOCOL();
	void param_SERVER_SOFTWARE();

private:

	std::string CONTENT_LENGTH; // seulement si bdy
	std::string CONTENT_TYPE; // seulement si present dans client request
	std::string GATEWAY_INTERFACE = "CGI/1.1";
	std::string PATH_INFO; // = ce qui suit le nom du script/fichier
	std::string PATH_TRANSLATED; // obligatoire ? = transformation in server document
	std::string QUERY_STRING; // = ce qui suit le '?' ou bien ""
	std::string REMOTE_ADDR; // obligatoire ? = host
	std::string REQUEST_METHOD; // GET ou POST
	std::string SCRIPT_NAME; // path to script
	std::string SERVER_NAME = "42webserv";
	std::string SERVER_PORT;
	std::string SERVER_PROTOCOL = "HTTP/1.1";
	std::string SERVER_SOFTWARE = "42webserv";
	char *env[14];

}

CGI::param_CONTENT_LENGTH(Request &req) {

	if (req.method == "GET") {
		this->CONTENT_LENGTH = "CONTENT_LENGTH=";
	else {
		this->CONTENT_LENGTH = "CONTENT_LENGTH=" + req.body.size(); // à modif possiblement
	}
	}
}

void	perform_CGI(Request &req, std::string::iterator it) {

	CGI cgi;

	cgi.param_CONTENT_LENGTH(req)


}

int main(int argc, char **argv, char **env) {

	// char arg1[] = "/usr/bin/php";
	// char *arg2[] = {"/usr/bin/php", "hello.php"};
	// execve(arg1, arg2, NULL);

	Request req;
	std::string REQUEST = "/hello.php";
	std::string::iterator it = REQUEST.end();
	perform_CGI(req, it);
	return (0);
}