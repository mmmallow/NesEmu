#include <iostream>
#include "CPU.h"

int main (int argc, char* argv[]) {
    CPU c;
    
    int num_cycles = 0;
    while (num_cycles < 2) {
        u16 instruction = c.PC;
        // TODO:
        // Make a table copying the instruction set table on the website
        // to make it easier to pull instructions from (not having to use switch or if).
        // Probably just want a separate file.

    }

    return 0;
}
