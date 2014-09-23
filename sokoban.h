#ifndef __SOKOBAN_H
#define __SOKOBAN_H

#define UINF 0x3FFFFFFFU

#ifdef NDEBUG
#define TYPECAST static_cast
#else
#define TYPECAST dynamic_cast
#endif

typedef unsigned int  uint ;
typedef unsigned char uchar;

typedef const int   _int  ;
typedef const uint  _uint ;
typedef const uchar _uchar;
typedef const bool  _bool ;

// XXX "Mini-" curses ;)
#include <iostream>
inline void clear_screen(void) { std::cout << "\033[2J"  ; }
inline void hide_cursor (void) { std::cout << "\033[?25l"; }
inline void show_cursor (void) { std::cout << "\033[?25h"; }
inline void goto00      (void) { std::cout << "\033[H"   ; }
//inline void goto00      (void) { std::cout << "\033[0d\0330G"; }

#endif
