#ifndef __STATE_H
#define __STATE_H

#include <list>
#include <stack>
#include <vector>
#include <cstddef>
#include <tr1/unordered_set>
#include "sokoban.h"

class Board       ;
class State       ;
class StateVisitor;

typedef Board* const Board_;

typedef const State   _State ;
typedef _State* const _State_;
typedef State* const   State_;

typedef std::vector<State*>  StateArray;
typedef std::list  <State*>  StateList ;
typedef std::stack<State*>   StateStack;
typedef const StateArray    _StateArray;
typedef const StateList     _StateList ;
typedef const StateStack    _StateStack;

#define EachStateIn(_i, _list)      (StateList ::const_iterator _i = _list.begin(); _i != _list.end(); ++_i                  )
#define EachStateDel(_i, _list)     (StateList ::iterator       _i = _list.begin(); _i != _list.end();   _i = _list.erase(_i))
#define EachStateArrayIn(_i, _arr)  (StateArray::iterator       _i = _arr .begin(); _i != _arr .end(); ++_i                  )
#define EachStateArrayDel(_i, _arr) (StateArray::iterator       _i = _arr .begin(); _i != _arr .end();   _i = _arr .erase(_i))

class State {
public:
    _State*    parent  ;
    uint       g       ,
               heapi   ,
               f       ;
    StateArray nb      ;
    StateList  children;

    std::size_t hash;

             State(_State_ parent = NULL, _uint g = 0U);
             State(_State& copy                       );
    virtual ~State(void                               );

    void update_with(_State_ ns);
    State* next_neighbor(Board_ b);
    virtual State* get_neighbor (Board_ b, _uint k)=0;

    virtual State* clone(void) const=0;

    virtual uint        lower_bound( Board_ b , _State_ other)=0;
    virtual StateArray& neighbors  ( Board_ b                )=0;

    virtual void accept(StateVisitor& visitor)=0;

    virtual bool operator==(_State& other) const=0;
    virtual bool operator!=(_State& other) const  ;
};

struct StateEquals {
    bool operator()(_State_ a, _State_ b) const;
};

template<typename HashType>
class StateSet {
private:
    StateSet(void);
public:
    typedef std::tr1::unordered_set<State*, HashType, StateEquals> type;
};

#include "board.h"

#endif
