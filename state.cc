#include "state.h"

State::State(_State_ parent, _uint g):
    parent(parent),
    g     (g     ),
    heapi (0U    ),
    f     (UINF  ),
    hash  (0U    ) {}

State::State(_State& copy):
    parent(NULL      ),
    g     (copy.g    ),
    heapi (copy.heapi),
    f     (copy.f    ),
    hash  (copy.hash ) {
        for EachStateIn(i, copy.children)
            children.push_back((*i)->clone());
}

State::~State() {
    for EachStateArrayDel(i, nb      ) delete *i;
    for EachStateDel     (i, children) delete *i;
}

State* State::next_neighbor(Board_ b) {
    _uint n = nb.size();
    for(uint i = 0U; i < n; ++i)
        if(nb[i] == NULL)
            return get_neighbor(b, i);
    return NULL;
}

void State::update_with(_State_ ns) {
    parent = ns->parent   ;
    f      = f + ns->g - g;
    g      = ns->g        ;
    for EachStateDel(i, children)
        delete *i;
    for EachStateIn(i, ns->children)
        children.push_back((*i)->clone());
}

bool State::operator!=(_State& other) const {
    return !(*this == other);
}

bool StateEquals::operator()(_State_ a, _State_ b) const {
    return *a == *b;
}
