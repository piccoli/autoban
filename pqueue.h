#ifndef __PQUEUE_H
#define __PQUEUE_H

#include <vector>
#include "sokoban.h"
#include "state.h"

class PQueue {
private:
    std::vector<State*> heap;
public:
    uint size;

     PQueue(_uint size);
    ~PQueue(void      );

    State* top         (void             ) const;
    void   insert      (State_ s, _uint h)      ;
    void   decrease_key(State_ s         )      ;
    void   extract_min (void             )      ;
};

#endif
