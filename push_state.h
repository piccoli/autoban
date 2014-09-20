#ifndef __PUSH_STATE_H
#define __PUSH_STATE_H

#include "tile.h"

class PushState;

typedef const PushState   _PushState ;
typedef _PushState* const _PushState_;
typedef PushState* const   PushState_;

#include "state.h"
#include "move_state.h"
#include "board.h"

class PushState: public State {
private:
    TileSet rock_set; // listen to your heart ♫ ♪

    State* new_neighbor(Board_ b, _uint p, _int d);
public:
    TileList rocks;

     PushState(_State_ parent = NULL, _uint g = 0U);
     PushState(_PushState& copy                   );
    ~PushState(void                               );

    State* clone(void) const;

    State* get_neighbor(Board_ b, _uint k);

    _MoveState* guy_state  (void                    ) const;
    uint        lower_bound( Board_ b, _State_ other)      ;
    void        add_rock   (_uint   t               )      ;
    StateArray& neighbors  ( Board_ b               )      ;

    void accept(StateVisitor& visitor);

    bool operator==(_State& other) const;

    friend struct PushStateHash;
};

struct PushStateHash {
    std::size_t operator()(State_ key) const;
};

typedef StateSet<PushStateHash>::type PushStateSet;

#endif
