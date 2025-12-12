#include <iostream>
#include <cassert>
#include "../CPU.h"

class ldaTest {
public:

    void driver(CPU& c, int cycles) {
        while (c.cycle < cycles) {
            u8 instruction = c.mem[c.PC];
            auto instruct = c.instructions[instruction];
            auto command = std::get<0>(instruct);
            (c.*command)(std::get<1>(instruct));
        }
    }

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

        driver(c, 6);

        assert(c.A == 27);
        std::cout << "IndirectX: Passed" << std::endl;
    }

    void ZeroPage() {
        CPU c(1, 0);

        //lda $30
        c.mem[1] = 0xA5;
        c.mem[2] = 0x30;

        c.mem[0x30] = 10;

        driver(c, 3);

        assert(c.A == 10);
        std::cout << "ZeroPage: Passed" << std::endl;
    }

    void Immediate() {
        CPU c(1, 0);

        // lda #$10;
        c.mem[1] = 0xA9;
        c.mem[2] = 0x10;

        driver(c, 2);

        assert(c.A == 0x10);
        std::cout << "Immediate: Passed" << std::endl;
    }

    void Absolute() {
        CPU c(1, 0);

        // lda $2A30
        c.mem[1] = 0xAD;
        c.mem[2] = 0x30;
        c.mem[3] = 0x2A;

        c.mem[0x2A30] = 15;

        driver(c, 4);

        assert(c.A == 15);
        std::cout << "Absolute: Passed" << std::endl;
    }

    void IndirectY() {
        CPU c(1, 0);

        // lda ($30), Y
        c.Y = 5;
        c.mem[1] = 0xB1;
        c.mem[2] = 0x30;

        c.mem[0x30] = 0xD3;
        c.mem[0x31] = 0x34;

        c.mem[0x34D8] = 23;

        driver(c, 5);

        assert(c.A == 23);
        std::cout << "IndirectY: Passed" << std::endl;
    }

    void ZeroPageX() {
        CPU c(1, 0);

        // lda $20,X
        c.X = 10;
        c.mem[1] = 0xB5;
        c.mem[2] = 0x20;

        c.mem[0x2A] = 4;

        driver(c, 4);
        assert(c.A == 4);
        std::cout << "ZeroPageX: Passed" << std::endl;
    }

    void AbsoluteY() {
        CPU c(1, 0);

        // lda $2C3D,Y
        c.Y = 2;
        c.mem[1] = 0xB9;
        c.mem[2] = 0x3D;
        c.mem[3] = 0x2C;

        c.mem[0x2C3F] = 10;

        driver(c, 4);
        assert(c.A == 10);
        std::cout << "AbsoluteY: Passed" << std::endl;
    }

    void AbsoluteX() {
        CPU c(1, 0);

        // lda $2C3D,X
        c.X = 2;
        c.mem[1] = 0xBD;
        c.mem[2] = 0x3D;
        c.mem[3] = 0x2C;

        c.mem[0x2C3F] = 10;

        driver(c, 4);
        assert(c.A == 10);
        std::cout << "AbsoluteX: Passed" << std::endl;
    }
};
