# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ade-garr <ade-garr@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/07 11:26:53 by gdupont           #+#    #+#              #
#    Updated: 2021/10/13 16:31:32 by ade-garr         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

# CXX = clang++-9
#CXX = clang++
# CXX = /usr/lib/llvm-12/bin/clang++
CXX = /usr/bin/clang++

FLAGS = 
# -Wall -Wextra -Werror

INCLUDES = ./includes/

PATH_SRCS = sources

SRCS_FILES = 	exceptions.cpp main.cpp webserv.cpp utils.cpp vhost.cpp \
				location.cpp request.cpp parser.cpp logger.cpp config.cpp \
				CGI.cpp response.cpp

SRCS = $(addprefix ${PATH_SRCS}/, ${SRCS_FILES})

HEADER = ${INCLUDES}*.hpp

VERSION = --std=c++98

SANFLAGS = -g -fsanitize=address

OBJS = ${SRCS:.cpp=.o}

all: ${NAME}

$(NAME): ${OBJS} 
	${CXX} ${FLAGS} ${SANFLAGS} -I ${INCLUDES} ${VERSION} -o ${NAME} ${OBJS}

%.o: %.cpp ${HEADER}
	${CXX} ${FLAGS} ${SANFLAGS} -I ${INCLUDES} ${VERSION}  -c $< -o $@

clean:
	rm -rf ${OBJS}

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean
