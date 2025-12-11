#include "CPU.h"

CPU::CPU()
: A(0),
  X(0),
  Y(0),
  PC(0xFFFC),
  S(0xFD),
  P(0b00100100), // Negative, Overflow, Constant 1, Break, Decimal, Interrupt Disable, Zero, Carry
  cycle (0),
  current_cycle_time (std::chrono::high_resolution_clock::now())
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


    // Initialize the instruction set
    instructions[0xa9] = std::make_tuple(&CPU::lda, 2);
    instructions[0xa5] = std::make_tuple(&CPU::lda, 1);
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
}
