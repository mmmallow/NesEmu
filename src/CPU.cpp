#include "CPU.h"

CPU::CPU (u8 prg_start_low, u8 prg_start_high)
: A(0),
  X(0),
  Y(0),
  PC(0xFFFC),
  S(0xFF),
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
    // BMI
    instructions[0x30] = std::make_tuple(&CPU::bmi, 0);
    // BNE
    instructions[0xD0] = std::make_tuple(&CPU::bne, 0);
    // BPL
    instructions[0x10] = std::make_tuple(&CPU::bpl, 0);
    // BVC
    instructions[0x50] = std::make_tuple(&CPU::bvc, 0);
    // BVS
    instructions[0x70] = std::make_tuple(&CPU::bvs, 0);

    // BIT
    instructions[0x24] = std::make_tuple(&CPU::bit, 0);
    instructions[0x2C] = std::make_tuple(&CPU::bit, 1);

    // BRK
    instructions[0x00] = std::make_tuple(&CPU::brk, 0);

    // CMP
    instructions[0xc1] = std::make_tuple(&CPU::cmp, 0);
    instructions[0xc5] = std::make_tuple(&CPU::cmp, 1);
    instructions[0xc9] = std::make_tuple(&CPU::cmp, 2);
    instructions[0xcd] = std::make_tuple(&CPU::cmp, 3);
    instructions[0xd1] = std::make_tuple(&CPU::cmp, 4);
    instructions[0xd5] = std::make_tuple(&CPU::cmp, 5);
    instructions[0xd9] = std::make_tuple(&CPU::cmp, 6);
    instructions[0xdd] = std::make_tuple(&CPU::cmp, 7);

    // CPX
    instructions[0xE0] = std::make_tuple(&CPU::cpx, 0);
    instructions[0xE4] = std::make_tuple(&CPU::cpx, 1);
    instructions[0xEC] = std::make_tuple(&CPU::cpx, 2);

    // CPY
    instructions[0xC0] = std::make_tuple(&CPU::cpy, 0);
    instructions[0xC4] = std::make_tuple(&CPU::cpy, 1);
    instructions[0xCC] = std::make_tuple(&CPU::cpy, 2);

    // DEC
    instructions[0xC6] = std::make_tuple(&CPU::dec, 0);
    instructions[0xCE] = std::make_tuple(&CPU::dec, 1);
    instructions[0xD6] = std::make_tuple(&CPU::dec, 2);
    instructions[0xDE] = std::make_tuple(&CPU::dec, 3);

    // DEX
    instructions[0xCA] = std::make_tuple(&CPU::dex, 0);
    
    // DEY
    instructions[0x88] = std::make_tuple(&CPU::dey, 0);

    // EOR
    instructions[0x41] = std::make_tuple(&CPU::eor, 0);
    instructions[0x45] = std::make_tuple(&CPU::eor, 1);
    instructions[0x49] = std::make_tuple(&CPU::eor, 2);
    instructions[0x4D] = std::make_tuple(&CPU::eor, 3);
    instructions[0x51] = std::make_tuple(&CPU::eor, 4);
    instructions[0x55] = std::make_tuple(&CPU::eor, 5);
    instructions[0x59] = std::make_tuple(&CPU::eor, 6);
    instructions[0x5D] = std::make_tuple(&CPU::eor, 7);

    // INC
    instructions[0xE6] = std::make_tuple(&CPU::inc, 0);
    instructions[0xEE] = std::make_tuple(&CPU::inc, 1);
    instructions[0xF6] = std::make_tuple(&CPU::inc, 2);
    instructions[0xFE] = std::make_tuple(&CPU::inc, 3);

    // INX
    instructions[0xE8] = std::make_tuple(&CPU::inx, 0);

    // INY
    instructions[0xC8] = std::make_tuple(&CPU::iny, 0);

    // JMP
    instructions[0x4C] = std::make_tuple(&CPU::jmp, 0);
    instructions[0x6C] = std::make_tuple(&CPU::jmp, 1);

    // JSR
    instructions[0x20] = std::make_tuple(&CPU::jsr, 0);

    // LDX
    instructions[0xA2] = std::make_tuple(&CPU::ldx, 0);
    instructions[0xA6] = std::make_tuple(&CPU::ldx, 1);
    instructions[0xAE] = std::make_tuple(&CPU::ldx, 2);
    instructions[0xB6] = std::make_tuple(&CPU::ldx, 3);
    instructions[0xBE] = std::make_tuple(&CPU::ldx, 4);

    // LDY
    instructions[0xA0] = std::make_tuple(&CPU::ldy, 0);
    instructions[0xA4] = std::make_tuple(&CPU::ldy, 1);
    instructions[0xAC] = std::make_tuple(&CPU::ldy, 2);
    instructions[0xB4] = std::make_tuple(&CPU::ldy, 3);
    instructions[0xBC] = std::make_tuple(&CPU::ldy, 4);

    // LSR
    instructions[0x46] = std::make_tuple(&CPU::lsr, 0);
    instructions[0x4A] = std::make_tuple(&CPU::lsr, 1);
    instructions[0x4E] = std::make_tuple(&CPU::lsr, 2);
    instructions[0x56] = std::make_tuple(&CPU::lsr, 3);
    instructions[0x5E] = std::make_tuple(&CPU::lsr, 4);

    // NOP
    instructions[0xEA] = std::make_tuple(&CPU::nop, 0);

    // OR 
    instructions[0x01] = std::make_tuple(&CPU::OR, 0);
    instructions[0x05] = std::make_tuple(&CPU::OR, 1);
    instructions[0x09] = std::make_tuple(&CPU::OR, 2);
    instructions[0x0d] = std::make_tuple(&CPU::OR, 3);
    instructions[0x11] = std::make_tuple(&CPU::OR, 4);
    instructions[0x15] = std::make_tuple(&CPU::OR, 5);
    instructions[0x19] = std::make_tuple(&CPU::OR, 6);
    instructions[0x1d] = std::make_tuple(&CPU::OR, 7);

    // PHA
    instructions[0x48] = std::make_tuple(&CPU::pha, 0);

    // PHP
    instructions[0x08] = std::make_tuple(&CPU::php, 0);

    // PLA
    instructions[0x68] = std::make_tuple(&CPU::pla, 0);

    // PLP
    instructions[0x28] = std::make_tuple(&CPU::plp, 0);
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

