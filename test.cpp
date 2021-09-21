#include <string>
#include <iostream>

int main () {

	std::string _body = "abcdefghijklmno";
	size_t length = 10;
	std::string _left;
	
	if (_body.size() > length) {
		_left = _body.substr(length, _body.end() - (_body.begin() + length));
		_body = _body.substr(0, length);
	}
	std::cout << _body << std::endl;
	std::cout << _left << std::endl;
}