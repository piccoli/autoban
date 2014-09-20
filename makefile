DEFS=-DNDEBUG
#DEFS=
#CXXFLAGS=-std=c++98 -Wall -pedantic -march=native -O3 -pipe -fomit-frame-pointer $(DEFS)
CXXFLAGS=-std=c++98 -Wall -pedantic -march=core2 -msse4.1 -O3 -pipe -fomit-frame-pointer $(DEFS)
#CXXFLAGS=-O0 -ggdb -std=c++98 -Wall -pedantic $(DEFS)
CXX=g++
OBJ=board.o hungarian_solver.o state.o move_state.o push_state.o solver.o pqueue.o astar.o idastar.o
AOBJ=amain.o
IDAOBJ=idamain.o

all: apusher idapusher

apusher: $(OBJ) $(AOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(AOBJ)

idapusher: $(OBJ) $(IDAOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(IDAOBJ)

clean:
	rm -f $(OBJ) $(AOBJ) $(IDAOBJ) apusher idapusher
