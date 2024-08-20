BUILDDIR  ?= build
SOURCEDIR ?= src

TARGET ?= $(BUILDDIR)/ipv4-chat

CC ?= gcc

CFLAGS += -g3
CFLAGS += -Wall -Wextra -Wpedantic

LDFLAGS := -lpthread

OBJECTS ?= main.o \
		   		 recv_send.o \
		       regex_check.o

dir_guard := @mkdir -p $(BUILDDIR)

.PHONY: all
all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

main.o: main.c regex_check.h recv_send.h macro_funcs.h
	$(dir_guard)
	$(CC) -c $(CFLAGS) main.c -o $(BUILDDIR)/main.o

recv_send.o: recv_send.c recv_send.h macro_funcs.h
	$(CC) -c $(CFLAGS) recv_send.c -o $(BUILDDIR)/recv_send.o

regex_check.o: regex_check.c regex_check.h macro_funcs.h
	$(CC) -c $(CFLAGS) regex_check.c -o $(BUILDDIR)/regex_check.o

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
