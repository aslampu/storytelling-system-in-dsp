CC=g++
CFLAGS=-Wall
LDFLAGS=
CPPEXECUTABLES=image_to_header

CPPOBJECTS=io_helper.o

image_to_header.SOURCES = io_helper.o

all: $(CPPEXECUTABLES)

$(CPPEXECUTABLES): $(CPPOBJECTS)
	$(CC) $(CFLAGS) $($@.SOURCES) $@.cc -o $@

$(CPPOBJECTS): %.o: %.cc
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(CPPEXECUTABLES)