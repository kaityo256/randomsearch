all: a.out
SRC=main.cpp
SRC+=sudoku_lib/grid.cpp
SRC+=smr/fastgrid.cpp
SRC+=smr/solvergrid.cpp
SRC+=smr/gridbase.cpp
SRC+=smr/rater.cpp
SRC+=smr/uafinder.cpp
SRC+=smr/mask81.cpp
SRC+=smr/uniquenesschecker.cpp
OBJ=$(SRC:.cpp=.o)

CC=mpic++
CPPFLAGS=-O3 -mavx2 -std=c++11

-include makefile.opt

a.out: $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $@

.PHONY: clean

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

dep:
	g++  -std=gnu++11 -MM -MG $(SRC) >makefile.depend

clean:
	rm -f a.out $(OBJ)

-include makefile.dep
