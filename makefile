OBJS = search.o parameters.o LSH.o IVFFLAT.o ivfpq.o
HEADERS = params.hpp LSH.h ivfflat.hpp ivfpq.hpp
SOURCE = search.cpp parameters.cpp LSH.cpp IVFFLAT.cpp IVFPQ.cpp
EXEC = search

ARGS = -ivfflat -type mnist -k 50  -seed 9 -d input.dat -kclusters 4 -range true

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
	rm -f $(OBJS) $(EXEC) output.txt
