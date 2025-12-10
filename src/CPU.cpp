#include "CPU.h"

CPU::CPU()
: A(0),
  X(0),
  Y(0),
  PC(0xFFFC),
  S(0xFD),
  P(0b00100100) // Negative, Overflow, Constant 1, Break, Decimal, Interrupt Disable, Zero, Carry
{ 
    // Test program to add two numbers together
    // Start at 0x0001
    mem[0xFFFC] = 1;
    mem[0xFFFD] = 0;
    
    // lda #$10
    mem[1] = 0xa9;
    mem[2] = 0x0a;

    init();
}

CPU::~CPU() { }

void CPU::init() {
    u16 high_byte = mem[0xFFFD];
    u16 low_byte = mem[0xFFFC];

    high_byte = high_byte << 8;

    u16 program_start = high_byte | low_byte;
    PC = program_start;
}

/******************* Instructions *******************/

void CPU::lda (u8 mode, u16 value) {
    switch (mode) {
        // Indirect, X
        case (0):
            A = mem[value + X];
        // Zero Page
        case (1):
            A = mem[value];
            break;
        // Immediate
        case (2):
            A = value;
            break;
    }
}
