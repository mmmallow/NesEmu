#include <iostream>
#include "CPU.h"

int main (int argc, char* argv[]) {
    CPU c;
    
    while (c.cycle < 2) {
        u8 instruction = c.mem[c.PC];
        auto instruct = c.instructions[instruction];
        auto command = std::get<0>(instruct);
        (c.*command)(std::get<1>(instruct));
    }

    std::cout << (int)c.A << std::endl;

    return 0;
}
