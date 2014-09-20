#ifndef __SMASTAR_H
#define __SMASTAR_H

#include "solver.h"

template<typename StateSet>
class SMAStarSolver: public Solver {
private:
    StateSet open  ,
             closed;
public:
    SMAStarSolver (Board_ b     , const bool verbose = false);
    SMAStarSolver (Board_ b     ,
                   State_ start ,
                   State_ goal  , const bool verbose = false);
    ~SMAStarSolver(void        );

    StateStack solve(void);
};

#endif
