#include <string>
#include <iostream>
#include <list>

class request {
    
    public:

		std::list<std::string>				_trailer;
		void	param_trailer(request &req, std::string str);
};

// void request::param_trailer(request &req, std::string str) {

// 	std::string::iterator it = str.begin();
// 	std::string::iterator it2;
// 	std::string::iterator it3;
// 	std::string substr;

// 	if (str.size() == 0)
// 		return ;
// 	for (; it != str.end(); it++) {
// 		for (; it != str.end() && std::isspace(*it) != 0; it++) {}
// 		if (str.find(',', it - str.begin()) != std::string::npos)
// 			it2 = str.begin() + str.find(',', it - str.begin());
// 		else
// 			it2 = str.end();
// 		substr = str.substr(it - str.begin(), it2 - it);
// 		it3 = it2 - 1;
// 		for (; it3 - it >= 0 && std::isspace(*it3) != 0; it3--) {}
// 		substr = substr.substr(0, it3 - it + 1);
// 		req._trailer.push_back(substr);
// 		it = (it2 == str.end() ? it2 - 1 : it2);
// 	}
// 	return ;
// }

int main () {

	std::string str = "7\r\nolala   \r\n";
	// request test;
	
	std::string substr = str.substr(str.find("\r\n", 0) + 2, str.find("\r\n", str.find("\r\n", 0) + 2) - (str.find("\r\n", 0) + 2));
	// test.param_trailer(test, str);
	// for (std::list<std::string>::iterator it = test._trailer.begin(); it != test._trailer.end(); it++) {
	// 	std::cout << *it << ", size = " << it->size() << std::endl;
	// }
	std::cout << substr << ", size = " << substr.size() << std::endl;
}