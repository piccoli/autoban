#ifndef __MOVE_STATE_H
#define __MOVE_STATE_H

#include "tile.h"

class MoveState;

typedef const MoveState   _MoveState ;
typedef _MoveState* const _MoveState_;
typedef MoveState* const   MoveState_;

#include "state.h"

class MoveState: public State {
public:
    uint pusher;

     MoveState(_uint   pusher = UINF ,
               _State_ parent = NULL ,
               _uint   g      = 0U  );

     MoveState(_MoveState& copy);
    ~MoveState(void            );

    State* clone(void) const;

    State* get_neighbor(Board_ b, _uint k);

    uint        lower_bound( Board_ b , _State_ other);
    void        set_pusher (_uint   p                );
    StateArray& neighbors  ( Board_ b                );

    void accept(StateVisitor& visitor);

    bool operator==(_State& other) const;

    friend struct MoveStateHash;
};

struct MoveStateHash {
    std::size_t operator()(State_ key) const;
};

typedef StateSet<MoveStateHash>::type MoveStateSet;

#endif
