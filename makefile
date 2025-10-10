OBJS = search.o
HEADERS = 
SOURCE = search.cpp
EXEC = search

CC =    g++
FLAGS = -Wall -g

all: search

search:search.cpp
	$(CC) -o search $(SOURCE)

clean:
	rm -f $(OBJS) $(EXEC)