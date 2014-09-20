#ifndef __SOLVER_H
#define __SOLVER_H

#include "state.h"
#include "board.h"

class Solver {
protected:
    Board_ board  ;
    State_ start  ,
           goal   ;
    const bool verbose;
public:
    Solver(Board_ b    , const bool verbose = false);
    Solver(Board_ b    ,
           State_ start,
           State_ goal , const bool verbose = false);

    virtual ~Solver(void);

    virtual StateStack solve           (void          )=0;
    virtual void       animate_solution(StateStack& ss)  ;
};

#endif
