#include <csignal>
#include <cstdlib>
#include <iostream>
#include "smastar.h"
#include "push_state.h"

typedef SMAStarSolver<PushStateSet> SolverType;

void cleanup(int signum) {
    show_cursor();
    std::cout << "Cleaning up after signal " << signum << "..." << std::endl;
    signal(signum, NULL);
}

int main(int argc, char* argv[]) {
    signal(SIGABRT, cleanup);
    signal(SIGINT , cleanup);
    signal(SIGTERM, cleanup);
    signal(SIGSEGV, cleanup);

    Board      b     (std::cin);
    SolverType solver(&b, true);

    hide_cursor ();
    clear_screen();

    StateStack ss = solver.solve();
    _int status = ss.empty() ? EXIT_FAILURE : EXIT_SUCCESS;
    solver.animate_solution(ss);

    show_cursor();
    return status;
}