void CPU::pushStack (u8 item) {
    u16 stack_addr = 0x0100 | S;
    mem[stack_addr] = item;

    S--;
}

u8 CPU::pullStack() {
    S++;
    u16 stack_addr = 0x0100 | S;
    
    return mem[stack_addr];
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

void CPU::bit (u8 mode) {
    u16 low_byte;
    u16 high_byte;
    u16 address;
    u8 result;
    u8 value;
    switch (mode) {
        // Zero Page
        case 0:
            address = mem[++PC];
            value = mem[address];
            result = A & value;
            advanceNClockCycles(3);
            break;
        // Absolute
        case 1:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            address = high_byte | low_byte;
            value = mem[address];
            result = A & value;
            advanceNClockCycles(4);
            break;
    }

    if (result == 0)
        Z = 1;
    else
        Z = 0;

    // Bit shift 6 places so only last 2 bits are left
    u8 nv_values = value >> 6;
    if (nv_values == 0) {
        N = 0;
        V = 0;
    }
    else if (nv_values == 1) {
        N = 0;
        V = 1;
    }
    else if (nv_values == 2) {
        N = 1;
        V = 0;
    }
    else {
        N = 1;
        V = 1;
    }
}

void CPU::bmi (u8 mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (N == 1)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bne (u8 mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (Z == 0)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bpl (u8 mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (N == 0)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::brk (u8 mode) {
    // Only one mode, so no switch

    PC += 2;
    u8 high_byte = PC >> 8;
    u16 temp_low_byte = PC << 8;
    u8 low_byte = temp_low_byte >> 8;

    // Form Status flags into one word
    u8 status = 0;
    status = (status << 1) | N;
    status = (status << 1) | V;
    // Constant 1
    status = (status << 1) | 1;
    // Set B Flag
    status = (status << 1) | 1;
    // Set D Flag to 0 (not used in NES)
    status = (status << 1) | 0;
    // Set Interrupt Disable flag
    status = (status << 1) | 1;
    status = (status << 1) | Z;
    status = (status << 1) | C;

    pushStack(high_byte);
    pushStack(low_byte);
    pushStack(status);

    // Get new PC position
    u16 pc_high_byte = mem[0xFFFF];
    low_byte = mem[0xFFFE];
    pc_high_byte = pc_high_byte << 8;
    PC = pc_high_byte | low_byte;
    
    advanceNClockCycles(7);
}

void CPU::bvc (u8 mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (V == 0)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bvs (u8 mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (V == 1)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::clc (u8 mode) {
    // Only one mode
    C = 0;
    advanceNClockCycles(2);
}

void CPU::cli (u8 mode) {
    // Only one mode
    I = 0;
    advanceNClockCycles(2);
}

void CPU::clv (u8 mode) {
    V = 0;
    advanceNClockCycles(2);
}

void CPU::cpx (u8 mode) {
    u16 low_byte;
    u16 high_byte;
    u16 value;
    u8 result;
    switch (mode) {
        // Immediate
        case 0:
            result = mem[++PC];
            advanceNClockCycles(2);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            result = mem[value];
            advanceNClockCycles(3);
            break;
        // Absolute
        case 2:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = mem[value];
            advanceNClockCycles(4);
            break;
    }
    PC++;
    // Set flags
    if (X < result) {
        Z = 0;
        C = 0;
        if (result >= 128)
            N = 1;
        else
            N = 0;
    }
    else if (X == result) {
        N = 0;
        Z = 1;
        C = 1;
    }
    else if (X > result) {
        Z = 0;
        C = 1;
        if (result >= 128)
            N = 1;
        else
            N = 0;
    }
}

void CPU::cpy (u8 mode) {
    u16 low_byte;
    u16 high_byte;
    u16 value;
    u8 result;
    switch (mode) {
        // Immediate
        case 0:
            result = mem[++PC];
            advanceNClockCycles(2);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            result = mem[value];
            advanceNClockCycles(3);
            break;
        // Absolute
        case 2:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = mem[value];
            advanceNClockCycles(4);
            break;
    }
    PC++;
    // Set flags
    if (Y < result) {
        Z = 0;
        C = 0;
        if (result >= 128)
            N = 1;
        else
            N = 0;
    }
    else if (Y == result) {
        N = 0;
        Z = 1;
        C = 1;
    }
    else if (Y > result) {
        Z = 0;
        C = 1;
        if (result >= 128)
            N = 1;
        else
            N = 0;
    }
}

void CPU::dec (u8 mode) {
    u16 low_byte;
    u16 high_byte;
    u16 value;
    u8 zp_address;
    u8 result;
    switch (mode) {
        // Zero Page
        case 0:
            zp_address = mem[++PC];
            result = --mem[zp_address];
            advanceNClockCycles(5);
            break;
        // Absolute
        case 1:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = --mem[value];
            advanceNClockCycles(6);
            break;
        // Zero Page, X
        case 2:
            zp_address = mem[++PC];
            result = --mem[zp_address + X];
            advanceNClockCycles(6);
            break;
        // Absolute, X
        case 3:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = --mem[value + X];
            advanceNClockCycles(7);
            break;
    }

    PC++;

    if (result == 0) {
        Z = 1;
        N = 0;
    }
    else if (result >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::dex (u8 mode) {
    X--;
    advanceNClockCycles(2);

    PC++;

    if (X == 0) {
        Z = 1;
        N = 0;
    }
    else if (X >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::dey (u8 mode) {
    Y--;
    advanceNClockCycles(2);

    PC++;

    if (Y == 0) {
        Z = 1;
        N = 0;
    }
    else if (Y >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::eor (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    u8 zp_address;
    switch (mode) {
        // Indirect, X: 
        case 0:
            // eor ($70,X)
            // XOR the contents of the address stored at $70+X and $70+1+X with A
            zp_address = mem[++PC];
            low_byte = mem[zp_address + X];
            high_byte = mem[zp_address + X + 1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A ^ mem[value];
            advanceNClockCycles(6);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            A = A ^ mem[value];
            advanceNClockCycles(3);
            break;
        // Immediate
        case 2:
            value = mem[++PC];
            A = A ^ value;
            advanceNClockCycles(2);
            break;
        // Absolute
        case 3:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A ^ mem[value];
            advanceNClockCycles(4);
            break;
        // Indirect, Y
        case 4:
            // eor ($70),Y
            // Add Y to the address stored at $70 and $70+1, then XOR the contents of the resulting
            // address with A
            zp_address = mem[++PC];
            low_byte = mem[zp_address];
            high_byte = mem[zp_address+1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A ^ mem[value+Y];
            advanceNClockCycles(5);
            break;
        // Zero Page, X
        case 5:
            value = mem[++PC];
            // Cast value to a u8 so values will wrap around to beginning of Zero Page
            A = A ^ mem[(u8)value + X];
            advanceNClockCycles(4);
            break;
        // Absolute, Y
        case 6:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A ^ mem[value + Y];
            advanceNClockCycles(4);
            break;
        // Absolute, X
        case 7:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A ^ mem[value + X];
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

void CPU::inc (u8 mode) {
    u16 low_byte;
    u16 high_byte;
    u16 value;
    u8 zp_address;
    u8 result;
    switch (mode) {
        // Zero Page
        case 0:
            zp_address = mem[++PC];
            result = ++mem[zp_address];
            advanceNClockCycles(5);
            break;
        // Absolute
        case 1:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = ++mem[value];
            advanceNClockCycles(6);
            break;
        // Zero Page, X
        case 2:
            zp_address = mem[++PC];
            result = ++mem[zp_address + X];
            advanceNClockCycles(6);
            break;
        // Absolute, X
        case 3:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            result = ++mem[value + X];
            advanceNClockCycles(7);
            break;
    }

    PC++;

    if (result == 0) {
        Z = 1;
        N = 0;
    }
    else if (result >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::inx (u8 mode) {
    X++;
    advanceNClockCycles(2);

    PC++;

    if (X == 0) {
        Z = 1;
        N = 0;
    }
    else if (X >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::iny (u8 mode) {
    Y++;
    advanceNClockCycles(2);

    PC++;

    if (Y == 0) {
        Z = 1;
        N = 0;
    }
    else if (Y >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::jmp (u8 mode) {
    u16 low_byte;
    u16 high_byte;
    u16 value;
    switch (mode) {
        // Absolute
        case 0:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            PC = value;
            advanceNClockCycles(3);
            break;
        // Indirect
        case 1:
            // Set the PC to the address stored at the address given by the programmer
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            // Address stored at the address given
            low_byte = mem[value];
            high_byte = mem[value+1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            PC = value;
            advanceNClockCycles(5);
            break;
    }
}

void CPU::jsr (u8 mode) {
    u16 low_byte = mem[++PC];
    // Use PC+1, not ++PC, so that current PC will be return address of subroutine
    u16 high_byte = mem[PC+1];
    high_byte = high_byte << 8;
    u16 value = high_byte | low_byte;
    u8 pc_low_byte = PC;
    u8 pc_high_byte = PC >> 8;
    
    pushStack(pc_high_byte);
    pushStack(pc_low_byte);
    PC = value;
    advanceNClockCycles(6);
}

void CPU::ldx (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    u8 zp_address;
    switch (mode) {
        // Immediate
        case 0:
            value = mem[++PC];
            X = value;
            advanceNClockCycles(2);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            X = mem[value];
            advanceNClockCycles(3);
            break;
        // Absolute
        case 2:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            X = mem[value];
            advanceNClockCycles(4);
            break;
        // Zero Page, Y
        case 3:
            value = mem[++PC];
            X = mem[value + Y];
            advanceNClockCycles(4);
            break;
        // Absolute, Y
        case 4:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            X = mem[value + Y];
            advanceNClockCycles(4);
            break;         
    }
    PC++;
    // Set Status flags
    if (X == 0) {
        Z = 1;
        N = 0;
    }
    // Largest bit is set
    else if (X >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::ldy (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    u8 zp_address;
    switch (mode) {
        // Immediate
        case 0:
            value = mem[++PC];
            Y = value;
            advanceNClockCycles(2);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            Y = mem[value];
            advanceNClockCycles(3);
            break;
        // Absolute
        case 2:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            Y = mem[value];
            advanceNClockCycles(4);
            break;
        // Zero Page, X
        case 3:
            value = mem[++PC];
            Y = mem[value + X];
            advanceNClockCycles(4);
            break;
        // Absolute, X
        case 4:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            Y = mem[value + X];
            advanceNClockCycles(4);
            break;         
    }
    PC++;
    // Set Status flags
    if (Y == 0) {
        Z = 1;
        N = 0;
    }
    // Largest bit is set
    else if (Y >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::lsr (u8 mode) {
    // Least significant bit gets stored in Carry flag
    u8 lsb;
    u8 current;
    u16 value;
    u16 high_byte;
    u16 low_byte;
    u8 zp_address;
    switch (mode) {
        // Zero Page
        case 0:
            zp_address = mem[++PC];
            lsb = mem[zp_address] & 1;
            mem[zp_address] = mem[zp_address] >> 1;
            current = mem[zp_address];
            advanceNClockCycles(5);
            break;
        // Accumulator
        case 1:
            lsb = A & 1;
            A = A >> 1;
            current = A;
            advanceNClockCycles(2);
            break;
        // Absolute
        case 2:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            lsb = mem[value] & 1;
            mem[value] = mem[value] >> 1;
            current = mem[value];
            advanceNClockCycles(6);
            break;
        // Zero Page, X
        case 3:
            zp_address = mem[++PC];
            lsb = mem[zp_address + X] & 1;
            mem[zp_address + X] = mem[zp_address + X] >> 1;
            current = mem[zp_address + X];
            advanceNClockCycles(6);
            break;
        // Absolute, X
        case 4:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            lsb = mem[value + X] & 1;
            mem[value + X] = mem[value + X] >> 1;
            current = mem[value + X];
            advanceNClockCycles(7);
            break;
    }

    PC++;

    // Set status flags
    C = lsb;

    if (current == 0) {
        Z = 1;
    }
    else {
        Z = 0;
    }

    // Number will never be negative
    N = 0;
}

void CPU::nop (u8 mode) {
    advanceNClockCycles(2);
}

void CPU::OR (u8 mode) {
    u16 high_byte;
    u16 low_byte;
    u16 value;
    u8 zp_address;
    switch (mode) {
        // Indirect, X: 
        case 0:
            // or ($70,X)
            // OR the contents of the address stored at $70+X and $70+1+X with A
            zp_address = mem[++PC];
            low_byte = mem[zp_address + X];
            high_byte = mem[zp_address + X + 1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A | mem[value];
            advanceNClockCycles(6);
            break;
        // Zero Page
        case 1:
            value = mem[++PC];
            A = A | mem[value];
            advanceNClockCycles(3);
            break;
        // Immediate
        case 2:
            value = mem[++PC];
            A = A | value;
            advanceNClockCycles(2);
            break;
        // Absolute
        case 3:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A | mem[value];
            advanceNClockCycles(4);
            break;
        // Indirect, Y
        case 4:
            // or ($70),Y
            // Add Y to the address stored at $70 and $70+1, then OR the contents of the resulting
            // address with A
            zp_address = mem[++PC];
            low_byte = mem[zp_address];
            high_byte = mem[zp_address+1];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A | mem[value+Y];
            advanceNClockCycles(5);
            break;
        // Zero Page, X
        case 5:
            value = mem[++PC];
            // Cast value to a u8 so values will wrap around to beginning of Zero Page
            A = A | mem[(u8)value + X];
            advanceNClockCycles(4);
            break;
        // Absolute, Y
        case 6:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A | mem[value + Y];
            advanceNClockCycles(4);
            break;
        // Absolute, X
        case 7:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            value = high_byte | low_byte;
            A = A | mem[value + X];
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

void CPU::pha (u8 mode) {
    pushStack(A);
    advanceNClockCycles(3);
}

void CPU::php (u8 mode) {
    // Form Status flags into one word
    u8 status = 0;
    status = (status << 1) | N;
    status = (status << 1) | V;
    // Constant 1
    status = (status << 1) | 1;
    // Set B Flag
    status = (status << 1) | 1;
    // Set D Flag to 0 (not used in NES)
    status = (status << 1) | 0;
    // Set Interrupt Disable flag
    status = (status << 1) | 1;
    status = (status << 1) | Z;
    status = (status << 1) | C;

    pushStack(status);
    advanceNClockCycles(3);
}

void CPU::pla (u8 mode) {
    A = pullStack();

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
    advanceNClockCycles(4);
}

void CPU::plp (u8 mode) {
    u8 status = pullStack();
    C = status & 1;
    status = status >> 1;

    Z = status & 1;
    status = status >> 1;

    I = status & 1;
    status = status >> 1;

    // D Flag (not used so skip)
    status = status >> 1;

    B = status & 1;
    status = status >> 1;

    // Constant 1 (skip)
    status = status >> 1;

    V = status & 1;
    status = status >> 1;

    N = status & 1;
    status = status >> 1;

    advanceNClockCycles(4);
}
