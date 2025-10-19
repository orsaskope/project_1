OBJS = search.o parameters.o LSH.o IVFFLAT.o
HEADERS = params.hpp LSH.h ivfflat.hpp
SOURCE = search.cpp parameters.cpp LSH.cpp IVFFLAT.cpp
EXEC = search

ARGS = -ivfflat -type mnist -k 6   -seed 9 -d input.dat

CC =    g++
FLAGS = -Wall -g

all: $(EXEC)

$(EXEC): $(SOURCE)
	$(CC) $(FLAGS) -o $(EXEC) $(SOURCE)

valgrind: $(EXEC)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXEC) $(ARGS)

run: $(EXEC)
	./$(EXEC) $(ARGS)

clean:
	rm -f $(OBJS) $(EXEC)
