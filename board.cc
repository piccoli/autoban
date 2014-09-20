#include <unistd.h>
//#define NDEBUG
#include <cassert>
#include <cstdlib>
#include <limits>
#include <string>
#include <iostream>
#include "board.h"
#include "move_state.h"
#include "astar.h"
//#include "idastar.h"

Board::Board(std::istream& in): start(new PushState), goal(new PushState) {
    //in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    width = 0U;
    std::list<std::string> lines;
    for(std::string l; std::getline(in, l); ) {
        lines.push_back(l);
        if(l.length() > width)
            width = l.length();
    }
    height = lines.size();
    map    = new uchar[size = width * height];

    MoveState_ ms = new MoveState;
    uint i = 0U;
    for(std::list<std::string>::const_iterator it = lines.begin(); i < size; ++it) {
        const std::string& line = *it;
        uint j = 0U;
        for(std::string::const_iterator jt = line.begin(); jt != line.end(); ++i, ++j, ++jt) {
            map[i] = TILE_EMPTY;
            switch(static_cast<uchar>(*jt)) {
                case TILE_GUY  : ms   ->set_guy (i); break;
                case TILE_ROCK : start->add_rock(i); break;
                case TILE_SLOT : goal ->add_rock(i); break;
                case TILE_RSLOT: start->add_rock(i);
                                 goal ->add_rock(i); break;
                case TILE_WALL : map[i] = TILE_WALL; break;
                case TILE_EMPTY:
                default        :                     break;
            }
        }
        // Complete missing spaces.
        for(; j < width; ++j, ++i)
            map[i] = TILE_EMPTY;
    }
    start->children.push_back(ms           );
    goal ->children.push_back(new MoveState);
    // Missing a slot? Then guy must be over a slot.
    if(goal->rocks.size() < start->rocks.size())
        goal->add_rock(ms->guy);
}

Board::~Board() {
    delete[] map  ;
    delete   start;
    delete   goal ;
}

uint Board::tile_distance(_uint a, _uint b) const {
    _uint ai = a / width,
          aj = a % width,
          bi = b / width,
          bj = b % width;
    return   static_cast<uint>(std::abs(static_cast<int>(ai) - static_cast<int>(bi)))
           + static_cast<uint>(std::abs(static_cast<int>(aj) - static_cast<int>(bj)));
}

StateStack Board::get_path(State_ start, State_ goal) {
    StateStack path = AStarSolver<MoveStateSet>(this, start, goal).solve();
    //StateStack path = IDAStarSolver(this, start, goal).solve();
    // We don't need these anymore.
    delete start;
    delete goal ;

    if(path.empty())
        return path;

    assert(path.size() > 1U);
    // We don't need the first node, as we already have it
    // from some previous push state.
    delete path.top();
    path.pop();
    return path;
}

StateStack Board::get_path() {
    return get_path(start, goal);
}

StateStack Board::get_path(_uint tstart, _uint tgoal) {
    return get_path(new MoveState(tstart),
                    new MoveState(tgoal ));
}

uint Board::path_len(_uint tstart, _uint tgoal) {
    const upair key = std::make_pair<uint, uint>(tstart, tgoal);

    if(len_map.find(key) != len_map.end())
        return len_map[key];

    StateStack path = get_path(tstart, tgoal);
    if(path.empty())
        return len_map[key] = UINF;
    _uint s = path.size();
    for(; !path.empty(); path.pop())
        delete path.top();
    return len_map[key] = s;
}

void Board::print(_bool clearscreen, _bool delay) const {
    if(clearscreen) goto00(); // clear_screen();
    for(uint i = 0U; i < size;) {
        for(uint j = 0U; j < width; ++j)
            std::cout << map[i++];
        std::cout << std::endl;
    }
    if(delay) usleep(125000U);
}

void Board::_print_state(State_ s, _bool clearscreen, _bool delay) {
    apply_overlay(s);
    if(s->children.size() == 0U)
        print(clearscreen, delay);
    else for EachStateIn(i, s->children)
        _print_state(*i, clearscreen, delay);
    remove_overlay(s);
}

void Board::print_state(State_ s, _bool clearscreen, _bool delay) {
    _print_state(s, clearscreen, delay);
    if(clearscreen) {
        std::cout << std::endl << s->g << " steps..." << std::endl;

        std::cout << std::endl << "Initial state:" << std::endl;
        apply_overlay(start); assert(start->children.size());
        apply_overlay(start->children.back());
        print(false, false);
        remove_overlay(start);
        remove_overlay(start->children.back());

        std::cout << std::endl << "Final state:"   << std::endl;
        apply_overlay(goal); assert(goal->children.size());
        apply_overlay(goal->children.back());
        print(false, false);
        remove_overlay(goal);
        remove_overlay(goal->children.back());
    }
}

void Board::apply_overlay(State_ s) {
    OverlayApplyVisitor v(*this);
    s->accept(v);
}

void Board::remove_overlay(State_ s) {
    OverlayRemoveVisitor v(*this);
    s->accept(v);
}

OverlayApplyVisitor::OverlayApplyVisitor(_Board& b): b(b) {}
OverlayApplyVisitor::~OverlayApplyVisitor() {}

void OverlayApplyVisitor::visit(MoveState_ ms) {
    _uint g = ms->guy;
    if(g < UINF)
        b.map[g] = TILE_GUY;
}

void OverlayApplyVisitor::visit(PushState_ ms) {
    _TileList& rocks = ms->rocks;
    for EachTileIn(it, rocks)
        b.map[*it] = TILE_ROCK;
}

OverlayRemoveVisitor::OverlayRemoveVisitor(_Board& b): b(b) {}
OverlayRemoveVisitor::~OverlayRemoveVisitor() {}

void OverlayRemoveVisitor::visit(MoveState_ ms) {
    _uint g = ms->guy;
    if(g < UINF)
        b.map[g] = TILE_EMPTY;
}

void OverlayRemoveVisitor::visit(PushState_ ms) {
    _TileList& rocks = ms->rocks;
    for EachTileIn(it, rocks)
        b.map[*it] = TILE_EMPTY;
}
