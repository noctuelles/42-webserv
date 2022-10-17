##################
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

SOURCES			= main.cpp \
				  ServerSocket.cpp \
				  EPoll.cpp \

OBJS/OBJECTS	= $(addprefix $(OBJS_DIR)/, $(SOURCES:.cpp=.o))
OBJS/DEPS		= $(patsubst	%.o,    %.d,		$(OBJS/OBJECTS))

#DEBUG			= -DDEBUG

## C (and c++) preprocessor flags
INCLUDE_FLAGS	= -I $(INC_DIR)
CPPFLAGS		= $(INCLUDE_FLAGS) -MMD

## Add -Werror before correction
CXXFLAGS		= -Wall -Wextra -std=c++98 -g3 -pedantic-errors #-Werror 
LDFLAGS			=
LDLIBS			=

## Our beloved address sanitizer
#ASAN_FLAG		=  -fsanitize=address
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

$(OBJS_DIR)/%.o:		$(SRCS_DIR)/%.cpp Makefile | $(OBJS_DIR)
				${CXX} ${DEBUG} ${CPPFLAGS} ${CXXFLAGS} -c $< -o $@

$(OBJS_DIR):
				mkdir $@ 

clean:
				$(RM) $(OBJS_DIR)

fclean:			clean
				$(RM) $(NAME)

re:				fclean all

-include $(OBJS/DEPS)

.PHONY:			all clean fclean re
