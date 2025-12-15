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

class adcTest {
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

        // adc ($20,X)
        c.A = 10;
        c.X = 2;
        c.mem[1] = 0x61;
        c.mem[2] = 0x20;

        c.mem[0x22] = 0x23;
        c.mem[0x23] = 0x40;

        c.mem[0x4023] = 15;

        driver(c, 6);

        assert(c.A == 25);
        std::cout << "IndirectX: Passed" << std::endl;
    }

    void ZeroPage() {
        CPU c(1, 0);

        // adc $30
        c.A = 20;
        c.mem[1] = 0x65;
        c.mem[2] = 0x30;

        c.mem[0x30] = 10;

        driver(c, 3);

        assert(c.A == 30);
        std::cout << "ZeroPage: Passed" << std::endl;
    }

    void Immediate() {
        CPU c(1, 0);

        // adc #$10;
        c.A = 1;
        c.mem[1] = 0x69;
        c.mem[2] = 0x10;

        driver(c, 2);

        assert(c.A == 0x11);
        std::cout << "Immediate: Passed" << std::endl;
    }

    void Absolute() {
        CPU c(1, 0);

        // adc $2A30
        c.A = 15;
        c.mem[1] = 0x6D;
        c.mem[2] = 0x30;
        c.mem[3] = 0x2A;

        c.mem[0x2A30] = 15;

        driver(c, 4);

        assert(c.A == 30);
        std::cout << "Absolute: Passed" << std::endl;
    }

    void IndirectY() {
        CPU c(1, 0);

        // adc ($30), Y
        c.A = 7;
        c.Y = 5;
        c.mem[1] = 0x71;
        c.mem[2] = 0x30;

        c.mem[0x30] = 0xD3;
        c.mem[0x31] = 0x34;

        c.mem[0x34D8] = 23;

        driver(c, 5);

        assert(c.A == 30);
        std::cout << "IndirectY: Passed" << std::endl;
    }

    void ZeroPageX() {
        CPU c(1, 0);

        // adc $20,X
        c.A = 5;
        c.X = 10;
        c.mem[1] = 0x75;
        c.mem[2] = 0x20;

        c.mem[0x2A] = 4;

        driver(c, 4);
        assert(c.A == 9);
        std::cout << "ZeroPageX: Passed" << std::endl;
    }

    void AbsoluteY() {
        CPU c(1, 0);

        // adc $2C3D,Y
        c.A = 5;
        c.Y = 2;
        c.mem[1] = 0x79;
        c.mem[2] = 0x3D;
        c.mem[3] = 0x2C;

        c.mem[0x2C3F] = 10;

        driver(c, 4);
        assert(c.A == 15);
        std::cout << "AbsoluteY: Passed" << std::endl;
    }

    void AbsoluteX() {
        CPU c(1, 0);

        // lda $2C3D,X
        c.A = 5;
        c.X = 2;
        c.mem[1] = 0x7D;
        c.mem[2] = 0x3D;
        c.mem[3] = 0x2C;

        c.mem[0x2C3F] = 10;

        driver(c, 4);
        assert(c.A == 15);
        std::cout << "AbsoluteX: Passed" << std::endl;
    }
};

class andTest {
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

        // and ($20,X)
        c.A = 0b11110000;
        c.X = 2;
        c.mem[1] = 0x21;
        c.mem[2] = 0x20;

        c.mem[0x22] = 0x23;
        c.mem[0x23] = 0x40;

        c.mem[0x4023] = 0b00001111;

        driver(c, 6);

