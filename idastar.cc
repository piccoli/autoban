#include <iostream>
#include "sokoban.h"
#include "idastar.h"

IDAStarSolver::IDAStarSolver(Board_ b, _bool verbose):
    Solver(b, verbose),
    ss(NULL) {}

IDAStarSolver::IDAStarSolver(Board_ b    ,
                             State_ start,
                             State_ goal ,
                             _bool verbose):
    Solver(b, start, goal, verbose),
    ss(NULL) {}

IDAStarSolver::~IDAStarSolver() {
    if(ss != NULL)
        delete ss;
}

StateStack IDAStarSolver::solve() {
    if(ss != NULL) delete ss;
    ss = new StateStack;
    k  = 0U            ;
    for(uint t, bound = start->lower_bound(board, goal); ; bound = t)
        if((t = search(start, bound)) == UINF || !ss->empty())
            return *ss;
}

uint IDAStarSolver::search(State_ s, _uint bound) {
    _uint f = s->g + s->lower_bound(board, goal);
    if(verbose && k++ % 1000U == 0U) {
        board->print_state(s, true, false);
        std::cout << std::endl << "Searching..." << std::endl;
    }
    if(f > bound)
        return f;
    if(*s == *goal) {
        goal->update_with(s);
        for(_State* p = goal; p != NULL; p = p->parent)
            ss->push(p->clone());
        return 0U;
    }
    uint        min    = UINF;
    StateArray& nhbors = s->neighbors(board);
    for(StateArray::iterator it = nhbors.begin(); it != nhbors.end(); ++it) {
        State_ nb = *it;
        if(nb == NULL) continue;

        _uint t = search(nb, bound);
        if(!ss->empty()) {
            for(; it != nhbors.end(); ++it) {
                delete *it;
                *it = NULL;
            }
            return 0U;
        }
        if(t < min) min = t;
        delete nb;
        *it = NULL;
    }
    return min;
}
