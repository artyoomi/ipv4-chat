CC := gcc

CFLAGS += -g3
CFLAGS += -Wall -Wpedantic

LDFLAGS += -lpthread

SRCDIR   := src
BUILDDIR := build

SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))
HEADERS := $(wildcard $(SRCDIR)/*.h)

TARGET := $(BUILDDIR)/ipv4_chat

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECTS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
