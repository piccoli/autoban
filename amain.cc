#include <csignal>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include "astar.h"
#include "push_state.h"

typedef AStarSolver<PushStateSet> SolverType;

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

    char * board_fn = NULL;

    for (int arg = 1; arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--input"))
        {
            arg++;
            if (arg == argc)
            {
                std::cerr << "Option without an argument." << std::endl;
                return -1;
            }
            else
            {
                board_fn = argv[arg];
            }
        }
        else
        {
            std::cerr << "Unknown option" << argv[arg] << "!" << std::endl;
            std::cerr << "Known options are: \"--input\"." << std::endl;
            return -1;
        }
    }

    Board * bp;
    if (board_fn)
    {
        std::ifstream fp((const char *)board_fn);
        bp = new Board (fp);
    }
    else
    {
        bp = new Board (std::cin);
    }
    SolverType solver(bp, true);

    hide_cursor ();
    clear_screen();

    StateStack ss = solver.solve();
    _int status = ss.empty() ? EXIT_FAILURE : EXIT_SUCCESS;
    solver.animate_solution(ss);

    show_cursor();

    if (bp)
    {
        delete bp;
        bp = NULL;
    }

    return status;
}
