//#define NDEBUG
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "push_state.h"
#include "move_state.h"
#include "hungarian_solver.h"
#include "astar.h"
//#include "idastar.h"

PushState::PushState(_State_ parent, _uint g):
    State(parent, g) {}

PushState::~PushState() {}

PushState::PushState(_PushState& copy):
    State(copy) {
    for EachTileIn(i, copy.rocks)
        add_rock(*i);
}

State* PushState::clone() const {
    return new PushState(*this);
}

State* PushState::get_neighbor(Board_ b, _uint k) {
    return NULL;
}

void PushState::accept(StateVisitor& visitor) {
    visitor.visit(this);
}

uint PushState::lower_bound(Board_ b, _State_ other) {
    if(*this == *other)
        return 0U;
    b->apply_overlay(this);

    // Check for deadlocks.
#ifdef NDEBUG
    _PushState_ ms = static_cast<_PushState_>(other);
#else
    _PushState_ ms = dynamic_cast<_PushState_>(other); assert(ms != NULL);
#endif
    _uint w = b->width ,
          h = b->height;
    for EachTileIn(it, rocks) {
        _uint t = *it  ,
              i = t / w,
              j = t % w;
        // Does this rock correspond to a slot in the destination state? If so,
        // we're ok, no need to check.
        if(ms->rock_set.find(t) != ms->rock_set.end())
            continue;
        _bool ublock = i      == 0U || b->map[t - w ] == TILE_WALL,
              dblock = i + 1U == h  || b->map[t + w ] == TILE_WALL,
              lblock = j      == 0U || b->map[t - 1U] == TILE_WALL,
              rblock = j + 1U == w  || b->map[t + 1U] == TILE_WALL;
        if(   (ublock && rblock)
           || (rblock && dblock)
           || (dblock && lblock)
           || (lblock && ublock)) {
            b->remove_overlay(this);
            return UINF;
        }
    }
    //b->remove_overlay(this);
    //_uint hs = HungarianSolver(b, rocks, ms->rocks).min_weight_perfect_match();

    /**/
     uint        k = 0U;
    _uint        n = rocks.size();
     uint* const c = new uint[n * n];
    for EachTileIn(it, rocks)
        for EachTileIn(jt, ms->rocks)
            c[k++] = b->path_len(*it, *jt);
    b->remove_overlay(this);
    _uint hs = HungarianSolver(c, n).min_weight_perfect_match();
    delete[] c;
    /**/
    return hs;
    //return hs * hs;
}

void PushState::add_rock(_uint t) {
    rocks.push_back(t);
    rock_set.insert(t);
}

_MoveState* PushState::guy_state() const {
    assert(children.size());

#ifdef NDEBUG
    _MoveState* mme = static_cast<_MoveState*>(children.back());
#else
    _MoveState* mme = dynamic_cast<_MoveState*>(children.back()); assert(mme != NULL);
#endif
    return mme;
}

State* PushState::new_neighbor(Board_ b, _uint p, _int d) {
    _uint gp = static_cast<_uint>(static_cast<_int>(p) + d),
          np = static_cast<_uint>(static_cast<_int>(p) - d);

    // Guy must be able to get to position gp and move the rock in p to np.
    if(b->map[gp] != TILE_EMPTY || b->map[np] != TILE_EMPTY)
        return NULL;

    // Check if current guy is already next to a rock.  In this case, we
    // actually move the rock instead of constructing a path.
    if(guy_state()->guy == gp) {
        PushState_ s = new PushState(this, g + 1U);
        // Copy rocks at their current location,
        // except for the rock at p, which goes to np instead.
        for EachTileIn(it, rocks)
            s->add_rock(*it == p ? np : *it);
        // Create move state for guy from gp to p.
        s->children.push_back(new MoveState(p));
        return s;
    }
    // Otherwise, the neighboring state is a path traveled by the guy
    // until he is next to rock (i, j), at (gi, gj). So we compute this
    // path using a "sub-"A* pathfinder.
    StateStack path = b->get_path(guy_state()->clone(),   // We begin at where we are right now.
                                  new MoveState(gp)); // And end at gp.

    // Give up if there is no path from the current guy position to gp.
    if(path.empty())
        return NULL;

    // Otherwise, create a new neighbor and copy the computed path into it.
    // (Except the first node, as it is identical to the last node of this state.)
    PushState_ s = new PushState(this, g + path.size());
    for(; !path.empty(); path.pop())
        s->children.push_back(path.top());
    // Copy rocks from this state.
    for EachTileIn(it, rocks)
        s->add_rock(*it);
    return s;
}

StateArray& PushState::neighbors(Board_ b) {
    if(nb.size() == 0U) {
        _uint n = 4U * rocks.size();
        nb.reserve(n);
        for(uint i = 0U; i < n; ++i)
            nb.push_back(NULL);
    }
    b->apply_overlay(this);
    // For each rock, add a neighbor for each of its surroundings cells.
    _uint w = b->width ,
          h = b->height;
    uint  k = 0U       ;
    for EachTileIn(it, rocks) {
        _uint r = *it  ,
              i = r / w,
              j = r % w;
        if(i > 0U && i + 1U < h) {
            if(nb[k + 0U] == NULL) nb[k + 0U] = new_neighbor(b, r, -w);
            if(nb[k + 1U] == NULL) nb[k + 1U] = new_neighbor(b, r,  w);
        }
        if(j > 0U && j + 1U < w) {
            if(nb[k + 2U] == NULL) nb[k + 2U] = new_neighbor(b, r, -1);
            if(nb[k + 3U] == NULL) nb[k + 3U] = new_neighbor(b, r,  1);
        }
        k += 4U;
    }
    b->remove_overlay(this);
    return nb;
}

bool PushState::operator==(_State& other) const {
    if(this == &other)
        return true;
#ifdef NDEBUG
    _PushState_ ms = static_cast<_PushState_>(&other);
#else
    _PushState_ ms = dynamic_cast<_PushState_>(&other); assert(ms != NULL);
#endif
    if(*(guy_state()) != *(ms->guy_state()))
        return false;
    // TODO starting guy position?
    for EachTileIn(i, rocks)
        if(ms->rock_set.find(*i) == ms->rock_set.end())
            return false;
    return true;
}

#define BUFSIZE 32
static uchar buf[BUFSIZE];

static _uchar bsym[] = {
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0xa1,
    0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xae, 0xaf, 0xb0, 0xb1, 0xb2,
    0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, 0xc2,
    0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe0, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf0, 0xf1, 0xf2,
    0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};
static const std::size_t blen = sizeof(bsym) / sizeof(*bsym);

static inline _uchar* base189(uint x) {
    uchar* p = buf;
    *p++ = ' ';
    do *p++ = bsym[x % blen]; while(x /= blen);
    *p++ = '\0';
    return buf;
}
#undef BUFSIZE

//#define SEED 2166136261U
//#define SHFT 16777619U
#define SEED 0U
#define SHFT 31U
std::size_t PushStateHash::operator()(State_ key) const {
    if(key->hash > 0U) return key->hash;

#ifdef NDEBUG
    _PushState_ ms = static_cast<_PushState_>(key);
#else
    _PushState_ ms = dynamic_cast<_PushState_>(key); assert(ms != NULL);
#endif
    std::size_t hh = SEED;
    _uchar* p = base189(ms->guy_state()->guy);
    do hh = (hh * SHFT) + static_cast<uint>(*p); while(*++p);

    for EachTileIn(i, ms->rocks) {
        p = base189(*i);
        do hh = (hh * SHFT) + static_cast<uint>(*p); while(*++p);
    }
    return key->hash = hh;
}
#undef SEED
#undef SHFT
