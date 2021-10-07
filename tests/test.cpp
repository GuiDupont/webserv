#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>

int main(int argc, char **argv, char **env) {

	std::string s1 = "eivjdvodfj emvodfijvdmovldfkmc dopc kvdjc giobvj fdovij ";
	std::ostringstream test(s1);

	// std::cout << test.str();
	write(1, s1.c_str(), 15);
	return (0);
}