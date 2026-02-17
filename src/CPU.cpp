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
    instructions[0xa1] = Instruction(&CPU::lda, IndirectX);
    instructions[0xa5] = Instruction(&CPU::lda, ZeroPage);
    instructions[0xa9] = Instruction(&CPU::lda, Immediate);
    instructions[0xad] = Instruction(&CPU::lda, Absolute);
    instructions[0xb1] = Instruction(&CPU::lda, IndirectY);
    instructions[0xb5] = Instruction(&CPU::lda, ZeroPageX);
    instructions[0xb9] = Instruction(&CPU::lda, AbsoluteY);
    instructions[0xbd] = Instruction(&CPU::lda, AbsoluteX);

    // ADC
    instructions[0x61] = Instruction(&CPU::adc, IndirectX);
    instructions[0x65] = Instruction(&CPU::adc, ZeroPage);
    instructions[0x69] = Instruction(&CPU::adc, Immediate);
    instructions[0x6d] = Instruction(&CPU::adc, Absolute);
    instructions[0x71] = Instruction(&CPU::adc, IndirectY);
    instructions[0x75] = Instruction(&CPU::adc, ZeroPageX);
    instructions[0x79] = Instruction(&CPU::adc, AbsoluteY);
    instructions[0x7d] = Instruction(&CPU::adc, AbsoluteX);

    // AND
    instructions[0x21] = Instruction(&CPU::AND, IndirectX);
    instructions[0x25] = Instruction(&CPU::AND, ZeroPage);
    instructions[0x29] = Instruction(&CPU::AND, Immediate);
    instructions[0x2d] = Instruction(&CPU::AND, Absolute);
    instructions[0x31] = Instruction(&CPU::AND, IndirectY);
    instructions[0x35] = Instruction(&CPU::AND, ZeroPageX);
    instructions[0x39] = Instruction(&CPU::AND, AbsoluteY);
    instructions[0x3d] = Instruction(&CPU::AND, AbsoluteX);

    // ASL
    instructions[0x06] = Instruction(&CPU::asl, ZeroPage);
    instructions[0x0A] = Instruction(&CPU::asl, Accumulator);
    instructions[0x0E] = Instruction(&CPU::asl, Absolute);
    instructions[0x16] = Instruction(&CPU::asl, ZeroPageX);
    instructions[0x1E] = Instruction(&CPU::asl, AbsoluteX);

    // Branch
    // BCC
    instructions[0x90] = Instruction(&CPU::bcc, Relative);
    // BCS
    instructions[0xB0] = Instruction(&CPU::bcs, Relative);
    // BEQ
    instructions[0xF0] = Instruction(&CPU::beq, Relative);
    // BMI
    instructions[0x30] = Instruction(&CPU::bmi, Relative);
    // BNE
    instructions[0xD0] = Instruction(&CPU::bne, Relative);
    // BPL
    instructions[0x10] = Instruction(&CPU::bpl, Relative);
    // BVC
    instructions[0x50] = Instruction(&CPU::bvc, Relative);
    // BVS
    instructions[0x70] = Instruction(&CPU::bvs, Relative);

    // BIT
    instructions[0x24] = Instruction(&CPU::bit, ZeroPage);
    instructions[0x2C] = Instruction(&CPU::bit, Absolute);

    // BRK
    instructions[0x00] = Instruction(&CPU::brk, Implied);

    // CLC
    instructions[0x18] = Instruction(&CPU::clc, Implied);

    // CLI
    instructions[0x58] = Instruction(&CPU::cli, Implied);

    // CLV
    instructions[0xB8] = Instruction(&CPU::clv, Implied);

    // CMP
    instructions[0xc1] = Instruction(&CPU::cmp, IndirectX);
    instructions[0xc5] = Instruction(&CPU::cmp, ZeroPage);
    instructions[0xc9] = Instruction(&CPU::cmp, Immediate);
    instructions[0xcd] = Instruction(&CPU::cmp, Absolute);
    instructions[0xd1] = Instruction(&CPU::cmp, IndirectY);
    instructions[0xd5] = Instruction(&CPU::cmp, ZeroPageX);
    instructions[0xd9] = Instruction(&CPU::cmp, AbsoluteY);
    instructions[0xdd] = Instruction(&CPU::cmp, AbsoluteY);

    // CPX
    instructions[0xE0] = Instruction(&CPU::cpx, Immediate);
    instructions[0xE4] = Instruction(&CPU::cpx, ZeroPage);
    instructions[0xEC] = Instruction(&CPU::cpx, Absolute);

    // CPY
    instructions[0xC0] = Instruction(&CPU::cpy, Immediate);
    instructions[0xC4] = Instruction(&CPU::cpy, ZeroPage);
    instructions[0xCC] = Instruction(&CPU::cpy, Absolute);

    // DEC
    instructions[0xC6] = Instruction(&CPU::dec, ZeroPage);
    instructions[0xCE] = Instruction(&CPU::dec, Absolute);
    instructions[0xD6] = Instruction(&CPU::dec, ZeroPageX);
    instructions[0xDE] = Instruction(&CPU::dec, AbsoluteX);

    // DEX
    instructions[0xCA] = Instruction(&CPU::dex, Implied);
    
    // DEY
    instructions[0x88] = Instruction(&CPU::dey, Implied);

    // EOR
    instructions[0x41] = Instruction(&CPU::eor, IndirectX);
    instructions[0x45] = Instruction(&CPU::eor, ZeroPage);
    instructions[0x49] = Instruction(&CPU::eor, Immediate);
    instructions[0x4D] = Instruction(&CPU::eor, Absolute);
    instructions[0x51] = Instruction(&CPU::eor, IndirectY);
    instructions[0x55] = Instruction(&CPU::eor, ZeroPageX);
    instructions[0x59] = Instruction(&CPU::eor, AbsoluteY);
    instructions[0x5D] = Instruction(&CPU::eor, AbsoluteX);

    // INC
    instructions[0xE6] = Instruction(&CPU::inc, ZeroPage);
    instructions[0xEE] = Instruction(&CPU::inc, Absolute);
    instructions[0xF6] = Instruction(&CPU::inc, ZeroPageX);
    instructions[0xFE] = Instruction(&CPU::inc, AbsoluteX);

    // INX
    instructions[0xE8] = Instruction(&CPU::inx, Implied);

    // INY
    instructions[0xC8] = Instruction(&CPU::iny, Implied);

    // JMP
    instructions[0x4C] = Instruction(&CPU::jmp, Absolute);
    instructions[0x6C] = Instruction(&CPU::jmp, Indirect);

    // JSR
    instructions[0x20] = Instruction(&CPU::jsr, Absolute);

    // LDX
    instructions[0xA2] = Instruction(&CPU::ldx, Immediate);
    instructions[0xA6] = Instruction(&CPU::ldx, ZeroPage);
    instructions[0xAE] = Instruction(&CPU::ldx, Absolute);
    instructions[0xB6] = Instruction(&CPU::ldx, ZeroPageY);
    instructions[0xBE] = Instruction(&CPU::ldx, AbsoluteY);

    // LDY
    instructions[0xA0] = Instruction(&CPU::ldy, Immediate);
    instructions[0xA4] = Instruction(&CPU::ldy, ZeroPage);
    instructions[0xAC] = Instruction(&CPU::ldy, Absolute);
    instructions[0xB4] = Instruction(&CPU::ldy, ZeroPageX);
    instructions[0xBC] = Instruction(&CPU::ldy, AbsoluteX);

    // LSR
    instructions[0x46] = Instruction(&CPU::lsr, ZeroPage);
    instructions[0x4A] = Instruction(&CPU::lsr, Accumulator);
    instructions[0x4E] = Instruction(&CPU::lsr, Absolute);
    instructions[0x56] = Instruction(&CPU::lsr, ZeroPageX);
    instructions[0x5E] = Instruction(&CPU::lsr, AbsoluteX);

    // NOP
    instructions[0xEA] = Instruction(&CPU::nop, Implied);

    // OR 
    instructions[0x01] = Instruction(&CPU::OR, IndirectX);
    instructions[0x05] = Instruction(&CPU::OR, ZeroPage);
    instructions[0x09] = Instruction(&CPU::OR, Immediate);
    instructions[0x0d] = Instruction(&CPU::OR, Absolute);
    instructions[0x11] = Instruction(&CPU::OR, IndirectY);
    instructions[0x15] = Instruction(&CPU::OR, ZeroPageX);
    instructions[0x19] = Instruction(&CPU::OR, AbsoluteY);
    instructions[0x1d] = Instruction(&CPU::OR, AbsoluteX);

    // PHA
    instructions[0x48] = Instruction(&CPU::pha, Implied);

    // PHP
    instructions[0x08] = Instruction(&CPU::php, Implied);

    // PLA
    instructions[0x68] = Instruction(&CPU::pla, Implied);

    // PLP
    instructions[0x28] = Instruction(&CPU::plp, Implied);

    // ROL
    instructions[0x26] = Instruction(&CPU::rol, ZeroPage);
    instructions[0x2A] = Instruction(&CPU::rol, Accumulator);
    instructions[0x2E] = Instruction(&CPU::rol, Absolute);
    instructions[0x36] = Instruction(&CPU::rol, ZeroPageX);
    instructions[0x3E] = Instruction(&CPU::rol, AbsoluteX);

    // ROR
    instructions[0x66] = Instruction(&CPU::ror, ZeroPage);
    instructions[0x6A] = Instruction(&CPU::ror, Accumulator);
    instructions[0x6E] = Instruction(&CPU::ror, Absolute);
    instructions[0x76] = Instruction(&CPU::ror, ZeroPageX);
    instructions[0x7E] = Instruction(&CPU::ror, AbsoluteX);

    // RTI
    instructions[0x40] = Instruction(&CPU::rti, Implied);

    // RTS
    instructions[0x60] = Instruction(&CPU::rts, Implied);

    // SBC
    instructions[0xE1] = Instruction(&CPU::sbc, IndirectX);
    instructions[0xE5] = Instruction(&CPU::sbc, ZeroPage);
    instructions[0xE9] = Instruction(&CPU::sbc, Immediate);
    instructions[0xEd] = Instruction(&CPU::sbc, Absolute);
    instructions[0xF1] = Instruction(&CPU::sbc, IndirectY);
    instructions[0xF5] = Instruction(&CPU::sbc, ZeroPageX);
    instructions[0xF9] = Instruction(&CPU::sbc, AbsoluteY);
    instructions[0xFd] = Instruction(&CPU::sbc, AbsoluteX);
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

