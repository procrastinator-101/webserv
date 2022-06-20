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

NAME = webserv

CC = g++
CFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address


SRCS_PATH = srcs

# client
#================================================================================
CLIENT_PATH = $(SRCS_PATH)/client

CLIENT_SRC = $(CLIENT_PATH)/Client.cpp
CLIENT_HDR = $(CLIENT_PATH)/Client.hpp
#================================================================================

# header_field
#================================================================================
HEADER_FIELD_PATH = $(SRCS_PATH)/header_field

HEADER_FIELD_SRC = $(HEADER_FIELD_PATH)/HeaderField.cpp
HEADER_FIELD_HDR = $(HEADER_FIELD_PATH)/HeaderField.hpp
#================================================================================

# host
#================================================================================
HOST_PATH = $(SRCS_PATH)/host

HOST_SRC = $(HOST_PATH)/Host.cpp
HOST_HDR = $(HOST_PATH)/Host.hpp
#================================================================================

# http_status
#================================================================================
HTTP_STATUS_PATH = $(SRCS_PATH)/http_status

HTTP_STATUS_SRC = $(HTTP_STATUS_PATH)/HttpStatus.cpp
HTTP_STATUS_HDR = $(HTTP_STATUS_PATH)/HttpStatus.hpp
#================================================================================

# location
#================================================================================
LOCATION_PATH = $(SRCS_PATH)/location

LOCATION_SRC = $(LOCATION_PATH)/Location.cpp
LOCATION_HDR = $(LOCATION_PATH)/Location.hpp
#================================================================================

# multiplex
#================================================================================
MULTIPLEX_PATH = $(SRCS_PATH)/multiplex

MULTIPLEX_SRC = $(MULTIPLEX_PATH)/Selecter.cpp
MULTIPLEX_HDR = $(MULTIPLEX_PATH)/Action.hpp \
				$(MULTIPLEX_PATH)/Selecter.hpp
#================================================================================

# nginy
#================================================================================
NGINY_PATH = $(SRCS_PATH)/nginy

NGINY_SRC = $(NGINY_PATH)/Nginy.cpp
NGINY_HDR = $(NGINY_PATH)/Nginy.hpp
#================================================================================

# request
#================================================================================
REQUEST_PATH = $(SRCS_PATH)/request

REQUEST_SRC = $(REQUEST_PATH)/Request.cpp
REQUEST_HDR = $(REQUEST_PATH)/Request.hpp
#================================================================================

# response
#================================================================================
RESPONSE_PATH = $(SRCS_PATH)/response

RESPONSE_SRC = $(RESPONSE_PATH)/Response.cpp
RESPONSE_HDR = $(RESPONSE_PATH)/Response.hpp
#================================================================================

# server
#================================================================================
SERVER_PATH = $(SRCS_PATH)/server

SERVER_SRC = $(SERVER_PATH)/Server.cpp
SERVER_HDR = $(SERVER_PATH)/Server.hpp
#================================================================================

# sockets
#================================================================================
SOCKT_PATH = $(SRCS_PATH)/sockets

SOCKT_SRC = $(SOCKT_PATH)/ClientSockt.cpp \
			$(SOCKT_PATH)/ServerSockt.cpp \
			$(SOCKT_PATH)/Sockt.cpp

SOCKT_HDR = $(SOCKT_PATH)/ClientSockt.hpp \
			$(SOCKT_PATH)/ServerSockt.hpp \
			$(SOCKT_PATH)/Sockt.hpp
#================================================================================

# stdlib
#================================================================================
STDLIB_PATH = $(SRCS_PATH)/stdlib

STDLIB_SRC = $(STDLIB_PATH)/stdlib.cpp
STDLIB_HDR = $(STDLIB_PATH)/stdlib.hpp
#================================================================================


# includes
#--------------------------------------------------------------------------------
NGINY_CONF_PATH_INC = NGINY_CONF_PATH_INC=\"$$PWD/confFiles/default.conf\"
NGINY_VAR_PATH_INC = NGINY_VAR_PATH_INC=\"/goinfre/$$USER/var/nginy\"

INCLUDE_PARAMS = $(NGINY_CONF_PATH_INC) $(NGINY_VAR_PATH_INC)
#--------------------------------------------------------------------------------


SRC =	$(SRCS_PATH)/main.cpp $(CLIENT_SRC) $(HEADER_FIELD_SRC) $(HOST_SRC) \
		$(HTTP_STATUS_SRC) $(LOCATION_SRC) $(MULTIPLEX_SRC) $(NGINY_SRC) \
		$(REQUEST_SRC) $(RESPONSE_SRC) $(SERVER_SRC) $(SOCKT_SRC) $(STDLIB_SRC)

HDR =	$(CLIENT_HDR) $(HEADER_FIELD_HDR) $(HOST_HDR) $(HTTP_STATUS_HDR) $(LOCATION_HDR) \
		$(MULTIPLEX_HDR) $(NGINY_HDR) $(REQUEST_HDR) $(RESPONSE_HDR) $(SERVER_HDR) \
		$(SOCKT_HDR) $(STDLIB_HDR)


INC = $(INCLUDE_PARAMS:%=-D%)

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

$(NAME) : $(SRC) $(HDR)
	@$(CC) $(CFLAGS) $(INC) -o $@ $(SRC)

%.o: %.cpp
	@$(CC) $(CFLAGS) $(INC) -o $@ -c $<

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY : clean fclean re
