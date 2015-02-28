CC=g++
CFLAGS=-c -Wall
CFLAGS += -Iinclude/
LDFLAGS=
SOURCES= main.cpp towers.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=simulator

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS)
	rm $(EXECUTABLE)



#usage
#make -f makefile clean all