u8* CPU::fetch() {
    // Get Addressing Mode
    u8 op_code = mem[PC];
    AddressingMode mode = instructions[op_code].mode;
    u8* value;
    switch (mode) {
        u16 high_byte;
        u16 low_byte;
        u8 zp_address;
        u16 address;
        case IndirectX:
            // Fetch contents of address stored at $## + X and $## + X + 1
            zp_address = mem[++PC];
            low_byte = mem[zp_address + X];
            high_byte = mem[zp_address + X + 1];
            high_byte = high_byte << 8;
            address = high_byte | low_byte;
            value = &(mem[address]);
            break;
        case ZeroPage:
            // Fetch contents of address stored in first 256 bytes
            zp_address = mem[++PC];
            value = &(mem[zp_address]);
            break;
        case Immediate:
            // Return value given
            value = &(mem[++PC]);
            break;
        case Absolute:
            // Fetch contents of address stored at $XXXX
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            address = high_byte | low_byte;
            value = &(mem[address]);
            break;
        case IndirectY:
            zp_address = mem[++PC];
            low_byte = mem[zp_address];
            high_byte = mem[zp_address+1];
            high_byte = high_byte << 8;
            address = high_byte | low_byte;
            value = &(mem[address + Y]);
            break;
        case ZeroPageX:
            zp_address = mem[++PC];
            value = &(mem[zp_address + X]);
            break;
        case ZeroPageY:
            zp_address = mem[++PC];
            value = &(mem[zp_address + Y]);
            break;
        case AbsoluteY:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            address = high_byte | low_byte;
            value = &(mem[address + Y]);
            break;
        case AbsoluteX:
            low_byte = mem[++PC];
            high_byte = mem[++PC];
            high_byte = high_byte << 8;
            address = high_byte | low_byte;
            value = &(mem[address + X]);
            break;
    }

    return value;
}

