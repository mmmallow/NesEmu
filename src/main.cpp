#include <iostream>
#include "CPU.h"
#include "tests/InstructionsTest.h"

void Test() {
    staTest t;
    t.IndirectX();
    t.ZeroPage();
    //t.Immediate();
    //t.Accumulator();
    t.Absolute();
    t.IndirectY();
    t.ZeroPageX();
    t.AbsoluteY();
    t.AbsoluteX();

    
    /*branchTest t;
    t.BCC();
    t.BCS();
    t.BEQ();
    t.BMI();
    t.BNE();
    t.BPL();
    t.BVC();
    t.BVS();
    t.realProgram();*/

    /*pushPullTest t;
    t.pha();
    t.php();
    t.pla();
    t.plp();*/

    /*brkTest t;
    t.Implied();*/

    /*jmpTest t;
    //t.ZeroPage();
    t.Absolute();
    t.Indirect();*/
    //t.ZeroPageX();
    //t.AbsoluteX();

    /*inXYTest t2;
    t2.Inx();
    t2.Iny();*/

    /*returnTest t;
    t.rti();
    t.rts();*/
}

int main (int argc, char* argv[]) {
    Test();

    
    CPU c (1, 0);
/*
    //lda $30
    c.mem[1] = 0xA5;
    c.mem[2] = 0x30;

    c.mem[0x30] = 10;

    while (c.cycle < 3) {
        u8 instruction = c.mem[c.PC];
        auto instruct = c.instructions[instruction];
        auto command = std::get<0>(instruct);
        (c.*command)(std::get<1>(instruct));
    }

    std::cout << (int)c.A << std::endl;
  */  

    return 0;
}
