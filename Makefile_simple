BUILDDIR  ?= build
SOURCEDIR ?= src

TARGET ?= ipv4-chat

CC ?= gcc

CFLAGS += -g3
CFLAGS += -Wall -Wextra -Wpedantic

LDFLAGS := -lpthread

OBJECTS ?= main.o \
		   recv_send.o \
		   regex_check.o

.PHONY: all
all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
	rm -rf *.o

main.o: main.c regex_check.h recv_send.h macro_funcs.h
	$(CC) -c $(CFLAGS) main.c -o main.o

recv_send.o: recv_send.c recv_send.h macro_funcs.h
	$(CC) -c $(CFLAGS) recv_send.c -o recv_send.o

regex_check.o: regex_check.c regex_check.h macro_funcs.h
	$(CC) -c $(CFLAGS) regex_check.c -o regex_check.o

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
