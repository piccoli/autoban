#include <unistd.h>
#include <iostream>
#include "solver.h"

Solver::Solver(Board_ b, const bool verbose):
    board  (b       ),
    start  (b->start),
    goal   (b->goal ),
    verbose(verbose ) {}

Solver::Solver(Board_ b    ,
               State_ start,
               State_ goal ,
               const bool verbose):
    board  (b      ),
    start  (start  ),
    goal   (goal   ),
    verbose(verbose) {}

Solver::~Solver() {}

// XXX Will clear stack (for now).
void Solver::animate_solution(StateStack& ss) {
    if(ss.empty()) {
        std::cout << std::endl << "No solutions found!" << std::endl;
        usleep(2000000U);
        return;
    }
    std::cout << std::endl << "Done! Animating solution..." << std::endl;
    usleep(2000000U);
    for(; !ss.empty(); ss.pop()) {
        State_ s = ss.top();
        board->print_state(s, true, true);
        delete s;
    }
}
