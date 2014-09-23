#include <iostream>
#include "sokoban.h"
#include "pqueue.h"
#include "astar.h"

template<typename StateSet>
AStarSolver<StateSet>::AStarSolver(Board_ b, _bool verbose):
    Solver(b, verbose) {}

template<typename StateSet>
AStarSolver<StateSet>::AStarSolver(Board_ b    ,
                                   State_ start,
                                   State_ goal ,
                                   _bool verbose):
    Solver(b, start, goal, verbose) {}

template<typename StateSet>
AStarSolver<StateSet>::~AStarSolver() {
    //closed.clear();
    //open  .clear();
}

template<typename StateSet>
StateStack AStarSolver<StateSet>::solve() {
    closed.clear();
    open  .clear();

    PQueue q(0U);
    q.insert(start, start->lower_bound(board, goal));

    uint k = 0U;
    while(q.size) {
        State* s = q.top();
        if(*s == *goal) {
            goal->update_with(s);
            StateStack ss;
            for(_State* t = goal; t != NULL; t = t->parent)
                ss.push(t->clone());
            return ss;
        }
        if(verbose && k++ % 1000U == 0U) {
            board->print_state(s, true, false);
            std::cout << std::endl << "Searching..." << std::endl;
        }
        q.extract_min();
        open  .erase (s);
        closed.insert(s);
        StateArray& nhbors = s->neighbors(board);
        for(StateArray::iterator it = nhbors.begin(); it != nhbors.end(); ++it) {
            State_ nb = *it;
            if(nb == NULL) continue;

            if(closed.find(nb) != closed.end()) {
                delete nb;
                *it = NULL;
                continue;
            }
            const typename StateSet::const_iterator oi = open.find(nb);
            if(oi != open.end()) {
                State_ onb = *oi;
                if(nb->g < onb->g) {
                    onb->update_with(nb);
                    q.decrease_key(onb);
                }
                delete nb;
                *it = NULL;
                continue;
            }
            open.insert(nb);
            q.insert(nb, nb->lower_bound(board, goal));
        }
    }
    return StateStack();
}

#include "move_state.h"
#include "push_state.h"
template class AStarSolver<MoveStateSet>;
template class AStarSolver<PushStateSet>;
