#ifndef __HUNGARIAN_SOLVER_H
#define __HUNGARIAN_SOLVER_H
// Solver for the "Assignment Problem" (find a minimum-weighted perfect
// bipartite matching of two sets), using the primal-dual method,
// also known as the "Hungarian Method" for the assignment problem.
//
// The implementation is based on the book "Combinatorial Optimization:
// Algorithms and Complexity" (p. 251) by Papadimitriou and Steiglitz, and
// 'somewhat inspired' by William Rummler's implementation and fixes (in
// particular, the use of macros for better readability).
//
// For more information: http://www.cs.princeton.edu/~ken/PUBS.html

#include <vector>
#include <utility>
#include "sokoban.h"
#include "tile.h"
#include "board.h"

// Data for the vertices on the X subset of V.
struct XData {
    int  alpha  ;
    uint mate   ,
         exposed,
         label  ,
         count  ;
};

// Data for the vertices on the Y subset of V.
struct YData {
    int  beta ,
         slack;
    uint mate ,
         nhbor;
};

typedef std::vector< std::pair<uint, uint> > ArcSet;

class HungarianSolver {
private:
    uint              N; // Number of vertices in each X/Y set.
    int*              C; // Cost matrix (remember, we are dealing with a complete bipartite graph).
    XData*            X; // The 'X' set of vertices (sokoban rocks).
    YData*            Y; // The 'Y' set (sokoban slots).
    std::vector<uint> Q; // Used for the BFS routine when looking for augmenting paths.
    ArcSet            A; // Set of admissible edges for each iteration.

    // Helper routines.
    void init                 (void    );
    void build_auxiliary_graph(void    );
    bool collect              (void    );
    bool search               (void    );
    bool modify               (void    );
    void reduce_slack         (_uint& i);
    void augment              ( uint  i);

public:
     HungarianSolver(Board_ b, _TileList& arocks, _TileList& brocks);
     HungarianSolver(_uint c[], _uint n);
    ~HungarianSolver(void              );

    uint min_weight_perfect_match(void);
};

#endif
