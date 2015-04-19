#ifndef __TILE_H
#define __TILE_H

//#include <set>
#include <tr1/unordered_set>
#include <list>

#define TILE_PUSHER '@'
#define TILE_ROCK   '$'
#define TILE_SLOT   '.'
#define TILE_RSLOT  '*'
#define TILE_WALL   '#'
#define TILE_EMPTY  ' '
#define TILE_PUSHER_AND_SLOT  '+'

/*
enum {
    TILE_PUSHER = '@',
    TILE_ROCK   = '$',
    TILE_SLOT   = '.',
    TILE_RSLOT  = '*',
    TILE_WALL   = '#',
    TILE_EMPTY  = ' '
};
*/

#include "sokoban.h"

typedef std::list<uint>   TileList ;
typedef const TileList   _TileList ;
typedef _TileList* const _TileList_;
typedef TileList* const   TileList_;

//typedef std::set<uint> TileSet;
typedef std::tr1::unordered_set<uint> TileSet;

#define EachTileIn(_i, _l)  (TileList::const_iterator _i = (_l).begin(); _i != (_l).end(); ++_i                 )
#define EachTileDel(_i, _l) (TileList::iterator       _i = (_l).begin(); _i != (_l).end();   _i = (_l).erase(_i))

#endif
