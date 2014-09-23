#ifndef __ASTAR_H
#define __ASTAR_H

#include "solver.h"

template<typename StateSet>
class AStarSolver: public Solver {
private:
    StateSet open  ,
             closed;
public:
    AStarSolver (Board_ b     , _bool verbose = false);
    AStarSolver (Board_ b     ,
                 State_ start ,
                 State_ goal  , _bool verbose = false);
    ~AStarSolver(void        );

    StateStack solve(void);
};

#endif
