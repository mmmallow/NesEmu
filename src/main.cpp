#include <iostream>
#include "CPU.h"
#include "tests/InstructionsTest.cpp"

void Test() {
    /*
    aslTest t;
    t.ZeroPage();
    t.Accumulator();
    t.Absolute();
    t.ZeroPageX();
    t.AbsoluteX();*/

    branchTest t;
    t.BCC();
    t.BCS();
    t.BEQ();
}

int main (int argc, char* argv[]) {
    Test();

    /*
    CPU c (1, 0);
    
    while (c.cycle < 5) {
        u8 instruction = c.mem[c.PC];
        auto instruct = c.instructions[instruction];
        auto command = std::get<0>(instruct);
        (c.*command)(std::get<1>(instruct));
    }

    std::cout << (int)c.A << std::endl;
    */

    return 0;
}
