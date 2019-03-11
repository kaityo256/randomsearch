all: a.out
SRC=main.cpp
SRC+=sudoku_lib/grid.cpp
SRC+=smr/rater.cpp
SRC+=smr/gridbase.cpp
OBJ=$(SRC:.cpp=.o)

CC=g++
CPPFLAGS=-O3 -mavx2

a.out: $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $@

.PHONY: clean

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

dep:
	g++  -std=gnu++11 -MM -MG $(INCLUDE) $(SRC) >makefile.depend

clean:
	rm -f a.out $(OBJ)

-include makefile.dep
