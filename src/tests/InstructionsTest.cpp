#include <iostream>
#include <cassert>
#include "InstructionsTest.h"

void driver(CPU& c, bool ignoreFirstBrk) {
    bool skippedFirstBrk = false;

    while (true) {
        u8 instruction = c.mem[c.PC];

        // Stop if hit BRK
        if (instruction == 0x00) {
            if (ignoreFirstBrk && !skippedFirstBrk) {
                skippedFirstBrk = true;
                // We don't break here so the CPU actually executes the BRK
            } else {
                break;
            }
        }

        auto instruct = c.instructions[instruction];
        auto command = instruct.instruction;
        (c.*command)();
    }
}

// =========================
// ldaTest
// =========================
void ldaTest::IndirectX() {
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

    driver(c, false);

    assert(c.A == 27);
    std::cout << "IndirectX: Passed" << std::endl;
}

void ldaTest::ZeroPage() {
    CPU c(1, 0);

    //lda $30
    c.mem[1] = 0xA5;
    c.mem[2] = 0x30;

    c.mem[0x30] = 10;

    driver(c, false);

    assert(c.A == 10);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void ldaTest::Immediate() {
    CPU c(1, 0);

    // lda #$10;
    c.mem[1] = 0xA9;
    c.mem[2] = 0x10;

    driver(c, false);

    assert(c.A == 0x10);
    std::cout << "Immediate: Passed" << std::endl;
}

void ldaTest::Absolute() {
    CPU c(1, 0);

    // lda $2A30
    c.mem[1] = 0xAD;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 15;

    driver(c, false);

    assert(c.A == 15);
    std::cout << "Absolute: Passed" << std::endl;
}

void ldaTest::IndirectY() {
    CPU c(1, 0);

    // lda ($30), Y
    c.Y = 5;
    c.mem[1] = 0xB1;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0xD3;
    c.mem[0x31] = 0x34;

    c.mem[0x34D8] = 23;

    driver(c, false);

    assert(c.A == 23);
    std::cout << "IndirectY: Passed" << std::endl;
}

void ldaTest::ZeroPageX() {
    CPU c(1, 0);

    // lda $20,X
    c.X = 10;
    c.mem[1] = 0xB5;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 4;

    driver(c, false);
    assert(c.A == 4);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void ldaTest::AbsoluteY() {
    CPU c(1, 0);

    // lda $2C3D,Y
    c.Y = 2;
    c.mem[1] = 0xB9;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 10;

    driver(c, false);
    assert(c.A == 10);
    std::cout << "AbsoluteY: Passed" << std::endl;
}

void ldaTest::AbsoluteX() {
    CPU c(1, 0);

    // lda $2C3D,X
    c.X = 2;
    c.mem[1] = 0xBD;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 10;

    driver(c, false);
    assert(c.A == 10);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// adcTest
// =========================
void adcTest::IndirectX() {
    CPU c(1, 0);

    // adc ($20,X)
    c.A = 10;
    c.X = 2;
    c.mem[1] = 0x61;
    c.mem[2] = 0x20;

    c.mem[0x22] = 0x23;
    c.mem[0x23] = 0x40;

    c.mem[0x4023] = 15;

    driver(c, false);

    assert(c.A == 25);
    std::cout << "IndirectX: Passed" << std::endl;
}

void adcTest::ZeroPage() {
    CPU c(1, 0);

    // adc $30
    c.A = 20;
    c.mem[1] = 0x65;
    c.mem[2] = 0x30;

    c.mem[0x30] = 10;

    driver(c, false);

    assert(c.A == 30);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void adcTest::Immediate() {
    CPU c(1, 0);

    // adc #$10;
    c.A = 1;
    c.mem[1] = 0x69;
    c.mem[2] = 0x10;

    driver(c, false);

    assert(c.A == 0x11);
    std::cout << "Immediate: Passed" << std::endl;
}

void adcTest::Absolute() {
    CPU c(1, 0);

    // adc $2A30
    c.A = 15;
    c.mem[1] = 0x6D;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 15;

    driver(c, false);

    assert(c.A == 30);
    std::cout << "Absolute: Passed" << std::endl;
}

void adcTest::IndirectY() {
    CPU c(1, 0);

    // adc ($30), Y
    c.A = 7;
    c.Y = 5;
    c.mem[1] = 0x71;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0xD3;
    c.mem[0x31] = 0x34;

    c.mem[0x34D8] = 23;

    driver(c, false);

    assert(c.A == 30);
    std::cout << "IndirectY: Passed" << std::endl;
}

void adcTest::ZeroPageX() {
    CPU c(1, 0);

    // adc $20,X
    c.A = 5;
    c.X = 10;
    c.mem[1] = 0x75;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 4;

    driver(c, false);
    assert(c.A == 9);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void adcTest::AbsoluteY() {
    CPU c(1, 0);

    // adc $2C3D,Y
    c.A = 5;
    c.Y = 2;
    c.mem[1] = 0x79;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 10;

    driver(c, false);
    assert(c.A == 15);
    std::cout << "AbsoluteY: Passed" << std::endl;
}

void adcTest::AbsoluteX() {
    CPU c(1, 0);

    // lda $2C3D,X
    c.A = 5;
    c.X = 2;
    c.mem[1] = 0x7D;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 10;

    driver(c, false);
    assert(c.A == 15);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// andTest
// =========================
void andTest::IndirectX() {
    CPU c(1, 0);

    // and ($20,X)
    c.A = 0b11110000;
    c.X = 2;
    c.mem[1] = 0x21;
    c.mem[2] = 0x20;

    c.mem[0x22] = 0x23;
    c.mem[0x23] = 0x40;

    c.mem[0x4023] = 0b00001111;

    driver(c, false);

    assert(c.A == 0);
    std::cout << "IndirectX: Passed" << std::endl;
}

void andTest::ZeroPage() {
    CPU c(1, 0);

    // and $30
    c.A = 0b11110000;
    c.mem[1] = 0x25;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0b00001111;

    driver(c, false);

    assert(c.A == 0);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void andTest::Immediate() {
    CPU c(1, 0);

    // and #$10;
    c.A = 0b00001111;
    c.mem[1] = 0x29;
    c.mem[2] = 0b11110000;

    driver(c, false);

    assert(c.A == 0);
    std::cout << "Immediate: Passed" << std::endl;
}

void andTest::Absolute() {
    CPU c(1, 0);

    // and $2A30
    c.A = 0b11110000;
    c.mem[1] = 0x2D;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 0b00001111;

    driver(c, false);

    assert(c.A == 0);
    std::cout << "Absolute: Passed" << std::endl;
}

void andTest::IndirectY() {
    CPU c(1, 0);

    // and ($30), Y
    c.A = 0b11110000;
    c.Y = 5;
    c.mem[1] = 0x31;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0xD3;
    c.mem[0x31] = 0x34;

    c.mem[0x34D8] = 0b00001111;

    driver(c, false);

    assert(c.A == 0);
    std::cout << "IndirectY: Passed" << std::endl;
}

void andTest::ZeroPageX() {
    CPU c(1, 0);

    // and $20,X
    c.A = 0b11110000;
    c.X = 10;
    c.mem[1] = 0x35;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 0b00001111;

    driver(c, false);
    assert(c.A == 0);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void andTest::AbsoluteY() {
    CPU c(1, 0);

    // and $2C3D,Y
    c.A = 0b11110000;
    c.Y = 2;
    c.mem[1] = 0x39;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00001111;

    driver(c, false);
    assert(c.A == 0);
    std::cout << "AbsoluteY: Passed" << std::endl;
}

void andTest::AbsoluteX() {
    CPU c(1, 0);

    // and $2C3D,X
    c.A = 0b11110000;
    c.X = 2;
    c.mem[1] = 0x3D;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00001111;

    driver(c, false);
    assert(c.A == 0);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// aslTest
// =========================
void aslTest::ZeroPage() {
    CPU c(1, 0);

    // asl $30
    c.mem[1] = 0x06;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x30] == 2);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void aslTest::Accumulator() {
    CPU c(1, 0);

    // asl A
    c.A = 0b00000001;
    c.mem[1] = 0x0A;

    driver(c, false);

    assert(c.A == 2);
    std::cout << "Accumulator: Passed" << std::endl;
}

void aslTest::Absolute() {
    CPU c(1, 0);

    // asl $2A30
    c.mem[1] = 0x0E;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x2A30] == 2);
    std::cout << "Absolute: Passed" << std::endl;
}

void aslTest::ZeroPageX() {
    CPU c(1, 0);

    // asl $20,X
    c.X = 10;
    c.mem[1] = 0x16;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x2A] == 2);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void aslTest::AbsoluteX() {
    CPU c(1, 0);

    // asl $2C3D,X
    c.X = 2;
    c.mem[1] = 0x1E;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00000001;

    driver(c, false);
    assert(c.mem[0x2C3F] == 2);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// branchTest
// =========================
void branchTest::BCC() {
    CPU c (10, 0);

    // Test sets the accumulator to 1 then branches to
    // different part of program where it is set to 5
    c.A = 1;
    c.mem[10] = 0x90;
    // Two's complement negative 5
    c.mem[11] = 0b11111011;

    c.mem[5] = 0xA9;
    c.mem[6] = 5;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "BCC: passed" << std::endl;
}

void branchTest::BCS() {
    CPU c (10, 0);

    // Test sets the accumulator to 1 then branches to
    // different part of program where it is set to 5
    c.A = 1;
    c.C = 1;
    c.mem[10] = 0xB0;
    // Two's complement negative 5
    c.mem[11] = 0b11111011;
    
    c.mem[5] = 0xA9;
    c.mem[6] = 5;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "BCS: passed" << std::endl;
}

void branchTest::BEQ() {
    CPU c (10, 0);

    c.Z = 1;
    c.A = 1;
    c.mem[10] = 0xF0;
    c.mem[11] = 0b11111011;

    c.mem[5] = 0xA9;
    c.mem[6] = 5;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "BEQ: passed" << std::endl;
}

void branchTest::BMI() {
    CPU c (10, 0);

    c.N = 1;
    c.A = 1;
    c.mem[10] = 0x30;
    c.mem[11] = 0b11111011;

    c.mem[5] = 0xA9;
    c.mem[6] = 5;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "BMI: passed" << std::endl;
}

void branchTest::BNE() {
    CPU c (10, 0);

    c.Z = 0;
    c.A = 1;
    c.mem[10] = 0xD0;
    c.mem[11] = 0b11111011;

    c.mem[5] = 0xA9;
    c.mem[6] = 5;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "BNE: passed" << std::endl;
}

void branchTest::BPL() {
    CPU c (10, 0);

    c.N = 0;
    c.A = 1;
    c.mem[10] = 0x10;
    c.mem[11] = 0b11111011;

    c.mem[5] = 0xA9;
    c.mem[6] = 5;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "BPL: passed" << std::endl;
}

void branchTest::BVC() {
    CPU c (10, 0);

    c.V = 0;
    c.A = 1;
    c.mem[10] = 0x50;
    c.mem[11] = 0b11111011;

    c.mem[5] = 0xA9;
    c.mem[6] = 5;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "BVC: passed" << std::endl;
}

void branchTest::BVS() {
    CPU c (10, 0);

    c.V = 1;
    c.A = 1;
    c.mem[10] = 0x70;
    c.mem[11] = 0b11111011;

    c.mem[5] = 0xA9;
    c.mem[6] = 5;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "BVS: passed" << std::endl;
}

void branchTest::realProgram() {
    CPU c(1, 0);

    // lda #$FA
    // adc #$6
    // bcs SET_TEN
    // bcc SET_FIFTY
    //
    // SET_TEN: lda #$A
    // SET_FIFTY: lda #$32

    c.mem[1] = 0xA9;
    c.mem[2] = 0xF5;

    c.mem[3] = 0x69;
    c.mem[4] = 6;

    c.mem[5] = 0xB0;
    c.mem[6] = 0x1B;
    
    c.mem[7] = 0x90;
    c.mem[8] = 0x29;

    c.mem[0x20] = 0xA9;
    c.mem[0x21] = 10;

    c.mem[0x30] = 0xA9;
    c.mem[0x31] = 50;

    driver(c, false);

    std::cout << "Accumulator == " << (int)c.A << std::endl;
}

// =========================
// cmpTest
// =========================
void cmpTest::IndirectX() {
    CPU c(1, 0);

    // cmp ($20,X)
    c.A = 5;
    c.X = 2;
    c.mem[1] = 0xc1;
    c.mem[2] = 0x20;

    c.mem[0x22] = 0x23;
    c.mem[0x23] = 0x40;

    c.mem[0x4023] = 10;

    driver(c, false);

    // Less than, C = 0, Z = 0, N = 0
    assert(c.C == 0 && c.Z == 0 && c.N == 0);
    std::cout << "IndirectX: Passed" << std::endl;
}

void cmpTest::ZeroPage() {
    CPU c(1, 0);

    // cmp $30
    c.A = 10;
    c.mem[1] = 0xc5;
    c.mem[2] = 0x30;

    c.mem[0x30] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void cmpTest::Immediate() {
    CPU c(1, 0);

    // cmp #$10;
    c.A = 10;
    c.mem[1] = 0xc9;
    c.mem[2] = 10;

    driver(c, false);

    // Equal, C == 1, Z = 1, N = 0
    assert(c.C == 1 && c.Z == 1 && c.N == 0);
    std::cout << "Immediate: Passed" << std::endl;
}

void cmpTest::Absolute() {
    CPU c(1, 0);

    // cmp $2A30
    c.A = 10;
    c.mem[1] = 0xcD;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "Absolute: Passed" << std::endl;
}

void cmpTest::IndirectY() {
    CPU c(1, 0);

    // cmp ($30), Y
    c.A = 10;
    c.Y = 5;
    c.mem[1] = 0xd1;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0xD3;
    c.mem[0x31] = 0x34;

    c.mem[0x34D8] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "IndirectY: Passed" << std::endl;
}

void cmpTest::ZeroPageX() {
    CPU c(1, 0);

    // cmp $20,X
    c.A = 10;
    c.X = 10;
    c.mem[1] = 0xd5;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void cmpTest::AbsoluteY() {
    CPU c(1, 0);

    // cmp $2C3D,Y
    c.A = 10;
    c.Y = 2;
    c.mem[1] = 0xd9;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "AbsoluteY: Passed" << std::endl;
}

void cmpTest::AbsoluteX() {
    CPU c(1, 0);

    // cmp $2C3D,X
    c.A = 10;
    c.X = 2;
    c.mem[1] = 0xDD;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// bitTest
// =========================
void bitTest::ZeroPage() {
    CPU c(1, 0);
    
    c.A = 0b11110000;
    c.mem[1] = 0x24;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0b00001111;

    driver(c, false);

    assert(c.Z == 1);
    assert(c.N == 0);
    assert(c.V == 0);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void bitTest::Absolute() {
    CPU c(1, 0);

    c.A = 0b01110011;
    c.mem[1] = 0x2C;
    c.mem[2] = 0x30;
    c.mem[3] = 0x4C;

    c.mem[0x4C30] = 0b11101100;

    driver(c, false);

    assert(c.Z == 0);
    assert(c.N == 1);
    assert(c.V == 1);
    std::cout << "Absolute: Passed" << std::endl;
}

// =========================
// brkTest
// =========================
void brkTest::Implied() {
    CPU c(1, 0);

    // lda #$FF
    // adc #$2
    // brk
    // ...
    // lda #$A

    // Set IRQ vector
    c.mem[0xFFFE] = 0x23;
    c.mem[0xFFFF] = 0xA3;
    c.A = 255;
    // adc #$2
    c.mem[1] = 0x69;
    c.mem[2] = 2;
    // brk
    c.mem[3] = 0x00;

    c.mem[0xA323] = 0xA9;
    c.mem[0xA324] = 10;

    // We pass true to let the driver execute our test's brk
    driver(c, true);

    assert(c.A == 10);
    assert(c.mem[(0x0100 | c.S) + 1] == 0b01110101);
    std::cout << "Implied: Passed" << std::endl;

}

// =========================
// cpxTest
// =========================
void cpxTest::ZeroPage() {
    CPU c(1, 0);

    // cpx $30
    c.X = 10;
    c.mem[1] = 0xE4;
    c.mem[2] = 0x30;

    c.mem[0x30] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void cpxTest::Immediate() {
    CPU c(1, 0);

    // cpx #$10;
    c.X = 10;
    c.mem[1] = 0xE0;
    c.mem[2] = 10;

    driver(c, false);

    // Equal, C == 1, Z = 1, N = 0
    assert(c.C == 1 && c.Z == 1 && c.N == 0);
    std::cout << "Immediate: Passed" << std::endl;
}

void cpxTest::Absolute() {
    CPU c(1, 0);

    // cpx $2A30
    c.X = 10;
    c.mem[1] = 0xEC;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "Absolute: Passed" << std::endl;
}

// =========================
// cpyTest
// =========================
void cpyTest::ZeroPage() {
    CPU c(1, 0);

    // cpy $30
    c.Y = 10;
    c.mem[1] = 0xC4;
    c.mem[2] = 0x30;

    c.mem[0x30] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void cpyTest::Immediate() {
    CPU c(1, 0);

    // cpy #$10;
    c.Y = 10;
    c.mem[1] = 0xC0;
    c.mem[2] = 10;

    driver(c, false);

    // Equal, C == 1, Z = 1, N = 0
    assert(c.C == 1 && c.Z == 1 && c.N == 0);
    std::cout << "Immediate: Passed" << std::endl;
}

void cpyTest::Absolute() {
    CPU c(1, 0);

    // cpy $2A30
    c.Y = 10;
    c.mem[1] = 0xCC;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 5;

    driver(c, false);

    // Greater than, C = 1, Z = 0, N = 0
    assert(c.C == 1 && c.Z == 0 && c.N == 0);
    std::cout << "Absolute: Passed" << std::endl;
}

// =========================
// decTest
// =========================
void decTest::ZeroPage() {
    CPU c(1, 0);

    // dec $30
    c.mem[1] = 0xC6;
    c.mem[2] = 0x30;
    
    c.mem[0x30] = 10;

    driver(c, false);
    
    assert(c.mem[0x30] == 9);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void decTest::Absolute() {
    CPU c(1, 0);

    // dec $3020
    c.mem[1] = 0xCE;
    c.mem[2] = 0x20;
    c.mem[3] = 0x30;

    c.mem[0x3020] = 10;

    driver(c, false);

    assert(c.mem[0x3020] == 9);
    std::cout << "Absolute: Passed" << std::endl;
}

void decTest::ZeroPageX() {
    CPU c(1, 0);

    // dec $30,X
    c.X = 2;
    c.mem[1] = 0xD6;
    c.mem[2] = 0x30;

    c.mem[0x32] = 10;

    driver(c, false);

    assert(c.mem[0x32] == 9);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void decTest::AbsoluteX() {
    CPU c(1, 0);

    // dec $3020,X
    c.X = 5;
    c.mem[1] = 0xDE;
    c.mem[2] = 0x20;
    c.mem[3] = 0x30;

    c.mem[0x3025] = 10;

    driver(c, false);

    assert(c.mem[0x3025] == 9);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// deXYTest
// =========================
void deXYTest::Dex() {
    CPU c(1, 0);

    // DEX
    c.X = 10;
    c.mem[1] = 0xCA;

    driver(c, false);

    assert(c.X == 9);
    std::cout << "DEX: Passed" << std::endl;
}

void deXYTest::Dey() {
    CPU c(1, 0);

    // DEY
    c.Y = 10;
    c.mem[1] = 0x88;

    driver(c, false);

    assert(c.Y == 9);
    std::cout << "DEY: Passed" << std::endl;
}

// =========================
// eorTest
// =========================
void eorTest::IndirectX() {
    CPU c(1, 0);

    // eor ($20,X)
    c.A = 0b11110000;
    c.X = 2;
    c.mem[1] = 0x41;
    c.mem[2] = 0x20;

    c.mem[0x22] = 0x23;
    c.mem[0x23] = 0x40;

    c.mem[0x4023] = 0b00001111;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "IndirectX: Passed" << std::endl;
}

void eorTest::ZeroPage() {
    CPU c(1, 0);

    // eor $30
    c.A = 0b11110000;
    c.mem[1] = 0x45;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0b00001111;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void eorTest::Immediate() {
    CPU c(1, 0);

    // eor #$10;
    c.A = 0b00001111;
    c.mem[1] = 0x49;
    c.mem[2] = 0b11110000;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "Immediate: Passed" << std::endl;
}

void eorTest::Absolute() {
    CPU c(1, 0);

    // eor $2A30
    c.A = 0b11110000;
    c.mem[1] = 0x4D;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 0b00001111;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "Absolute: Passed" << std::endl;
}

void eorTest::IndirectY() {
    CPU c(1, 0);

    // eor ($30), Y
    c.A = 0b11110000;
    c.Y = 5;
    c.mem[1] = 0x51;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0xD3;
    c.mem[0x31] = 0x34;

    c.mem[0x34D8] = 0b00001111;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "IndirectY: Passed" << std::endl;
}

void eorTest::ZeroPageX() {
    CPU c(1, 0);

    // eor $20,X
    c.A = 0b11110000;
    c.X = 10;
    c.mem[1] = 0x55;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 0b00001111;

    driver(c, false);
    assert(c.A == 255);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void eorTest::AbsoluteY() {
    CPU c(1, 0);

    // eor $2C3D,Y
    c.A = 0b11110000;
    c.Y = 2;
    c.mem[1] = 0x59;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00001111;

    driver(c, false);
    assert(c.A == 255);
    std::cout << "AbsoluteY: Passed" << std::endl;
}

void eorTest::AbsoluteX() {
    CPU c(1, 0);

    // eor $2C3D,X
    c.A = 0b11110000;
    c.X = 2;
    c.mem[1] = 0x5D;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00001111;

    driver(c, false);
    assert(c.A == 255);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// incTest
// =========================
void incTest::ZeroPage() {
    CPU c(1, 0);

    // inc $30
    c.mem[1] = 0xE6;
    c.mem[2] = 0x30;
    
    c.mem[0x30] = 10;

    driver(c, false);
    
    assert(c.mem[0x30] == 11);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void incTest::Absolute() {
    CPU c(1, 0);

    // inc $3020
    c.mem[1] = 0xEE;
    c.mem[2] = 0x20;
    c.mem[3] = 0x30;

    c.mem[0x3020] = 10;

    driver(c, false);

    assert(c.mem[0x3020] == 11);
    std::cout << "Absolute: Passed" << std::endl;
}

void incTest::ZeroPageX() {
    CPU c(1, 0);

    // inc $30,X
    c.X = 2;
    c.mem[1] = 0xF6;
    c.mem[2] = 0x30;

    c.mem[0x32] = 10;

    driver(c, false);

    assert(c.mem[0x32] == 11);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void incTest::AbsoluteX() {
    CPU c(1, 0);

    // inc $3020,X
    c.X = 5;
    c.mem[1] = 0xFE;
    c.mem[2] = 0x20;
    c.mem[3] = 0x30;

    c.mem[0x3025] = 10;

    driver(c, false);

    assert(c.mem[0x3025] == 11);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// inXYTest
// =========================
void inXYTest::Inx() {
    CPU c(1, 0);

    // INX
    c.X = 10;
    c.mem[1] = 0xE8;

    driver(c, false);

    assert(c.X == 11);
    std::cout << "INX: Passed" << std::endl;
}

void inXYTest::Iny() {
    CPU c(1, 0);

    // INY
    c.Y = 10;
    c.mem[1] = 0xC8;

    driver(c, false);

    assert(c.Y == 11);
    std::cout << "INY: Passed" << std::endl;
}

// =========================
// jmpTest
// =========================
void jmpTest::Absolute() {
    CPU c(1, 0);

    c.mem[1] = 0x4C;
    c.mem[2] = 0x33;
    c.mem[3] = 0xD4;

    driver(c, false);

    assert(c.PC == 0xD433);
    std::cout << "Absolute: Passed" << std::endl;
}

void jmpTest::Indirect() { 
    CPU c(1, 0);

    c.mem[1] = 0x6C;
    c.mem[2] = 0x45;
    c.mem[3] = 0x2E;

    c.mem[0x2E45] = 0xD6;
    c.mem[0x2E46] = 0x36;

    driver(c, false);

    assert(c.PC == 0x36D6);
    std::cout << "Indirect: Passed" << std::endl;
}

// =========================
// ldxTest
// =========================
void ldxTest::Immediate() {
    CPU c(1, 0);

    c.mem[1] = 0xA2;
    c.mem[2] = 0x20;

    driver(c, false);

    assert(c.X == 0x20);
    std::cout << "Immediate: Passed" << std::endl;
}

void ldxTest::ZeroPage() {
    CPU c(1, 0);

    c.mem[1] = 0xA6;
    c.mem[2] = 0x3A;
    c.mem[0x3A] = 23;

    driver(c, false);

    assert(c.X == 23);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void ldxTest::Absolute() {
    CPU c(1, 0);

    c.mem[1] = 0xAE;
    c.mem[2] = 0x32;
    c.mem[3] = 0x53;

    c.mem[0x5332] = 10;

    driver(c, false);

    assert(c.X == 10);
    std::cout << "Absolute: Passed" << std::endl;
}

void ldxTest::ZeroPageY() {
    CPU c(1, 0);

    c.Y = 5;
    c.mem[1] = 0xB6;
    c.mem[2] = 0x20;
    
    c.mem[0x25] = 10;

    driver(c, false);

    assert(c.X == 10);
    std::cout << "ZeroPageY: Passed" << std::endl;
}

void ldxTest::AbsoluteY() {
    CPU c(1, 0);

    c.Y = 5;
    c.mem[1] = 0xBE;
    c.mem[2] = 0x30;
    c.mem[3] = 0x5A;

    c.mem[0x5A35] = 10;

    driver(c, false);

    assert(c.X == 10);
    std::cout << "AbsoluteY: Passed" << std::endl;
}

// =========================
// ldyTest
// =========================
void ldyTest::Immediate() {
    CPU c(1, 0);

    c.mem[1] = 0xA0;
    c.mem[2] = 0x20;

    driver(c, false);

    assert(c.Y == 0x20);
    std::cout << "Immediate: Passed" << std::endl;
}

void ldyTest::ZeroPage() {
    CPU c(1, 0);

    c.mem[1] = 0xA4;
    c.mem[2] = 0x3A;
    c.mem[0x3A] = 23;

    driver(c, false);

    assert(c.Y == 23);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void ldyTest::Absolute() {
    CPU c(1, 0);

    c.mem[1] = 0xAC;
    c.mem[2] = 0x32;
    c.mem[3] = 0x53;

    c.mem[0x5332] = 10;

    driver(c, false);

    assert(c.Y == 10);
    std::cout << "Absolute: Passed" << std::endl;
}

void ldyTest::ZeroPageX() {
    CPU c(1, 0);

    c.X = 5;
    c.mem[1] = 0xB4;
    c.mem[2] = 0x20;
    
    c.mem[0x25] = 10;

    driver(c, false);

    assert(c.Y == 10);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void ldyTest::AbsoluteX() {
    CPU c(1, 0);

    c.X = 5;
    c.mem[1] = 0xBC;
    c.mem[2] = 0x30;
    c.mem[3] = 0x5A;

    c.mem[0x5A35] = 10;

    driver(c, false);
    
    assert(c.Y == 10);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// lsrTest
// =========================
void lsrTest::ZeroPage() {
    CPU c(1, 0);

    // lsr $30
    c.mem[1] = 0x46;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x30] == 0);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void lsrTest::Accumulator() {
    CPU c(1, 0);

    // lsr A
    c.A = 0b00001010;
    c.mem[1] = 0x4A;

    driver(c, false);

    assert(c.A == 5);
    std::cout << "Accumulator: Passed" << std::endl;
}

void lsrTest::Absolute() {
    CPU c(1, 0);

    // lsr $2A30
    c.mem[1] = 0x4E;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x2A30] == 0);
    std::cout << "Absolute: Passed" << std::endl;
}

void lsrTest::ZeroPageX() {
    CPU c(1, 0);

    // lsr $20,X
    c.X = 10;
    c.mem[1] = 0x56;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x2A] == 0);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void lsrTest::AbsoluteX() {
    CPU c(1, 0);

    // lsr $2C3D,X
    c.X = 2;
    c.mem[1] = 0x5E;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00000001;

    driver(c, false);
    assert(c.mem[0x2C3F] == 0);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// orTest
// =========================
void orTest::IndirectX() {
    CPU c(1, 0);

    // or ($20,X)
    c.A = 0b11110000;
    c.X = 2;
    c.mem[1] = 0x01;
    c.mem[2] = 0x20;

    c.mem[0x22] = 0x23;
    c.mem[0x23] = 0x40;

    c.mem[0x4023] = 0b00001111;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "IndirectX: Passed" << std::endl;
}

void orTest::ZeroPage() {
    CPU c(1, 0);

    // or $30
    c.A = 0b11110000;
    c.mem[1] = 0x05;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0b00001111;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void orTest::Immediate() {
    CPU c(1, 0);

    // or #$10;
    c.A = 0b00001111;
    c.mem[1] = 0x09;
    c.mem[2] = 0b11110000;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "Immediate: Passed" << std::endl;
}

void orTest::Absolute() {
    CPU c(1, 0);

    // or $2A30
    c.A = 0b11110000;
    c.mem[1] = 0x0D;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 0b00001111;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "Absolute: Passed" << std::endl;
}

void orTest::IndirectY() {
    CPU c(1, 0);

    // or ($30), Y
    c.A = 0b11110000;
    c.Y = 5;
    c.mem[1] = 0x11;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0xD3;
    c.mem[0x31] = 0x34;

    c.mem[0x34D8] = 0b00001111;

    driver(c, false);

    assert(c.A == 255);
    std::cout << "IndirectY: Passed" << std::endl;
}

void orTest::ZeroPageX() {
    CPU c(1, 0);

    // or $20,X
    c.A = 0b11110000;
    c.X = 10;
    c.mem[1] = 0x15;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 0b00001111;

    driver(c, false);
    assert(c.A == 255);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void orTest::AbsoluteY() {
    CPU c(1, 0);

    // or $2C3D,Y
    c.A = 0b11110000;
    c.Y = 2;
    c.mem[1] = 0x19;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00001111;

    driver(c, false);
    assert(c.A == 255);
    std::cout << "AbsoluteY: Passed" << std::endl;
}

void orTest::AbsoluteX() {
    CPU c(1, 0);

    // or $2C3D,X
    c.A = 0b11110000;
    c.X = 2;
    c.mem[1] = 0x1D;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00001111;

    driver(c, false);
    assert(c.A == 255);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// pushPullTest
// =========================
void pushPullTest::pha() {
    CPU c(1, 0);

    c.A = 20;
    c.mem[1] = 0x48;

    driver(c, false);

    assert(c.mem[0x0100 | c.S + 1] == 20);
    std::cout << "pha: Passed" << std::endl;
}

void pushPullTest::php() {
    CPU c(1, 0);

    c.N = 1;
    c.V = 0;
    c.B = 1;
    c.I = 1;
    c.Z = 0;
    c.C = 1;

    c.mem[1] = 0x08;

    driver(c, false);

    assert(c.mem[0x0100 | c.S + 1] == 0b10110101);
    std::cout << "php: Passed" << std::endl;
}

void pushPullTest::pla() {
    CPU c(1, 0);

    c.pushStack(30);
    c.mem[1] = 0x68;

    driver(c, false);
    
    assert(c.A == 30);
    std::cout << "pla: Passed" << std::endl;
}

void pushPullTest::plp() {
    CPU c(1, 0);

    c.pushStack(0b11110000);
    c.mem[1] = 0x28;

    driver(c, false);

    assert(c.N == 1 && c.V == 1 && c.B == 1 && c.I == 0 && c.Z == 0 && c.C == 0);
    std::cout << "plp: Passed" << std::endl;
}

// =========================
// rolTest
// =========================
void rolTest::ZeroPage() {
    CPU c(1, 0);

    // rol $30
    c.C = 1;
    c.mem[1] = 0x26;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x30] == 3 && c.C == 0);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void rolTest::Accumulator() {
    CPU c(1, 0);

    // rol A
    c.C = 1;
    c.A = 0b00000101;
    c.mem[1] = 0x2A;

    driver(c, false);

    assert(c.A == 11 && c.C == 0);
    std::cout << "Accumulator: Passed" << std::endl;
}

void rolTest::Absolute() {
    CPU c(1, 0);

    // rol $2A30
    c.C = 1;
    c.mem[1] = 0x2E;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x2A30] == 3 && c.C == 0);
    std::cout << "Absolute: Passed" << std::endl;
}

void rolTest::ZeroPageX() {
    CPU c(1, 0);

    // rol $20,X
    c.X = 10;
    c.C = 1;
    c.mem[1] = 0x36;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x2A] == 3 && c.C == 0);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void rolTest::AbsoluteX() {
    CPU c(1, 0);

    // rol $2C3D,X
    c.X = 2;
    c.C = 1;
    c.mem[1] = 0x3E;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00000001;

    driver(c, false);
    assert(c.mem[0x2C3F] == 3 && c.C == 0);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// rorTest
// =========================
void rorTest::ZeroPage() {
    CPU c(1, 0);

    // ror $30
    c.C = 1;
    c.mem[1] = 0x66;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x30] == 128 && c.C == 1);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void rorTest::Accumulator() {
    CPU c(1, 0);

    // ror A
    c.C = 1;
    c.A = 0b00000001;
    c.mem[1] = 0x6A;

    driver(c, false);

    assert(c.A == 128 && c.C == 1);
    std::cout << "Accumulator: Passed" << std::endl;
}

void rorTest::Absolute() {
    CPU c(1, 0);

    // ror $2A30
    c.C = 1;
    c.mem[1] = 0x6E;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x2A30] == 128 && c.C == 1);
    std::cout << "Absolute: Passed" << std::endl;
}

void rorTest::ZeroPageX() {
    CPU c(1, 0);

    // ror $20,X
    c.X = 10;
    c.C = 1;
    c.mem[1] = 0x76;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 0b00000001;

    driver(c, false);

    assert(c.mem[0x2A] == 128 && c.C == 1);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void rorTest::AbsoluteX() {
    CPU c(1, 0);

    // ror $2C3D,X
    c.X = 2;
    c.C = 1;
    c.mem[1] = 0x7E;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 0b00000001;

    driver(c, false);
    assert(c.mem[0x2C3F] == 128 && c.C == 1);
    std::cout << "AbsoluteX: Passed" << std::endl;
}

// =========================
// returnTest
// =========================
void returnTest::rti() {
    CPU c(0x20, 0x4A);

    c.mem[0xFFFE] = 0x23;
    c.mem[0xFFFF] = 0xA4;
    // brk
    c.mem[0x4A20] = 0x00;
    
    // adc #$A
    c.mem[0xA423] = 0x69;
    c.mem[0xA424] = 10;
    // rti
    c.mem[0xA425] = 0x40;

    // adc #$1
    c.mem[0x4A22] = 0x69;
    c.mem[0x4A23] = 1;

    // Pass true to execute the break instruction properly
    driver(c, true);

    assert(c.A == 11);
    std::cout << "RTI: Passed" << std::endl;
}

void returnTest::rts() {
    CPU c(1, 0);

    // jsr
    c.mem[1] = 0x20;
    c.mem[2] = 0x39;
    c.mem[3] = 0x6E;

    // lda #$A
    c.mem[0x6E39] = 0xA9;
    c.mem[0x6E3A] = 0xA;
    // rts
    c.mem[0x6E3B] = 0x60;

    driver(c, false);

    assert(c.A == 10 && c.PC == 4);
    std::cout << "RTS: Passed" << std::endl;
}

// =========================
// sbcTest
// =========================
void sbcTest::IndirectX() {
    CPU c(1, 0);

    // sbc ($20,X)
    c.A = 0x80;
    c.C = 0;
    c.X = 2;
    c.mem[1] = 0xE1;
    c.mem[2] = 0x20;

    c.mem[0x22] = 0x23;
    c.mem[0x23] = 0x40;

    c.mem[0x4023] = 1;

    driver(c, false);

    assert(c.A == 0x7E);
    assert(c.V == 1);
    assert(c.C == 1);
    std::cout << "IndirectX: Passed" << std::endl;
}

void sbcTest::ZeroPage() {
    CPU c(1, 0);

    // sbc $30
    c.A = 2;
    c.C = 1;
    c.mem[1] = 0xE5;
    c.mem[2] = 0x30;

    c.mem[0x30] = 3;

    driver(c, false);

    assert(c.A == 0xFF);
    assert(c.V == 0);
    assert(c.C == 0);
    std::cout << "ZeroPage: Passed" << std::endl;
}

void sbcTest::Immediate() {
    CPU c(1, 0);

    // sbc #$1;
    c.A = 2;
    c.C = 1;
    c.mem[1] = 0xE9;
    c.mem[2] = 0x1;

    driver(c, false);

    assert(c.A == 1);
    assert(c.V == 0);
    assert(c.C == 1);
    std::cout << "Immediate: Passed" << std::endl;
}

void sbcTest::Absolute() {
    CPU c(1, 0);

    // sbc $2A30
    c.A = 0x7F;
    c.C = 1;
    c.mem[1] = 0xED;
    c.mem[2] = 0x30;
    c.mem[3] = 0x2A;

    c.mem[0x2A30] = 0xFF;

    driver(c, false);

    assert(c.A == 0x80);
    assert(c.V == 1);
    assert(c.C == 0);
    assert(c.N == 1);
    assert(c.Z == 0);
    std::cout << "Absolute: Passed" << std::endl;
}

void sbcTest::IndirectY() {
    CPU c(1, 0);

    // sbc ($30), Y
    c.A = 0x7F;
    c.C = 0;
    c.Y = 5;
    c.mem[1] = 0xF1;
    c.mem[2] = 0x30;

    c.mem[0x30] = 0xD3;
    c.mem[0x31] = 0x34;

    c.mem[0x34D8] = 0xFF;

    driver(c, false);

    assert(c.A == 0x7F);
    assert(c.V == 0);
    assert(c.C == 0);
    assert(c.N == 0);
    std::cout << "IndirectY: Passed" << std::endl;
}

void sbcTest::ZeroPageX() {
    CPU c(1, 0);

    // sbc $20,X
    c.A = 10;
    c.C = 1;
    c.X = 10;
    c.mem[1] = 0xF5;
    c.mem[2] = 0x20;

    c.mem[0x2A] = 1;

    driver(c, false);
    assert(c.A == 9);
    assert(c.V == 0);
    assert(c.C == 1);
    std::cout << "ZeroPageX: Passed" << std::endl;
}

void sbcTest::AbsoluteY() {
    CPU c(1, 0);

    // sbc $2C3D,Y
    c.A = 10;
    c.Y = 2;
    c.C = 0;
    c.mem[1] = 0xF9;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 1;

    driver(c, false);
    assert(c.A == 8);
    assert(c.V == 0);
    assert(c.C == 1);
    std::cout << "AbsoluteY: Passed" << std::endl;
}

void sbcTest::AbsoluteX() {
    CPU c(1, 0);

    // sbc $2C3D,X
    c.A = 5;
    c.C = 0;
    c.X = 2;
    c.mem[1] = 0xFD;
    c.mem[2] = 0x3D;
    c.mem[3] = 0x2C;

    c.mem[0x2C3F] = 10;

    driver(c, false);
    assert(c.A == 0xFA);
    assert(c.V == 0);
    assert(c.C == 0);
    std::cout << "AbsoluteX: Passed" << std::endl;
}