        assert(c.A == 0);
        std::cout << "IndirectX: Passed" << std::endl;
    }

    void ZeroPage() {
        CPU c(1, 0);

        // and $30
        c.A = 0b11110000;
        c.mem[1] = 0x25;
        c.mem[2] = 0x30;

        c.mem[0x30] = 0b00001111;

        driver(c, 3);

        assert(c.A == 0);
        std::cout << "ZeroPage: Passed" << std::endl;
    }

    void Immediate() {
        CPU c(1, 0);

        // and #$10;
        c.A = 0b00001111;
        c.mem[1] = 0x29;
        c.mem[2] = 0b11110000;

        driver(c, 2);

        assert(c.A == 0);
        std::cout << "Immediate: Passed" << std::endl;
    }

    void Absolute() {
        CPU c(1, 0);

        // and $2A30
        c.A = 0b11110000;
        c.mem[1] = 0x2D;
        c.mem[2] = 0x30;
        c.mem[3] = 0x2A;

        c.mem[0x2A30] = 0b00001111;

        driver(c, 4);

        assert(c.A == 0);
        std::cout << "Absolute: Passed" << std::endl;
    }

    void IndirectY() {
        CPU c(1, 0);

        // and ($30), Y
        c.A = 0b11110000;
        c.Y = 5;
        c.mem[1] = 0x31;
        c.mem[2] = 0x30;

        c.mem[0x30] = 0xD3;
        c.mem[0x31] = 0x34;

        c.mem[0x34D8] = 0b00001111;

        driver(c, 5);

        assert(c.A == 0);
        std::cout << "IndirectY: Passed" << std::endl;
    }

    void ZeroPageX() {
        CPU c(1, 0);

        // and $20,X
        c.A = 0b11110000;
        c.X = 10;
        c.mem[1] = 0x35;
        c.mem[2] = 0x20;

        c.mem[0x2A] = 0b00001111;

        driver(c, 4);
        assert(c.A == 0);
        std::cout << "ZeroPageX: Passed" << std::endl;
    }

    void AbsoluteY() {
        CPU c(1, 0);

        // and $2C3D,Y
        c.A = 0b11110000;
        c.Y = 2;
        c.mem[1] = 0x39;
        c.mem[2] = 0x3D;
        c.mem[3] = 0x2C;

        c.mem[0x2C3F] = 0b00001111;

        driver(c, 4);
        assert(c.A == 0);
        std::cout << "AbsoluteY: Passed" << std::endl;
    }

    void AbsoluteX() {
        CPU c(1, 0);

        // and $2C3D,X
        c.A = 0b11110000;
        c.X = 2;
        c.mem[1] = 0x3D;
        c.mem[2] = 0x3D;
        c.mem[3] = 0x2C;

        c.mem[0x2C3F] = 0b00001111;

        driver(c, 4);
        assert(c.A == 0);
        std::cout << "AbsoluteX: Passed" << std::endl;
    }
};

class aslTest {
public:
    void driver(CPU& c, int cycles) {
        while (c.cycle < cycles) {
            u8 instruction = c.mem[c.PC];
            auto instruct = c.instructions[instruction];
            auto command = std::get<0>(instruct);
            (c.*command)(std::get<1>(instruct));
        }
    }

    void ZeroPage() {
        CPU c(1, 0);

        // asl $30
        c.mem[1] = 0x06;
        c.mem[2] = 0x30;

        c.mem[0x30] = 0b00000001;

        driver(c, 5);

        assert(c.mem[0x30] == 2);
        std::cout << "ZeroPage: Passed" << std::endl;
    }

    void Accumulator() {
        CPU c(1, 0);

        // asl A
        c.A = 0b00000001;
        c.mem[1] = 0x0A;

        driver(c, 2);

        assert(c.A == 2);
        std::cout << "Accumulator: Passed" << std::endl;
    }

    void Absolute() {
        CPU c(1, 0);

        // asl $2A30
        c.mem[1] = 0x0E;
        c.mem[2] = 0x30;
        c.mem[3] = 0x2A;

        c.mem[0x2A30] = 0b00000001;

        driver(c, 6);

        assert(c.mem[0x2A30] == 2);
        std::cout << "Absolute: Passed" << std::endl;
    }

    void ZeroPageX() {
        CPU c(1, 0);

        // asl $20,X
        c.X = 10;
        c.mem[1] = 0x16;
        c.mem[2] = 0x20;

        c.mem[0x2A] = 0b00000001;

        driver(c, 6);

        assert(c.mem[0x2A] == 2);
        std::cout << "ZeroPageX: Passed" << std::endl;
    }

    void AbsoluteX() {
        CPU c(1, 0);

        // asl $2C3D,X
        c.X = 2;
        c.mem[1] = 0x1E;
        c.mem[2] = 0x3D;
        c.mem[3] = 0x2C;

        c.mem[0x2C3F] = 0b00000001;

        driver(c, 7);
        assert(c.mem[0x2C3F] == 2);
        std::cout << "AbsoluteX: Passed" << std::endl;
    }
};
