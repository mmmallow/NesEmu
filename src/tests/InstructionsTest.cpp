#include <iostream>
#include <cassert>
#include "../CPU.h"

class ldaTest {
public:
void IndirectX() {
    CPU c(1, 0);

    // lda ($80, X)
    c.X = 3;
    c.mem[1] = 0xA1;
    c.mem[2] = 0x80;

    // 0x80+X & 0x80+X+1 store address 0x4532
    c.mem[0x83] = 0x32;
    c.mem[0x84] = 0x45;

    // Store 27 at 0x4532
    c.mem[0x4532] = 27;

    while (c.cycle < 6) {
        u8 instruction = c.mem[c.PC];
        auto instruct = c.instructions[instruction];
        auto command = std::get<0>(instruct);
        (c.*command)(std::get<1>(instruct));
    }

    assert(c.A == 27);
    std::cout << "ldaTest: Passed" << std::endl;
}

};
