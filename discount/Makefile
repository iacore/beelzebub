CC=gcc
CFLAGS=-std=gnu11 -c -Wall -Wextra -Wpedantic -g -Wno-unused-parameter
LDFLAGS=-Ldiscount -lmarkdown -lxml2
SOURCES=mdtohtml_test.c md2html.c xml.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=beelzebub

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o beelzebub
