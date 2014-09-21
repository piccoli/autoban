#include <cstdlib>
#include <limits>
#include "hungarian_solver.h"

// For a cleaner and "functional" style.
#define Each(_i)    (uint _i = 0; _i < N; ++_i)
#define EachArc(_i) (ArcSet::const_iterator _i = A.begin(); _i != A.end(); ++_i)
#define Blank       N
#define Cost(i, j)  C[(i) * N + (j)]
#define XMate(v)    X[v].mate
#define Alpha(v)    X[v].alpha
#define Exposed(v)  X[v].exposed
#define Label(v)    X[v].label
#define Count(v)    X[v].count
#define YMate(u)    Y[u].mate
#define Beta(u)     Y[u].beta
#define Slack(u)    Y[u].slack
#define Nhbor(u)    Y[u].nhbor

using namespace std;

HungarianSolver::HungarianSolver(Board_ b,
                                 _TileList& arocks,
                                 _TileList& brocks):
    N(arocks.size()) {
    // The bipartite graph (or cost matrix).
    C = new  int [N * N];
    X = new XData[N    ];
    Y = new YData[N    ];

    TileList::const_iterator it = arocks.begin();
    for(uint i = 0U; i < N; ++i, ++it) {
        TileList::const_iterator jt = brocks.begin();
        for(uint j = 0U; j < N; ++j, ++jt)
            // XXX Multiply by 2 to Ensure integral values during computation
            // (theta1 is 1/2 * min[j=0..n-1]{ slack(j) = c[i][j] - alpha[i] -
            // beta[j] for some i }).
            Cost(i, j) = static_cast<int>(b->tile_distance(*it, *jt) << 1U);
    }
}

HungarianSolver::HungarianSolver(_uint c[], _uint n):
    N(n),
    // The bipartite graph (or cost matrix).
    C(new  int [n * n]),
    X(new XData[n    ]),
    Y(new YData[n    ]) {
    uint k = 0U;
    for(uint i = 0U; i < N; ++i)
        for(uint j = 0U; j < N; ++j)
            // XXX Multiply by 2 to Ensure integral values during computation
            // (theta1 is 1/2 * min[j=0..n-1]{ slack(j) = c[i][j] - alpha[i] -
            // beta[j] for some i }).
            Cost(i, j) = static_cast<int>(c[k++] << 1U);
}

HungarianSolver::~HungarianSolver() {
    delete[] C;
    delete[] X;
    delete[] Y;
}

uint HungarianSolver::min_weight_perfect_match() {
    init();
    // For each stage s = 1...N, try to augment the admissible graph or the number
    // of admissible edges.
    for Each(s) {
        build_auxiliary_graph();
        if(collect())
            while(search() && modify())
                ;
    }
    // Compute and return the minimal matching cost.
    uint cost = 0U;
    for Each(i)
        cost += static_cast<uint>(Cost(i, XMate(i)));
    // XXX Costs were multiplied by 2 for avoiding loss of precision, as we did
    // all the work with integers. So we divide by two back here.
    return cost >> 1U;
}

void HungarianSolver::init() {
    for Each(j) {
        XMate(j) =
        YMate(j) = Blank;
        Alpha(j) = 0    ;
        Beta(j)  = std::numeric_limits<int>::max();
        // Beta[j] is initialized to min[i = 0..N-1] { Cost(i, j) }.
        for Each(i)
            if(Cost(i, j) < Beta(j))
                Beta(j) = Cost(i, j);
    }
}

void HungarianSolver::build_auxiliary_graph() {
    A.clear();
    for Each(i) {
        Exposed(i) =
        Label(i)   =
        Nhbor(i)   = Blank;
        Count(i)   = 0U   ;
        Slack(i)   = std::numeric_limits<int>::max();
    }
    for Each(i)
        for Each(j)
            if(Cost(i, j) == Alpha(i) + Beta(j)) {
                if(YMate(j) == Blank)
                    Exposed(i) = j;
                else if(i != YMate(j))
                    A.push_back(make_pair(i, YMate(j)));
            }
}

bool HungarianSolver::collect() {
    Q.clear();
    for Each(i)
        if(XMate(i) == Blank) {
            if(Exposed(i) != Blank) {
                augment(i);
                return false;
            }
            Q.push_back(i);
            Label(i) = Blank;
            reduce_slack(i);
        }
    return true;
}

bool HungarianSolver::search() {
    while(!Q.empty()) {
        _uint i = Q.back();
        Q.pop_back();
        for EachArc(ak) {
            const pair<uint, uint>& p = *ak;
            if(p.first == i) {
                _uint j = p.second;
                if(Label(j) == Blank) {
                    Label(j) = i;
                    if(Exposed(j) != Blank) {
                        augment(j);
                        return false;
                    }
                    Q.push_back(j);
                    reduce_slack(j);
                }
            }
        }
    }
    return true;
}

bool HungarianSolver::modify() {
    int theta1 = std::numeric_limits<int>::max();
    for Each(j)
        if(0 < Slack(j) && Slack(j) < theta1)
            theta1 = Slack(j);
    theta1 >>= 1;
    for Each(i)
        if(Label(i) != Blank || Count(i) > 0U)
            Alpha(i) += theta1;
        else
            Alpha(i) -= theta1;
    for Each(j)
        if(Slack(j) == 0)
            Beta(j) -= theta1;
        else
            Beta(j) += theta1;
    for Each(j)
        if(Slack(j) > 0 && (Slack(j) -= theta1 << 1) == 0) {
            if(YMate(j) == Blank) {
                Exposed(Nhbor(j)) = j;
                augment(Nhbor(j));
                return false;
            }
            else {
                Q.push_back(Nhbor(j));
                A.push_back(make_pair(Nhbor(j), YMate(j)));
            }
        }
    return true;
}

void HungarianSolver::reduce_slack(_uint& i) {
    for Each(k) {
        _int ck = Cost(i, k) - Alpha(i) - Beta(k);
        if(0 <= ck && ck < Slack(k)) {
            Slack(k) = ck;
            if(Nhbor(k) != Blank)
                --Count(Nhbor(k));
            ++Count(i);
            Nhbor(k) = i;
        }
    }
}

void HungarianSolver::augment(uint i) {
    for(; Label(i) != Blank; i = Label(i)) {
        Exposed(Label(i)) = XMate(i);
        XMate(i)          = Exposed(i);
        YMate(Exposed(i)) = i;
    }
    XMate(i)          = Exposed(i);
    YMate(Exposed(i)) = i         ;
}
