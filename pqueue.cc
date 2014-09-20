#include "pqueue.h"

PQueue::PQueue(_uint size):
    size(size) {}

PQueue::~PQueue() {}

State* PQueue::top() const {
    return heap[0U];
}

void PQueue::insert(State_ s, _uint h) {
    s->heapi = size++;
    s->f = s->g + h;
    //heap.reserve(size);
    heap.push_back(s);
    decrease_key(s);
}

void PQueue::decrease_key(State_ s) {
    uint n = s->heapi,
         p           ;
    // Push deeper states to the front of the queue with <= instead of <.
    for(; n && s->f <= heap[p = (n - 1U) >> 1U]->f; n = p) {
        (heap[n] = heap[p])->heapi = n;
        heap[p] = s;
    }
    s->heapi = n;
}

void PQueue::extract_min() {
    _uint   n   = --size  ;
     State_ tmp = heap[0U]
                = heap[n ];
     uint   i   = 0U      ,
            c             ;
    heap.pop_back();
    for(; (c = (i << 1U) + 1U) < n; i = c) {
        if(c + 1U < n && heap[c + 1U]->f < heap[c]->f)
            ++c;
        if(heap[i]->f <= heap[c]->f)
            break;
        (heap[i] = heap[c])->heapi = i;
        heap[c] = tmp;
    }
    tmp->heapi = i;
}
