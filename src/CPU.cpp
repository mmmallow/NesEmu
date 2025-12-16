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

    // ASL
    instructions[0x06] = std::make_tuple(&CPU::asl, 0);
    instructions[0x0A] = std::make_tuple(&CPU::asl, 1);
    instructions[0x0E] = std::make_tuple(&CPU::asl, 2);
    instructions[0x16] = std::make_tuple(&CPU::asl, 3);
    instructions[0x1E] = std::make_tuple(&CPU::asl, 4);

    // Branch
    // BCC
    instructions[0x90] = std::make_tuple(&CPU::bcc, 0);
    // BCS
    instructions[0xB0] = std::make_tuple(&CPU::bcs, 0);
    // BEQ
    instructions[0xF0] = std::make_tuple(&CPU::beq, 0);

    // CMP
    instructions[0xc1] = std::make_tuple(&CPU::cmp, 0);
    instructions[0xc5] = std::make_tuple(&CPU::cmp, 1);
    instructions[0xc9] = std::make_tuple(&CPU::cmp, 2);
    instructions[0xcd] = std::make_tuple(&CPU::cmp, 3);
    instructions[0xd1] = std::make_tuple(&CPU::cmp, 4);
    instructions[0xd5] = std::make_tuple(&CPU::cmp, 5);
    instructions[0xd9] = std::make_tuple(&CPU::cmp, 6);
    instructions[0xdd] = std::make_tuple(&CPU::cmp, 7);
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
    int previous_A = A;
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

    if (previous_A + value + C >= 256) {
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

void CPU::asl (u8 mode) {
    u8 previous;
    u8 current;
    u16 value;
    u16 high_byte;
    u16 low_byte;
    u8 zp_address;
    switch (mode) {
        // Zero Page
        case 0:
            zp_address = mem[++PC];
            previous = mem[zp_address];
            mem[zp_address] = mem[zp_address] << 1;
            current = mem[zp_address];
            advanceNClockCycles(5);
            break;
        // Accumulator
        case 1:
            previous = A;
            A = A << 1;
            current = A;
            advanceNClockCycles(2);
            break;
        // Absolute
        case 2:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            previous = mem[value];
            mem[value] = mem[value] << 1;
            current = mem[value];
            advanceNClockCycles(6);
            break;
        // Zero Page, X
        case 3:
            zp_address = mem[++PC];
            previous = mem[zp_address + X];
            mem[zp_address + X] = mem[zp_address + X] << 1;
            current = mem[zp_address + X];
            advanceNClockCycles(6);
            break;
        // Absolute, X
        case 4:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            previous = mem[value + X];
            mem[value + X] = mem[value + X] << 1;
            current = mem[value + X];
            advanceNClockCycles(7);
            break;
    }

    PC++;

    // Set status flags
    if (previous >= 128)
        C = 1;
    else
        C = 0;

    if (current == 0) {
        Z = 1;
        N = 0;
    }
    else if (current >= 128) {
        Z = 0;
        N = 1;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::bcc (u8 mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (C == 0)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bcs (u8 mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (C == 1)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::beq (u8 mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (Z == 1)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::cmp (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    u8 zp_address;
    u8 result;
    switch (mode) {
        // Indirect, X: 
        case 0:
            // cmp ($70,X)
            // compare the contents of the address stored at $70+X and $70+1+X with A
            zp_address = mem[++PC];
            low_byte = mem[zp_address + X];
            high_byte = mem[zp_address + X + 1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = mem[value];
            advanceNClockCycles(6);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            result = mem[value];
            advanceNClockCycles(3);
            break;
        // Immediate
        case 2:
            value = mem[++PC];
            result = value;
            advanceNClockCycles(2);
            break;
        // Absolute
        case 3:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = mem[value];
            advanceNClockCycles(4);
            break;
        // Indirect, Y
        case 4:
            // cmp ($70),Y
            // Add Y to the address stored at $70 and $70+1, then compare the contents of the resulting
            // address with A
            zp_address = mem[++PC];
            low_byte = mem[zp_address];
            high_byte = mem[zp_address+1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = mem[value+Y];
            advanceNClockCycles(5);
            break;
        // Zero Page, X
        case 5:
            value = mem[++PC];
            // Cast value to a u8 so values will wrap around to beginning of Zero Page
            result = mem[(u8)value + X];
            advanceNClockCycles(4);
            break;
        // Absolute, Y
        case 6:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = mem[value + Y];
            advanceNClockCycles(4);
            break;
        // Absolute, X
        case 7:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = mem[value + X];
            advanceNClockCycles(4);
            break;           
    }
    PC++;
    // Set flags
    if (A < result) {
        Z = 0;
        C = 0;
        if (result >= 128)
            N = 1;
        else
            N = 0;
    }
    else if (A == result) {
        N = 0;
        Z = 1;
        C = 1;
    }
    else if (A > result) {
        Z = 0;
        C = 1;
        if (result >= 128)
            N = 1;
        else
            N = 0;
    }
}
