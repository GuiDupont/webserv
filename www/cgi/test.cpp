#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>



void	cancel_double_dots_in_request_target(std::string & target) {
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


int main(int argc, char **argv, char **env) {

	std::string s1 = "/location/../coucou";
	std::string s2 = "/location/../coucou/./flfl/..ss/../..rrrs/lol";
	std::string s3 = "/../../coucou";
	std::string s4 = "/location/../cou..cou/..";
	std::string s5 = "/location/../coucou/.././lol/../.";
	std::ostringstream test(s1);

	cancel_double_dots_in_request_target(s1);
	cancel_double_dots_in_request_target(s2);
	cancel_double_dots_in_request_target(s3);
	cancel_double_dots_in_request_target(s4);
	cancel_double_dots_in_request_target(s5);
	std::cout << s1 << std::endl;
	std::cout << s2 << std::endl;
	std::cout << s3 << std::endl;
	std::cout << s4 << std::endl;
	std::cout << s5 << std::endl;
	// write(1, s1.c_str(), 15);
	return (0);
}