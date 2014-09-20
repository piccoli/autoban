#! /usr/bin/gawk -f

BEGINFILE {
    File [++N] = FILENAME
    Count[  N] = 0
}

{ Count[N] += length(gensub(/[^\$]/, "", "g")) }

function swap(i, j,    tmp) {
    tmp      = File[i]
    File[i]  = File[j]
    File[j]  = tmp
    tmp      = Count[i]
    Count[i] = Count[j]
    Count[j] = tmp
}

function qsort(l, u,    p, i, j, t) {
    if(l >= u) return
    i = l + int(.5 + rand() * (u - l))
    p = Count[i]
    swap(i, l)
    i = l
    for(j = u + 1;; swap(i, j)) {
        do ++i; while(i <= u && Count[i] < p)
        do --j; while(          Count[j] > p)
        if(i > j) break
    }
    swap(j, l)
    qsort(l, j - 1)
    qsort(j + 1, u)
}

END {
    qsort(1, N)
    for(i = 1; i <= N; ++i)
        print File[i]
}
