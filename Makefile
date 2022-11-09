###################
##  VARIABLES   ##
##################
DEFAULT_CONFIG	= config
DEBUG			= -DDEBUG
## Our beloved address sanitizer
ASAN_FLAG		=  -fsanitize=address

MAKEFLAGS		+= --no-builtin-rules

CXX				= clang++-14 -gdwarf-4
CXX				= g++-12
CXX				= c++

SHELL			= bash

NAME			= webserv

SRCS_DIR		= srcs
OBJS_DIR		= objs
INC_DIR			= includes

INC_DIRS		+= $(INC_DIR) 
INC_DIRS		+= $(wildcard $(INC_DIR)/*/) 
INC_DIRS		+= $(wildcard $(INC_DIR)/*/*/) #etc.

PATH/SOURCES	+= $(wildcard $(SRCS_DIR)/*.cpp) 
PATH/SOURCES	+= $(wildcard $(SRCS_DIR)/*/*.cpp) 
PATH/SOURCES	+= $(wildcard $(SRCS_DIR)/*/*/*.cpp) #etc.

PATH/OBJECTS	= $(patsubst	$(SRCS_DIR)/%.cpp,	$(OBJS_DIR)/%.o,	$(PATH/SOURCES) )
PATH/DEPS		= $(patsubst	%.o,				%.d,				$(PATH/OBJECTS) )

## Preprocessor flags
COMMON_HEADERS	= $(wildcard $(INC_DIR)/common/* )
IMACROS_FLAGS	= $(addprefix -imacros , $(COMMON_HEADERS) ) #Headers in inc_dir/common will be automatically included in each file
INCLUDE_FLAGS	= $(addprefix -I, $(INC_DIRS) )
DEFINES			= ${DEBUG} -DDEFAULT_CONFIG=\"$(DEFAULT_CONFIG)\"
CPPFLAGS		+= $(INCLUDE_FLAGS) $(IMACROS_FLAGS) $(DEFINES) -MMD #output .d dependencies rules to be included

## Compile flags
CXXFLAGS		= -Wall -Wextra -std=c++98 -g3 $(ASAN_FLAG)
ifdef DEBUG
CXXFLAGS		+= -Wno-unused
else
CXXFLAGS		+= -Werror 
endif

## Link flags
LDFLAGS			=	$(ASAN_FLAG)
LDLIBS			=


RM				= rm -rf

##############
##  RULES   ##
##############

all:			$(NAME)

# Reminder :			LDFLAGS (-L) always come before oject files !
$(NAME):				$(PATH/OBJECTS)
						@echo -e '\e[032mLinking...\e[0m'
						${CXX} -o $@ ${LDFLAGS} $^ ${LDLIBS}

$(OBJS_DIR)/%.o:		$(SRCS_DIR)/%.cpp Makefile
						@mkdir -p $(dir $@)
						${CXX} ${CPPFLAGS} ${CXXFLAGS} -c $< -o $@
clean:
						$(RM) $(OBJS_DIR)

fclean:					clean
						$(RM) $(NAME)

re:						fclean all

-include $(OBJS/DEPS)

.PHONY:			all clean fclean re
