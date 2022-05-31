# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: youness <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/09/28 22:37:58 by youness           #+#    #+#              #
#    Updated: 2021/10/20 19:08:26 by yarroubi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

Author = younes

SERVER = server

CLIENT = client

NAME = $(SERVER) $(CLIENT)

CC = clang++
CFLAGS = #-Wall -Werror -Wextra -std=c++98 -fsanitize=address


CLIENT_SRC = client.cpp

SERVER_SRC = main.cpp

HDR = 

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(SERVER) : $(SERVER_SRC)
	@$(CC) $(CFLAGS) -o $@ $(SERVER_SRC)

$(CLIENT) : $(CLIENT_SRC)
	@$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC)

%.o: %.cpp
	@$(CC) $(CFLAGS) -o $@ -c $<

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY : clean fclean re