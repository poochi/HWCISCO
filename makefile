CC=g++
CFLAGS=-c -Wall 
CFLAGS += -Iinclude/ 
LDFLAGS= -lsqlite3
SOURCES= main.cpp towers.cpp routing.cpp send.cpp read.cpp database.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=simulator

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)



#usage
#make -f makefile clean all
