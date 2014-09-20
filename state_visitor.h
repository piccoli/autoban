#ifndef __STATE_VISITOR_H
#define __STATE_VISITOR_H

#include "state.h"
#include "move_state.h"
#include "push_state.h"

class StateVisitor {
public:
    virtual void visit(MoveState_ ms)=0;
    virtual void visit(PushState_ ms)=0;
};

#endif
