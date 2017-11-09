
#include <iostream>
#include <fstream>

#include "VFifo_test.h"
#include "VFifo_test_Fifo_test.h"
#include "verilated.h"

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    VFifo_test* top = new VFifo_test;

    printf("C, state N, F E R W DDDDDDDD QQQQQQQQ size\n");

    while (!Verilated::gotFinish()) {
        top->eval();

        if (top->clock) {
            printf("%d, state %d, %d %d %d %d %08llx %08llx %d\n",
                    (int) top->clock,
                    (int) top->Fifo_test->state,
                    (int) top->Fifo_test->fifo_full,
                    (int) top->Fifo_test->fifo_empty,
                    (int) top->Fifo_test->fifo_rdreq,
                    (int) top->Fifo_test->fifo_wrreq,
                    (uint64_t) top->Fifo_test->fifo_data,
                    (uint64_t) top->Fifo_test->fifo_q,
                    (int) top->Fifo_test->fifo_usedw);
        }

        // Toggle clock.
        top->clock = top->clock ^ 1;
    }

    delete top;

    exit(0);
}

