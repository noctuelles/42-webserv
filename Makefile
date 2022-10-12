##################
##  VARIABLES   ##
##################
MAKEFLAGS		+= --no-builtin-rules

CXX				= clang++-14 -gdwarf-4
CXX				= g++-12
CXX				= c++

SHELL			= bash

NAME			= webserv

SOURCES			= $(wildcard *.cpp)
OBJ/OBJECTS		= $(patsubst	%.cpp,  obj/%.o,	$(SOURCES))
OBJ/DEPS		= $(patsubst	%.o,    %.d,		$(OBJ/OBJECTS))

#DEBUG			= -DDEBUG

## C preprocessor flags
INCLUDE_FLAGS	= 
CPPFLAGS		= ${INCLUDE_FLAGS} -MMD

## Add -Werror before correction
CXXFLAGS		= -Wall -Wextra -std=c++98 -g3 -pedantic-errors #-Werror 
LDFLAGS			=
LDLIBS			=

## Our beloved address sanitizer
#ASAN_FLAG		=  -fsanitize=address
CXXFLAGS		+=	$(ASAN_FLAG)
LDFLAGS			+=	$(ASAN_FLAG)

##############
##  RULES   ##
##############

all:			$(NAME)

# Reminder : LDFLAGS (-L) always come before oject files !
$(NAME):		$(OBJ/OBJECTS)
				@echo -e '\e[032mLinking...\e[0m'
				${CXX} -o $@ ${LDFLAGS} $^ ${LDLIBS}

obj/%.o:		%.cpp Makefile | obj
				${CXX} ${DEBUG} ${CPPFLAGS} ${CXXFLAGS} -c $< -o $@

obj:
				mkdir obj
clean:
				rm -rf obj

fclean:			clean
				rm -rf $(NAME)

re:				fclean all

-include $(OBJ/DEPS)

.PHONY:			all clean fclean re diff
