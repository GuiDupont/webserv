#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>


int main(void)
{
    fd_set rfds;
    struct timeval tv;
    int retval;


    /* Surveiller stdin (fd 0) en attente d'entrées */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* Pendant 5 secondes maxi */
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(0, &rfds, NULL, NULL, &tv);
    /* Considérer tv comme indéfini maintenant ! */

    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Des données sont disponibles maintenant\n");
        /* FD_ISSET(0, &rfds) est vrai */
    else
        printf("Aucune données durant les 5 secondes\n");

    exit(EXIT_SUCCESS);
}

// typedef struct fd_set {
//   u_int  fd_count;
//   SOCKET fd_array[FD_SETSIZE];
// } fd_set, FD_SET, *PFD_SET, *LPFD_SET;

// FD_CLR(fd, &fdset)  
// Clears the bit for the file descriptor fd in the file descriptor set fdset.

// FD_ISSET(fd, &fdset)  
// Returns a non-zero value if the bit for the file descriptor fd is set in the file descriptor set pointed to by fdset, and 0 otherwise.

// FD_SET(fd, &fdset)  
// Sets the bit for the file descriptor fd in the file descriptor set fdset.

// FD_ZERO(&fdset)  
// Initializes the file descriptor set fdset to have zero bits for all file descriptors.


//http://manpagesfr.free.fr/man/man2/select.2.html
// clang++-9 -Wall -Wextra -Werror -std=c++98 -fsanitize=address select.cpp  && ./a.out

