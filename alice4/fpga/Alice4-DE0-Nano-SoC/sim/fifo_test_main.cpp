
#include <iostream>
#include <fstream>

#include "wavedrom.h"

#include "VFifo_test.h"
#include "VFifo_test_Fifo_test.h"
#include "verilated.h"

int main(int argc, char **argv, char **env) {
    Verilated::commandArgs(argc, argv);

    VFifo_test* top = new VFifo_test;

    WaveDrom waveDrom;
    waveDrom.add(WaveDromSignal("clock"));
    waveDrom.add(WaveDromSignal("state", true));
    waveDrom.add(WaveDromSignal("full"));
    waveDrom.add(WaveDromSignal("empty"));
    waveDrom.add(WaveDromSignal("rdreq"));
    waveDrom.add(WaveDromSignal("wrreq"));
    waveDrom.add(WaveDromSignal("d"));
    waveDrom.add(WaveDromSignal("q"));
    waveDrom.add(WaveDromSignal("size", true));

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

            waveDrom["clock"].add(top->clock);
            waveDrom["state"].add(top->Fifo_test->state);
            waveDrom["full"].add(top->Fifo_test->fifo_full);
            waveDrom["empty"].add(top->Fifo_test->fifo_empty);
            waveDrom["rdreq"].add(top->Fifo_test->fifo_rdreq);
            waveDrom["wrreq"].add(top->Fifo_test->fifo_wrreq);
            waveDrom["d"].add(to_hex(top->Fifo_test->fifo_data, 1));
            waveDrom["q"].add(to_hex(top->Fifo_test->fifo_q, 1));
            waveDrom["size"].add(top->Fifo_test->fifo_usedw);
        }

        // Toggle clock.
        top->clock = top->clock ^ 1;
    }

    delete top;

    waveDrom.write("out.json");

    exit(0);
}

