# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gdupont <gdupont@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/07 11:26:53 by gdupont           #+#    #+#              #
#    Updated: 2021/09/01 11:14:17 by gdupont          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserver

CXX = /usr/lib/llvm-12/bin/clang++

FLAGS = -Wall -Wextra -Werror

PATH_SRCS = sources

SRCS_FILES = socket.cpp

SRCS = $(addprefix ${PATH_SRCS}/, ${SRCS_FILES})

VERSION = --std=c++98

SANFLAGS = -g -fsanitize=address

OBJS = ${SRCS:.cpp=.o}

all: ${NAME}

$(NAME): ${OBJS}
	${CXX} ${FLAGS} ${SANFLAGS} ${VERSION} -o ${NAME} ${OBJS}

%.o: %.cpp
	${CXX} ${FLAGS} ${SANFLAGS} ${VERSION} -c $< -o $@

clean:
	rm -rf ${OBJS}

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean