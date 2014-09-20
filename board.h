#ifndef __BOARD_H
#define __BOARD_H

#include <map>
#include <stack>
#include <istream>
#include <iostream>
#include <utility>
#include "tile.h"

class State;
class Board;

typedef const Board   _Board ;
typedef _Board* const _Board_;
typedef Board* const   Board_;

#include "state.h"
#include "move_state.h"
#include "push_state.h"
#include "state_visitor.h"

typedef std::pair<uint , uint> upair    ;
typedef std::map <upair, uint> LengthMap;

class OverlayApplyVisitor: public StateVisitor {
private:
    _Board& b;
public:
    OverlayApplyVisitor(_Board& b);
    ~OverlayApplyVisitor(void);
    void visit(MoveState_ ms);
    void visit(PushState_ ms);
};

class OverlayRemoveVisitor: public StateVisitor {
private:
    _Board& b;
public:
    OverlayRemoveVisitor(_Board& b);
    ~OverlayRemoveVisitor(void);
    void visit(MoveState_ ms);
    void visit(PushState_ ms);
};

class Board {
private:
    LengthMap len_map;
    void _print_state(State_ s, _bool clearscreen = false, _bool delay = false);
public:
    uint       width ,
               height,
               size  ;
    PushState_ start ,
               goal  ;
    uchar*     map   ;

     Board(std::istream& in);
    ~Board(void            );

    uint tile_distance (_uint a, _uint b) const;
    void print         (          _bool clearscreen = false, _bool delay = false) const;
    void print_state   (State_ s, _bool clearscreen = false, _bool delay = false)      ;
    void apply_overlay (State_ s                                                )      ;
    void remove_overlay(State_ s                                                )      ;

    StateStack get_path(void                       );
    StateStack get_path(State_  start, State_  goal);
    StateStack get_path(_uint tstart, _uint tgoal);
    uint       path_len(_uint tstart, _uint tgoal);

    friend class OverlayApplyVisitor ;
    friend class OverlayRemoveVisitor;
};

#endif
