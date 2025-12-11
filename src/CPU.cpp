#include "CPU.h"

CPU::CPU()
: A(0),
  X(0),
  Y(0),
  PC(0xFFFC),
  S(0xFD),
  N (0),
  V (0),
  B (0),
  I (1),
  Z (0),
  C (0),
  cycle (0)
{ 
    mem = new u8[0x10000];

    // Test program to add two numbers together
    // Start at 0x0001
    mem[0xFFFC] = 1;
    mem[0xFFFD] = 0;
    
    /*
    // lda #$10
    mem[1] = 0xa9;
    mem[2] = 0x0a;
    */

    /*
    // lda $10
    mem[1] = 0xa5;
    mem[2] = 0x0a;
    */

    /*
    // adc #$10
    A = 20;
    mem[1] = 0x69;
    mem[2] = 0x0a;
    */

    // adc $10;
    A = 20;
    mem[1] = 0x65;
    mem[2] = 0x0a;

    mem[10] = 5;

    init();
}

CPU::~CPU() {
    delete[] mem;
}

void CPU::init() {
    u16 high_byte = mem[0xFFFD];
    u16 low_byte = mem[0xFFFC];

    high_byte = high_byte << 8;

    u16 program_start = high_byte | low_byte;
    PC = program_start;


    // Initialize the instruction set
    instructions[0xa9] = std::make_tuple(&CPU::lda, 2);
    instructions[0xa5] = std::make_tuple(&CPU::lda, 1);

    instructions[0x65] = std::make_tuple(&CPU::adc, 1);
    instructions[0x69] = std::make_tuple(&CPU::adc, 2);
}

void CPU::advanceNClockCycles (int n) {
    for (int i = 0; i < n; ++i) {
        auto now = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(now - current_cycle_time).count();

        // Check if time is less than 601 ns (NES clock speed)
        if (time < 601) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(601-time));
        }

        cycle++;
        current_cycle_time = std::chrono::high_resolution_clock::now();
    }
}

/******************* Instructions *******************/

void CPU::lda (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    switch (mode) {
        // Indirect, X: 
        case 0:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = mem[value + X];
            advanceNClockCycles(5);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            A = mem[value];
            advanceNClockCycles(3);
            break;
        // Immediate
        case 2:
            value = mem[++PC];
            A = value;
            advanceNClockCycles(2);
            break;
    }
    PC++;
    // Set Status flags
    if (A == 0)
        Z = 1;
    // Largest bit is set
    else if (A >= 128)
        N = 1;
    else {
        Z = 0;
        N = 0;
    }
}


void CPU::adc (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    switch (mode) {
        case 0:
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            A = A + mem[value] + C;
            advanceNClockCycles(3);
            break;
        // Immediate
        case 2:
            value = mem[++PC];
            A = A + value + C;
            advanceNClockCycles(2);
            break;
    }
    PC++;
    if (A == 0)
        Z = 1;
    else if (A >= 128)
        N = 1;
    else if ((A - value + C) + value + C >= 256) {
        V = 1;
        C = 1;
    }
    else {
        Z = 0;
        N = 0;
        C = 0;
        V = 0;
    }
}
