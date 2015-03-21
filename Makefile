CC ?= cc
CFLAGS ?= -O3 -Wall -Wextra
LDFLAGS ?= -s
PREFIX ?= /usr/local
EXECUTABLE = skype2-mempatch
SOURCES = skype2-mempatch.c
OBJECTS = $(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

.PHONY: clean install

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

install:
	install -Dm 0755 $(EXECUTABLE) $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE)

