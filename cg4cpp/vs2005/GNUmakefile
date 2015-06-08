
TARGET = generate_cg4cpp_autoexp

UNAME := $(shell uname)

SRCS = $(TARGET:=.c)
OBJS = $(SRCS:.c=.o)

CC = gcc
CFLAGS += -Wall

ifeq ($(UNAME), Darwin)
else
  ifeq ($(findstring CYGWIN, $(UNAME)), CYGWIN)
    EXE = .exe
  else
  endif
endif

BINARY := $(TARGET:=$(EXE))

all: cg4cpp_autoexp.txt

cg4cpp_autoexp.txt : $(BINARY)
	$(RM) $@
	./$(BINARY) > $@

$(BINARY) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(CLINKFLAGS)

clean:
	$(RM) $(BINARIES) $(OBJS)

clobber: clean
	$(RM) *.bak *.o *~

.PHONY: all clean clobber
