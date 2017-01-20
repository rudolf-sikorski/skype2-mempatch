CC ?= cc
CFLAGS ?= -O3
LDFLAGS ?= -s -lreadline
PREFIX ?= /usr/local
EXECUTABLE = skype2-mempatch
SOURCES = \
	scanmem-0.13/commands.c \
	scanmem-0.13/handlers.c \
	scanmem-0.13/list.c \
	scanmem-0.13/main.c \
	scanmem-0.13/maps.c \
	scanmem-0.13/menu.c \
	scanmem-0.13/ptrace.c \
	scanmem-0.13/scanroutines.c \
	scanmem-0.13/show_message.c \
	scanmem-0.13/target_memory_info_array.c \
	scanmem-0.13/value.c \
	skype2-mempatch.c
OBJECTS = $(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

.PHONY: clean install

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -I. $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

install:
	install -Dm 0755 $(EXECUTABLE) $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE)

