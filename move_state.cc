//#define NDEBUG
#include <cassert>
#include "move_state.h"

MoveState::MoveState(_uint guy, _State_ parent, _uint g):
    State(parent, g),
    guy  (guy      )  {
    nb = StateArray(4U, NULL);
}

MoveState::~MoveState() {}

MoveState::MoveState(_MoveState& copy):
    State(copy    ),
    guy  (copy.guy)  {
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
#ifdef NDEBUG
    _MoveState_ ms = static_cast<_MoveState_>(other);
#else
    _MoveState_ ms = dynamic_cast<_MoveState_>(other); assert(ms != NULL);
#endif
    return b->tile_distance(guy, ms->guy);
}

void MoveState::set_guy(_uint p) {
    guy = p;
}

#define IsEmpty(_p)          b->map[_p] == TILE_EMPTY
#define SetState(_index, _p) nb[_index] = new MoveState(_p, this, g + 1U)
StateArray& MoveState::neighbors(Board_ b) {
    _uint w = b->width ,
          h = b->height,
          i = guy / w,
          j = guy % w;
    if(nb[0] == NULL && i      > 0U && IsEmpty(guy - w )) SetState(0, guy - w );
    if(nb[1] == NULL && i + 1U < h  && IsEmpty(guy + w )) SetState(1, guy + w );
    if(nb[2] == NULL && j      > 0U && IsEmpty(guy - 1U)) SetState(2, guy - 1U);
    if(nb[3] == NULL && j + 1U < w  && IsEmpty(guy + 1U)) SetState(3, guy + 1U);
    return nb;
}

State* MoveState::get_neighbor(Board_ b, _uint k) {
    _uint w = b->width ,
          h = b->height,
          i = guy / w,
          j = guy % w;
    switch(k) {
        case 0: return nb[0] == NULL && i > 0U     && IsEmpty(guy - w ) ? SetState(0, guy - w ) : nb[0];
        case 1: return nb[1] == NULL && i + 1U < h && IsEmpty(guy + w ) ? SetState(1, guy + w ) : nb[1];
        case 2: return nb[2] == NULL && j > 0U     && IsEmpty(guy - 1U) ? SetState(2, guy - 1U) : nb[2];
        case 3: return nb[3] == NULL && j + 1U < w && IsEmpty(guy + 1U) ? SetState(3, guy + 1U) : nb[3];
    }
    return NULL;
}
#undef SetState
#undef IsEmpty

bool MoveState::operator==(_State& other) const {
    if(this == &other)
        return true;
#ifdef NDEBUG
    _MoveState_ ms = static_cast<_MoveState_>(&other);
#else
    _MoveState_ ms = dynamic_cast<_MoveState_>(&other); assert(ms != NULL);
#endif
    return !(guy < UINF && ms->guy < UINF && guy != ms->guy);
}

std::size_t MoveStateHash::operator()(State_ key) const {
    if(key->hash > 0U) return key->hash;
#ifdef NDEBUG
    _MoveState_ ms = static_cast<_MoveState_>(key);
#else
    _MoveState_ ms = dynamic_cast<_MoveState_>(key); assert(ms != NULL);
#endif
    return key->hash = 1U + ms->guy;
}
