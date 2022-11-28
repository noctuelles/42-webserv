CFLAGS	+=	-Wall -Wextra
SRCS	= $(wildcard *.cpp)
SCRIPTS	= $(patsubst %.cpp, %.cgi, $(SRCS))

all:	$(SCRIPTS)

%.cgi:	%.o
		c++	${LDFLAGS}	-o	$@	$^

.PHONY: all

