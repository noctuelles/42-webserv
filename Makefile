# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: plouvel <plouvel@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/10/29 18:01:34 by plouvel           #+#    #+#              #
#    Updated: 2022/11/01 18:34:17 by plouvel          ###   ########.fr        #
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

ALL_INC_DIRS	= $(INC_DIR) $(wildcard $(INC_DIR)/*/.) $(wildcard $(INC_DIR)/*/*/.) #trailing dot ensures that we'll get directories only, nice glibc glob() trick when you are unsure

PATH/SOURCES	= $(wildcard $(SRCS_DIR)/*.cpp) $(wildcard $(SRCS_DIR)/*/*.cpp) $(wildcard $(SRCS_DIR)/*/*/*.cpp) #etc.

PATH/OBJECTS	= $(patsubst	$(SRCS_DIR)/%.cpp,	$(OBJS_DIR)/%.o,	$(PATH/SOURCES) )
PATH/DEPS		= $(patsubst	%.o,	%.d,	$(PATH/OBJECTS) )

#DEBUG			= -DDEBUG

## Preprocessor flags
INCLUDE_FLAGS	+= $(addprefix -I, $(ALL_INC_DIRS) )
CPPFLAGS		+= $(INCLUDE_FLAGS) -MMD #output .d dependencies rules to be included

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
$(NAME):		$(PATH/OBJECTS)
				@echo -e '\e[032mLinking...\e[0m'
				${CXX} -o $@ ${LDFLAGS} $^ ${LDLIBS}

$(OBJS_DIR)/%.o:		$(SRCS_DIR)/%.cpp Makefile
						@mkdir -p $(dir $@)
						${CXX} ${DEBUG} ${CPPFLAGS} ${CXXFLAGS} -c $< -o $@
clean:
				$(RM) $(OBJS_DIR)

fclean:			clean
				$(RM) $(NAME)

re:				fclean all

-include $(OBJS/DEPS)

.PHONY:			all clean fclean re
