OBJS = search.o parameters.o
HEADERS = params.hpp
SOURCE = search.cpp parameters.cpp
EXEC = search

ARGS = -lsh -type sift -k 98 -seed 9 -d input.dat

CC =    g++
FLAGS = -Wall -g

all: $(EXEC)

search:search.cpp
	$(CC) -o search $(SOURCE)

valgrind:$(EXEC)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXEC) $(ARGS)

run:$(EXEC)
	./$(EXEC) $(ARGS)
clean:
	rm -f $(OBJS) $(EXEC)