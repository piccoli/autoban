#ifndef __IDASTAR_H
#define __IDASTAR_H

#include "solver.h"

class IDAStarSolver: public Solver {
private:
    uint        k ;
    StateStack* ss;

    uint search(State_ s, _uint bound);
public:
    IDAStarSolver(Board_ b    , const bool verbose = false);
    IDAStarSolver(Board_ b    ,
                  State_ start,
                  State_ goal , const bool verbose = false);
    ~IDAStarSolver(void);

    StateStack solve(void);
};

#endif
