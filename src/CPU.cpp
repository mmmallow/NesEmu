#include "CPU.h"

CPU::CPU (u8 prg_start_low, u8 prg_start_high)
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
    /*
    mem[0xFFFC] = 1;
    mem[0xFFFD] = 0;
    
    // lda #$10
    mem[1] = 0xa9;
    mem[2] = 0x0a;

    // adc $10;
    mem[3] = 0x65;
    mem[4] = 0x0a;

    mem[10] = 5;
    */

    mem[0xFFFC] = prg_start_low;
    mem[0xFFFD] = prg_start_high;

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
    // LDA
    instructions[0xa1] = std::make_tuple(&CPU::lda, 0);
    instructions[0xa5] = std::make_tuple(&CPU::lda, 1);
    instructions[0xa9] = std::make_tuple(&CPU::lda, 2);
    instructions[0xad] = std::make_tuple(&CPU::lda, 3);
    instructions[0xb1] = std::make_tuple(&CPU::lda, 4);
    instructions[0xb5] = std::make_tuple(&CPU::lda, 5);
    instructions[0xb9] = std::make_tuple(&CPU::lda, 6);
    instructions[0xbd] = std::make_tuple(&CPU::lda, 7);

    // ADC
    instructions[0x61] = std::make_tuple(&CPU::adc, 0);
    instructions[0x65] = std::make_tuple(&CPU::adc, 1);
    instructions[0x69] = std::make_tuple(&CPU::adc, 2);
    instructions[0x6d] = std::make_tuple(&CPU::adc, 3);
    instructions[0x71] = std::make_tuple(&CPU::adc, 4);
    instructions[0x75] = std::make_tuple(&CPU::adc, 5);
    instructions[0x79] = std::make_tuple(&CPU::adc, 6);
    instructions[0x7d] = std::make_tuple(&CPU::adc, 7);

    // AND
    instructions[0x21] = std::make_tuple(&CPU::AND, 0);
    instructions[0x25] = std::make_tuple(&CPU::AND, 1);
    instructions[0x29] = std::make_tuple(&CPU::AND, 2);
    instructions[0x2d] = std::make_tuple(&CPU::AND, 3);
    instructions[0x31] = std::make_tuple(&CPU::AND, 4);
    instructions[0x35] = std::make_tuple(&CPU::AND, 5);
    instructions[0x39] = std::make_tuple(&CPU::AND, 6);
    instructions[0x3d] = std::make_tuple(&CPU::AND, 7);
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
    u8 zp_address;
    switch (mode) {
        // Indirect, X: 
        case 0:
            // lda ($70,X)
            // Load the contents of the address stored at $70+X and $70+1+X into A
            zp_address = mem[++PC];
            low_byte = mem[zp_address + X];
            high_byte = mem[zp_address + X + 1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = mem[value];
            advanceNClockCycles(6);
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
        // Absolute
        case 3:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = mem[value];
            advanceNClockCycles(4);
            break;
        // Indirect, Y
        case 4:
            // lda ($70),Y
            // Add Y to the address stored at $70 and $70+1, then store the contents of the resulting
            // address in A
            zp_address = mem[++PC];
            low_byte = mem[zp_address];
            high_byte = mem[zp_address+1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = mem[value+Y];
            advanceNClockCycles(5);
            break;
        // Zero Page, X
        case 5:
            value = mem[++PC];
            // Cast value to a u8 so values will wrap around to beginning of Zero Page
            A = mem[(u8)value + X];
            advanceNClockCycles(4);
            break;
        // Absolute, Y
        case 6:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = mem[value + Y];
            advanceNClockCycles(4);
            break;
        // Absolute, X
        case 7:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = mem[value + X];
            advanceNClockCycles(4);
            break;           
    }
    PC++;
    // Set Status flags
    if (A == 0) {
        Z = 1;
        N = 0;
    }
    // Largest bit is set
    else if (A >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}


void CPU::adc (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    u8 zp_address;
    switch (mode) {
        // Indirect, X
        case 0:
            // adc ($70,X)
            // Add the contents of the address stored at $70+X and $70+1+X with A and C
            zp_address = mem[++PC];
            low_byte = mem[zp_address + X];
            high_byte = mem[zp_address + X + 1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A + mem[value] + C;
            advanceNClockCycles(6);
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
        // Absolute
        case 3:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A + mem[value] + C;
            advanceNClockCycles(4);
            break;
        // Indirect, Y
        case 4:
            // adc ($70),Y
            // Add Y to the address stored at $70 and $70+1, then add the contents of the resulting
            // address to A and C
            zp_address = mem[++PC];
            low_byte = mem[zp_address];
            high_byte = mem[zp_address+1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A + mem[value+Y] + C;
            advanceNClockCycles(5);
            break;
        // Zero Page, X
        case 5:
            value = mem[++PC];
            // Cast value to a u8 so values will wrap around to beginning of Zero Page
            A = A + mem[(u8)value + X] + C;
            advanceNClockCycles(4);
            break;
        // Absolute, Y
        case 6:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A + mem[value + Y] + C;
            advanceNClockCycles(4);
            break;
        // Absolute, X
        case 7:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A + mem[value + X] + C;
            advanceNClockCycles(4);
            break;
    }
    PC++;
    if (A == 0) {
        Z = 1;
        N = 0;
        
    }
    else if (A >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }

    if ((A - value + C) + value + C >= 256) {
        V = 1;
        C = 1;
    }
    else {
        V = 0;
        C = 0;
    }
}

void CPU::AND (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    u8 zp_address;
    switch (mode) {
        // Indirect, X: 
        case 0:
            // and ($70,X)
            // AND the contents of the address stored at $70+X and $70+1+X with A
            zp_address = mem[++PC];
            low_byte = mem[zp_address + X];
            high_byte = mem[zp_address + X + 1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A & mem[value];
            advanceNClockCycles(6);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            A = A & mem[value];
            advanceNClockCycles(3);
            break;
        // Immediate
        case 2:
            value = mem[++PC];
            A = A & value;
            advanceNClockCycles(2);
            break;
        // Absolute
        case 3:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A & mem[value];
            advanceNClockCycles(4);
            break;
        // Indirect, Y
        case 4:
            // and ($70),Y
            // Add Y to the address stored at $70 and $70+1, then AND the contents of the resulting
            // address with A
            zp_address = mem[++PC];
            low_byte = mem[zp_address];
            high_byte = mem[zp_address+1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A & mem[value+Y];
            advanceNClockCycles(5);
            break;
        // Zero Page, X
        case 5:
            value = mem[++PC];
            // Cast value to a u8 so values will wrap around to beginning of Zero Page
            A = A & mem[(u8)value + X];
            advanceNClockCycles(4);
            break;
        // Absolute, Y
        case 6:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A & mem[value + Y];
            advanceNClockCycles(4);
            break;
        // Absolute, X
        case 7:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A & mem[value + X];
            advanceNClockCycles(4);
            break;           
    }
    PC++;
    // Set Status flags
    if (A == 0) {
        Z = 1;
        N = 0;
    }
    // Largest bit is set
    else if (A >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}
