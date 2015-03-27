CC=g++
CFLAGS=-c -Wall 
CFLAGS += -Iinclude/ 
LDFLAGS= -lsqlite3
SOURCES= main.cpp towers.cpp routing.cpp send.cpp read.cpp
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
