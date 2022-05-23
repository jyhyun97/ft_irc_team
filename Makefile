# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jeonhyun <jeonhyun@student.42seoul.kr>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/22 15:29:02 by swang             #+#    #+#              #
#    Updated: 2022/05/23 13:57:45 by jeonhyun         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 

SRC_DIR = src/

SRC_NAME = main.cpp

SRCS = $(addprefix $(SRC_DIR), $(SRC_NAME))

OBJS = $(SRCS:.cpp=.o)

.cpp.o :
	$(CXX) $(CXXFLAGS) -c $< -o $@


all : $(NAME)

$(NAME) : $(OBJS)
		$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean :
		rm -rf $(OBJS)

fclean :clean
		rm -rf $(NAME)

re : fclean all

.PHONY : all clean fclean re