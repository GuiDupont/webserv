#include <unistd.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sstream>
#include <fcntl.h>

int main(int argc, char **argv, char **env) {

	int epfd;
	int	pipefd[2];
	pid_t pid;
	int nsfd;
	char *arg[3] = {"/usr/bin/php-cgi", "sleep.php", NULL};
	struct epoll_event *_revents;
	char buf[1000];

	_revents = (struct epoll_event *)calloc(64, sizeof(struct epoll_event));
	epfd = epoll_create(1);
	pipe(pipefd);
	// fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = pipefd[0];
	epoll_ctl(epfd, EPOLL_CTL_ADD, pipefd[0], &ev);

	pid = fork();
	if (pid == 0) { //fils
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		close(pipefd[1]);
		sleep(10);
		// write(1, "lol", 3);
		// shutdown(pipefd[1], 0);
		// shutdown(pipefd[0], 0);
		execve("/usr/bin/php-cgi", arg, NULL);
		sleep(3);
		exit(1);
	}
	close(pipefd[1]);
	while (1) {
		sleep(1);
		nsfd = epoll_wait(epfd, _revents, 64, 0);
		if (nsfd) {
			for (int i = 0; i < nsfd; i++) {
				if (_revents[i].events & EPOLLIN) {
					std::cout << "POLLIN on sock :" << _revents[i].data.fd << std::endl;
					int ret = read(pipefd[0], buf, 999);
					// buf[ret] = '\0';
					// std::cout << buf << std::endl;
				}
				if (_revents[i].events & EPOLLOUT)
					std::cout << "POLLOUT on sock :" << _revents[i].data.fd << std::endl;
				if (_revents[i].events & EPOLLERR)
					std::cout << "POLLERR on sock :" << _revents[i].data.fd << std::endl;
				if (_revents[i].events & EPOLLHUP)
					std::cout << "POLLHUP on sock :" << _revents[i].data.fd << std::endl;
			}
		}
	}
	return (0);
}