/******************* Instructions *******************/

/*****************
* TODO:
* - Go through methods and replaces switch statements with call to fetch
* - Find number of clock cycles each addressing mode takes when fetching
*****************/

void CPU::lda (AddressingMode mode) {
    u8* value = fetch();
    A = *value;
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


void CPU::adc (AddressingMode mode) {
    int previous_A = A;

    u8* value = fetch();
    A = *value + A + C;

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

    int int_result = previous_A + *value + C;
    // Check if outside of unsigned range
    if (int_result >= 256)
        C = 1;
    else
        C = 0;

    // Check if outside signed range
    if (int_result < -128 || int_result > 127)
        V = 1;
    else
        V = 0;
}

void CPU::AND (AddressingMode mode) {
    u8* value = fetch();
    A = A & *value;

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

void CPU::asl (AddressingMode mode) {
    u8* value = fetch();
    u8 previous = *value;
    *value = *value << 1;

    PC++;

    // Set status flags
    if (previous >= 128)
        C = 1;
    else
        C = 0;

    if (*value == 0) {
        Z = 1;
        N = 0;
    }
    else if (*value >= 128) {
        Z = 0;
        N = 1;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::bcc (AddressingMode mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (C == 0)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bcs (AddressingMode mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (C == 1)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::beq (AddressingMode mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (Z == 1)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::cmp (AddressingMode mode) {
    u8* result = fetch();

    PC++;
    // Set flags
    if (A < *result) {
        Z = 0;
        C = 0;
        if (*result >= 128)
            N = 1;
        else
            N = 0;
    }
    else if (A == *result) {
        N = 0;
        Z = 1;
        C = 1;
    }
    else if (A > *result) {
        Z = 0;
        C = 1;
        if (*result >= 128)
            N = 1;
        else
            N = 0;
    }
}

void CPU::bit (AddressingMode mode) {
    u8* value = fetch();
    u8 result = *value & A;

    PC++;
    
    if (result == 0)
        Z = 1;
    else
        Z = 0;

    // Bit shift 6 places so only last 2 bits are left
    u8 nv_values = *value >> 6;
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

void CPU::bmi (AddressingMode mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (N == 1)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bne (AddressingMode mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (Z == 0)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bpl (AddressingMode mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (N == 0)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::brk (AddressingMode mode) {
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

void CPU::bvc (AddressingMode mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (V == 0)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bvs (AddressingMode mode) {
    // Only one mode, so don't need switch statement
    
    // Cast the offset to a char because it is a signed integer 
    char offset = mem[PC+1];
    if (V == 1)
        PC += offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::clc (AddressingMode mode) {
    // Only one mode
    C = 0;
    PC++;
    advanceNClockCycles(2);
}

void CPU::cli (AddressingMode mode) {
    // Only one mode
    I = 0;
    PC++;
    advanceNClockCycles(2);
}

void CPU::clv (AddressingMode mode) {
    V = 0;
    PC++;
    advanceNClockCycles(2);
}

void CPU::cpx (AddressingMode mode) {
    u8* result = fetch();

    PC++;
    // Set flags
    if (X < *result) {
        Z = 0;
        C = 0;
        if (*result >= 128)
            N = 1;
        else
            N = 0;
    }
    else if (X == *result) {
        N = 0;
        Z = 1;
        C = 1;
    }
    else if (X > *result) {
        Z = 0;
        C = 1;
        if (*result >= 128)
            N = 1;
        else
            N = 0;
    }
}

void CPU::cpy (AddressingMode mode) {
    u8* result = fetch();

    PC++;
    // Set flags
    if (Y < *result) {
        Z = 0;
        C = 0;
        if (*result >= 128)
            N = 1;
        else
            N = 0;
    }
    else if (Y == *result) {
        N = 0;
        Z = 1;
        C = 1;
    }
    else if (Y > *result) {
        Z = 0;
        C = 1;
        if (*result >= 128)
            N = 1;
        else
            N = 0;
    }
}

void CPU::dec (AddressingMode mode) {
    u8* result = fetch();
    *result = (*result)--;

    PC++;

    if (*result == 0) {
        Z = 1;
        N = 0;
    }
    else if (*result >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::dex (AddressingMode mode) {
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

void CPU::dey (AddressingMode mode) {
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

void CPU::eor (AddressingMode mode) {
    u8* value = fetch();
    A = A ^ *value;

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

void CPU::inc (AddressingMode mode) {
    u8* result = fetch();
    *result = (*result)++;

    PC++;

    if (*result == 0) {
        Z = 1;
        N = 0;
    }
    else if (*result >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::inx (AddressingMode mode) {
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

void CPU::iny (AddressingMode mode) {
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

void CPU::jmp (AddressingMode mode) {
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

void CPU::jsr (AddressingMode mode) {
    u16 low_byte = mem[++PC];
    u16 high_byte = mem[++PC];
    high_byte = high_byte << 8;
    u16 value = high_byte | low_byte;
    // Return address
    PC++;
    u8 pc_low_byte = PC;
    u8 pc_high_byte = PC >> 8;
    
    pushStack(pc_high_byte);
    pushStack(pc_low_byte);
    PC = value;
    advanceNClockCycles(6);
}

void CPU::ldx (AddressingMode mode) {
    u8* result = fetch();
    X = *result;

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

void CPU::ldy (AddressingMode mode) {
    u8* result = fetch();
    Y = *result;

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

void CPU::lsr (AddressingMode mode) {
    // Least significant bit gets stored in Carry flag
    u8 lsb;
    u8* result = fetch();
    lsb = *result & 1;
    *result = *result >> 1;

    PC++;

    // Set status flags
    C = lsb;

    if (*result == 0) {
        Z = 1;
    }
    else {
        Z = 0;
    }

    // Number will never be negative
    N = 0;
}

void CPU::nop (AddressingMode mode) {
    advanceNClockCycles(2);
}

void CPU::OR (AddressingMode mode) {
    u8* result = fetch();
    A = A | *result;
    
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

void CPU::pha (AddressingMode mode) {
    pushStack(A);
    advanceNClockCycles(3);
}

void CPU::php (AddressingMode mode) {
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

void CPU::pla (AddressingMode mode) {
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

void CPU::plp (AddressingMode mode) {
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

void CPU::rol (AddressingMode mode) {
    u8* value = fetch();

    PC++;

    // Rotate the value left 1
    u8 new_c = *value >= 128 ? 1 : 0;
    *value = *value << 1;
    *value |= C;
    C = new_c;

    if (*value == 0) {
        Z = 1;
        N = 0;
    }
    else if (*value >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::ror (AddressingMode mode) {
    u8* value = fetch();

    PC++;

    // Rotate the value right 1
    u8 new_c = *value & 1;
    *value = *value >> 1;
    *value |= (C << 7);
    C = new_c;

    if (*value == 0) {
        Z = 1;
        N = 0;
    }
    else if (*value >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::rti (AddressingMode mode) {
    u8 status = pullStack();
    u8 low_byte = pullStack();
    u16 high_byte = pullStack();
    PC = (high_byte << 8) | low_byte;

    C = status & 1;
    status = status >> 1;

    Z = status & 1;
    status = status >> 1;

    I = status & 1;
    status = status >> 1;

    // D Flag (not used so skip)
    status = status >> 1;

    // Set B to 0 on rti
    B = 0;
    status = status >> 1;

    // Constant 1 (skip)
    status = status >> 1;

    V = status & 1;
    status = status >> 1;

    N = status & 1;
    status = status >> 1;

    advanceNClockCycles(6);
}

void CPU::rts (AddressingMode mode) {
    u8 low_byte = pullStack();
    u16 high_byte = pullStack();
    PC = (high_byte << 8) | low_byte;

    advanceNClockCycles(6);
}

void CPU::sbc (AddressingMode mode) {
    u8* result = fetch();
    int previous_A = A;
    // Invert bits of result
    u16 value = (u16)*result ^ 0x00FF;
    u16 temp = (u16)A + value + (u16)C;

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

    C = temp & 0xFF00;
    V = (temp ^ (u16)A) & (temp ^ value) & 0x0080;

    A = temp & 0x00FF;
}

