# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/29 18:01:34 by plouvel           #+#    #+#              #
#    Updated: 2022/11/01 16:02:54 by plouvel          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

###################
##  VARIABLES   ##
##################
MAKEFLAGS		+= --no-builtin-rules

CXX				= clang++-14 -gdwarf-4
CXX				= g++-12
CXX				= c++

SHELL			= bash

NAME			= webserv

SRCS_DIR		= srcs
OBJS_DIR		= objs
INC_DIR			= includes

SOURCES			= http/RequestParser.cpp		\
				  http/ResponseHeader.cpp		\
				  http/HTTP.cpp					\
				  http/Client.cpp				\
				  io/socket/ListeningSocket.cpp	\
				  io/EPoll.cpp					\
				  io/FileDescriptor.cpp			\
				  io/FileUtils.cpp				\
				  WebServ.cpp					\
				  main.cpp

OBJS/OBJECTS	= $(addprefix $(OBJS_DIR)/, $(SOURCES:.cpp=.o))
OBJS/DEPS		= $(patsubst	%.o,    %.d,		$(OBJS/OBJECTS))

#DEBUG			= -DDEBUG

## C (and c++) preprocessor flags
INCLUDE_FLAGS	= -I $(INC_DIR) \
				  -I $(INC_DIR)/http \
				  -I $(INC_DIR)/io \
				  -I $(INC_DIR)/io/socket

CPPFLAGS		= $(INCLUDE_FLAGS)

## Add -Werror before correction
CXXFLAGS		= -Wall -Wextra -std=c++98 -g3 #-Werror 
LDFLAGS			=
LDLIBS			=

## Our beloved address sanitizer
ASAN_FLAG		=  -fsanitize=address
CXXFLAGS		+=	$(ASAN_FLAG)
LDFLAGS			+=	$(ASAN_FLAG)

RM				= rm -rf

##############
##  RULES   ##
##############

all:			$(NAME)

# Reminder : LDFLAGS (-L) always come before oject files !
$(NAME):		$(OBJS/OBJECTS)
				@echo -e '\e[032mLinking...\e[0m'
				${CXX} -o $@ ${LDFLAGS} $^ ${LDLIBS}

$(OBJS_DIR)/%.o:		$(SRCS_DIR)/%.cpp Makefile | $(OBJS_DIR) $(OBJS_DIR)/http $(OBJS_DIR)/io $(OBJS_DIR)/io/socket
				${CXX} ${DEBUG} ${CPPFLAGS} ${CXXFLAGS} -c $< -o $@

$(dir $@):
	@mkdir -p $@

$(OBJS_DIR):
				@mkdir $@ 

$(OBJS_DIR)/http:
				@mkdir $@ 

$(OBJS_DIR)/io:
				@mkdir $@ 

$(OBJS_DIR)/io/socket:
				@mkdir $@ 

clean:
				$(RM) $(OBJS_DIR)

fclean:			clean
				$(RM) $(NAME)

re:				fclean all

-include $(OBJS/DEPS)

.PHONY:			all clean fclean re
