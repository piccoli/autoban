//#define NDEBUG
#include <cassert>
#include "move_state.h"

MoveState::MoveState(_uint pusher, _State_ parent, _uint g):
    State (parent, g),
    pusher(pusher   )  {
    nb = StateArray(4U, NULL);
}

MoveState::~MoveState() {}

MoveState::MoveState(_MoveState& copy):
    State (copy       ),
    pusher(copy.pusher)  {
    nb = StateArray(4U, NULL);
}

State* MoveState::clone() const {
    return new MoveState(*this);
}

void MoveState::accept(StateVisitor& visitor) {
    visitor.visit(this);
}

uint MoveState::lower_bound(Board_ b, _State_ other) {
    if(*this == *other)
        return 0U;
    _MoveState_ ms = TYPECAST<_MoveState_>(other);
    assert(ms != NULL);
    return b->tile_distance(pusher, ms->pusher);
}

void MoveState::set_pusher(_uint p) {
    pusher = p;
}

#define IsEmpty(_p)          b->map[_p] == TILE_EMPTY
#define SetState(_index, _p) nb[_index] = new MoveState(_p, this, g + 1U)
StateArray& MoveState::neighbors(Board_ b) {
    _uint w = b->width  ,
          h = b->height ,
          i = pusher / w,
          j = pusher % w;
    if(nb[0] == NULL && i      > 0U && IsEmpty(pusher - w )) SetState(0, pusher - w );
    if(nb[1] == NULL && i + 1U < h  && IsEmpty(pusher + w )) SetState(1, pusher + w );
    if(nb[2] == NULL && j      > 0U && IsEmpty(pusher - 1U)) SetState(2, pusher - 1U);
    if(nb[3] == NULL && j + 1U < w  && IsEmpty(pusher + 1U)) SetState(3, pusher + 1U);
    return nb;
}

State* MoveState::get_neighbor(Board_ b, _uint k) {
    _uint w = b->width  ,
          h = b->height ,
          i = pusher / w,
          j = pusher % w;
    switch(k) {
        case 0: return nb[0] == NULL && i > 0U     && IsEmpty(pusher - w ) ? SetState(0, pusher - w ) : nb[0];
        case 1: return nb[1] == NULL && i + 1U < h && IsEmpty(pusher + w ) ? SetState(1, pusher + w ) : nb[1];
        case 2: return nb[2] == NULL && j > 0U     && IsEmpty(pusher - 1U) ? SetState(2, pusher - 1U) : nb[2];
        case 3: return nb[3] == NULL && j + 1U < w && IsEmpty(pusher + 1U) ? SetState(3, pusher + 1U) : nb[3];
    }
    return NULL;
}
#undef SetState
#undef IsEmpty

bool MoveState::operator==(_State& other) const {
    if(this == &other)
        return true;
    _MoveState_ ms = TYPECAST<_MoveState_>(&other);
    assert(ms != NULL);
    return !(pusher < UINF && ms->pusher < UINF && pusher != ms->pusher);
}

std::size_t MoveStateHash::operator()(State_ key) const {
    if(key->hash > 0U) return key->hash;
    _MoveState_ ms = TYPECAST<_MoveState_>(key);
    assert(ms != NULL);
    return key->hash = 1U + ms->pusher;
}
