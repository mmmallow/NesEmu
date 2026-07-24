#include "CPU.h"
#include "Bus.h"

CPU::CPU ()
: A(0),
  X(0),
  Y(0),
  PC(0),
  S(0xFD),
  N (0),
  V (0),
  B (0),
  I (1),
  Z (0),
  C (0),
  cycle (0)
{
    buildInstructionTable();
}

void CPU::reset () {
    A = 0;
    X = 0;
    Y = 0;
    S = 0xFD;
    N = 0;
    V = 0;
    B = 0;
    I = 1;
    Z = 0;
    C = 0;
    cycle = 0;

    u16 low_byte = bus->cpuRead(0xFFFC);
    u16 high_byte = bus->cpuRead(0xFFFD);
    PC = (high_byte << 8) | low_byte;
}

u8 CPU::step () {
    u8 op_code = bus->cpuRead(PC);
    Instruction instr = instructions[op_code];
    (this->*instr.instruction)();
    return instr.cycles;
}

void CPU::nmi () {
    u8 high_byte = PC >> 8;
    u8 low_byte = PC & 0xFF;

    // Same status-byte layout as BRK/PHP, except the B flag is left clear --
    // that's how software (BRK) vs hardware (NMI/IRQ) interrupts are told
    // apart by code that inspects the pushed status byte.
    u8 status = 0;
    status = (status << 1) | N;
    status = (status << 1) | V;
    status = (status << 1) | 1;
    status = (status << 1) | 0;
    status = (status << 1) | 0;
    status = (status << 1) | I;
    status = (status << 1) | Z;
    status = (status << 1) | C;

    pushStack(high_byte);
    pushStack(low_byte);
    pushStack(status);

    I = 1;

    u16 vec_low_byte = bus->cpuRead(0xFFFA);
    u16 vec_high_byte = bus->cpuRead(0xFFFB);
    PC = (vec_high_byte << 8) | vec_low_byte;

    advanceNClockCycles(7);
}

void CPU::buildInstructionTable() {
    // Initialize the instruction set. Cycle counts are the standard base
    // costs from https://www.masswerk.at/6502/6502_instruction_set.html,
    // ignoring the +1 page-cross / +1-taken-branch nuances.
    // LDA
    instructions[0xa1] = Instruction(&CPU::lda, IndirectX, 6);
    instructions[0xa5] = Instruction(&CPU::lda, ZeroPage, 3);
    instructions[0xa9] = Instruction(&CPU::lda, Immediate, 2);
    instructions[0xad] = Instruction(&CPU::lda, Absolute, 4);
    instructions[0xb1] = Instruction(&CPU::lda, IndirectY, 5);
    instructions[0xb5] = Instruction(&CPU::lda, ZeroPageX, 4);
    instructions[0xb9] = Instruction(&CPU::lda, AbsoluteY, 4);
    instructions[0xbd] = Instruction(&CPU::lda, AbsoluteX, 4);

    // ADC
    instructions[0x61] = Instruction(&CPU::adc, IndirectX, 6);
    instructions[0x65] = Instruction(&CPU::adc, ZeroPage, 3);
    instructions[0x69] = Instruction(&CPU::adc, Immediate, 2);
    instructions[0x6d] = Instruction(&CPU::adc, Absolute, 4);
    instructions[0x71] = Instruction(&CPU::adc, IndirectY, 5);
    instructions[0x75] = Instruction(&CPU::adc, ZeroPageX, 4);
    instructions[0x79] = Instruction(&CPU::adc, AbsoluteY, 4);
    instructions[0x7d] = Instruction(&CPU::adc, AbsoluteX, 4);

    // AND
    instructions[0x21] = Instruction(&CPU::AND, IndirectX, 6);
    instructions[0x25] = Instruction(&CPU::AND, ZeroPage, 3);
    instructions[0x29] = Instruction(&CPU::AND, Immediate, 2);
    instructions[0x2d] = Instruction(&CPU::AND, Absolute, 4);
    instructions[0x31] = Instruction(&CPU::AND, IndirectY, 5);
    instructions[0x35] = Instruction(&CPU::AND, ZeroPageX, 4);
    instructions[0x39] = Instruction(&CPU::AND, AbsoluteY, 4);
    instructions[0x3d] = Instruction(&CPU::AND, AbsoluteX, 4);

    // ASL
    instructions[0x06] = Instruction(&CPU::asl, ZeroPage, 5);
    instructions[0x0A] = Instruction(&CPU::asl, Accumulator, 2);
    instructions[0x0E] = Instruction(&CPU::asl, Absolute, 6);
    instructions[0x16] = Instruction(&CPU::asl, ZeroPageX, 6);
    instructions[0x1E] = Instruction(&CPU::asl, AbsoluteX, 7);

    // Branch
    // BCC
    instructions[0x90] = Instruction(&CPU::bcc, Relative, 2);
    // BCS
    instructions[0xB0] = Instruction(&CPU::bcs, Relative, 2);
    // BEQ
    instructions[0xF0] = Instruction(&CPU::beq, Relative, 2);
    // BMI
    instructions[0x30] = Instruction(&CPU::bmi, Relative, 2);
    // BNE
    instructions[0xD0] = Instruction(&CPU::bne, Relative, 2);
    // BPL
    instructions[0x10] = Instruction(&CPU::bpl, Relative, 2);
    // BVC
    instructions[0x50] = Instruction(&CPU::bvc, Relative, 2);
    // BVS
    instructions[0x70] = Instruction(&CPU::bvs, Relative, 2);

    // BIT
    instructions[0x24] = Instruction(&CPU::bit, ZeroPage, 3);
    instructions[0x2C] = Instruction(&CPU::bit, Absolute, 4);

    // BRK
    instructions[0x00] = Instruction(&CPU::brk, Implied, 7);

    // CLC
    instructions[0x18] = Instruction(&CPU::clc, Implied, 2);

    // CLI
    instructions[0x58] = Instruction(&CPU::cli, Implied, 2);

    // CLV
    instructions[0xB8] = Instruction(&CPU::clv, Implied, 2);

    // CMP
    instructions[0xc1] = Instruction(&CPU::cmp, IndirectX, 6);
    instructions[0xc5] = Instruction(&CPU::cmp, ZeroPage, 3);
    instructions[0xc9] = Instruction(&CPU::cmp, Immediate, 2);
    instructions[0xcd] = Instruction(&CPU::cmp, Absolute, 4);
    instructions[0xd1] = Instruction(&CPU::cmp, IndirectY, 5);
    instructions[0xd5] = Instruction(&CPU::cmp, ZeroPageX, 4);
    instructions[0xd9] = Instruction(&CPU::cmp, AbsoluteY, 4);
    instructions[0xdd] = Instruction(&CPU::cmp, AbsoluteX, 4);

    // CPX
    instructions[0xE0] = Instruction(&CPU::cpx, Immediate, 2);
    instructions[0xE4] = Instruction(&CPU::cpx, ZeroPage, 3);
    instructions[0xEC] = Instruction(&CPU::cpx, Absolute, 4);

    // CPY
    instructions[0xC0] = Instruction(&CPU::cpy, Immediate, 2);
    instructions[0xC4] = Instruction(&CPU::cpy, ZeroPage, 3);
    instructions[0xCC] = Instruction(&CPU::cpy, Absolute, 4);

    // DEC
    instructions[0xC6] = Instruction(&CPU::dec, ZeroPage, 5);
    instructions[0xCE] = Instruction(&CPU::dec, Absolute, 6);
    instructions[0xD6] = Instruction(&CPU::dec, ZeroPageX, 6);
    instructions[0xDE] = Instruction(&CPU::dec, AbsoluteX, 7);

    // DEX
    instructions[0xCA] = Instruction(&CPU::dex, Implied, 2);

    // DEY
    instructions[0x88] = Instruction(&CPU::dey, Implied, 2);

    // EOR
    instructions[0x41] = Instruction(&CPU::eor, IndirectX, 6);
    instructions[0x45] = Instruction(&CPU::eor, ZeroPage, 3);
    instructions[0x49] = Instruction(&CPU::eor, Immediate, 2);
    instructions[0x4D] = Instruction(&CPU::eor, Absolute, 4);
    instructions[0x51] = Instruction(&CPU::eor, IndirectY, 5);
    instructions[0x55] = Instruction(&CPU::eor, ZeroPageX, 4);
    instructions[0x59] = Instruction(&CPU::eor, AbsoluteY, 4);
    instructions[0x5D] = Instruction(&CPU::eor, AbsoluteX, 4);

    // INC
    instructions[0xE6] = Instruction(&CPU::inc, ZeroPage, 5);
    instructions[0xEE] = Instruction(&CPU::inc, Absolute, 6);
    instructions[0xF6] = Instruction(&CPU::inc, ZeroPageX, 6);
    instructions[0xFE] = Instruction(&CPU::inc, AbsoluteX, 7);

    // INX
    instructions[0xE8] = Instruction(&CPU::inx, Implied, 2);

    // INY
    instructions[0xC8] = Instruction(&CPU::iny, Implied, 2);

    // JMP
    instructions[0x4C] = Instruction(&CPU::jmp, Absolute, 3);
    instructions[0x6C] = Instruction(&CPU::jmp, Indirect, 5);

    // JSR
    instructions[0x20] = Instruction(&CPU::jsr, Absolute, 6);

    // LDX
    instructions[0xA2] = Instruction(&CPU::ldx, Immediate, 2);
    instructions[0xA6] = Instruction(&CPU::ldx, ZeroPage, 3);
    instructions[0xAE] = Instruction(&CPU::ldx, Absolute, 4);
    instructions[0xB6] = Instruction(&CPU::ldx, ZeroPageY, 4);
    instructions[0xBE] = Instruction(&CPU::ldx, AbsoluteY, 4);

    // LDY
    instructions[0xA0] = Instruction(&CPU::ldy, Immediate, 2);
    instructions[0xA4] = Instruction(&CPU::ldy, ZeroPage, 3);
    instructions[0xAC] = Instruction(&CPU::ldy, Absolute, 4);
    instructions[0xB4] = Instruction(&CPU::ldy, ZeroPageX, 4);
    instructions[0xBC] = Instruction(&CPU::ldy, AbsoluteX, 4);

    // LSR
    instructions[0x46] = Instruction(&CPU::lsr, ZeroPage, 5);
    instructions[0x4A] = Instruction(&CPU::lsr, Accumulator, 2);
    instructions[0x4E] = Instruction(&CPU::lsr, Absolute, 6);
    instructions[0x56] = Instruction(&CPU::lsr, ZeroPageX, 6);
    instructions[0x5E] = Instruction(&CPU::lsr, AbsoluteX, 7);

    // NOP
    instructions[0xEA] = Instruction(&CPU::nop, Implied, 2);

    // OR
    instructions[0x01] = Instruction(&CPU::OR, IndirectX, 6);
    instructions[0x05] = Instruction(&CPU::OR, ZeroPage, 3);
    instructions[0x09] = Instruction(&CPU::OR, Immediate, 2);
    instructions[0x0d] = Instruction(&CPU::OR, Absolute, 4);
    instructions[0x11] = Instruction(&CPU::OR, IndirectY, 5);
    instructions[0x15] = Instruction(&CPU::OR, ZeroPageX, 4);
    instructions[0x19] = Instruction(&CPU::OR, AbsoluteY, 4);
    instructions[0x1d] = Instruction(&CPU::OR, AbsoluteX, 4);

    // PHA
    instructions[0x48] = Instruction(&CPU::pha, Implied, 3);

    // PHP
    instructions[0x08] = Instruction(&CPU::php, Implied, 3);

    // PLA
    instructions[0x68] = Instruction(&CPU::pla, Implied, 4);

    // PLP
    instructions[0x28] = Instruction(&CPU::plp, Implied, 4);

    // ROL
    instructions[0x26] = Instruction(&CPU::rol, ZeroPage, 5);
    instructions[0x2A] = Instruction(&CPU::rol, Accumulator, 2);
    instructions[0x2E] = Instruction(&CPU::rol, Absolute, 6);
    instructions[0x36] = Instruction(&CPU::rol, ZeroPageX, 6);
    instructions[0x3E] = Instruction(&CPU::rol, AbsoluteX, 7);

    // ROR
    instructions[0x66] = Instruction(&CPU::ror, ZeroPage, 5);
    instructions[0x6A] = Instruction(&CPU::ror, Accumulator, 2);
    instructions[0x6E] = Instruction(&CPU::ror, Absolute, 6);
    instructions[0x76] = Instruction(&CPU::ror, ZeroPageX, 6);
    instructions[0x7E] = Instruction(&CPU::ror, AbsoluteX, 7);

    // RTI
    instructions[0x40] = Instruction(&CPU::rti, Implied, 6);

    // RTS
    instructions[0x60] = Instruction(&CPU::rts, Implied, 6);

    // SBC
    instructions[0xE1] = Instruction(&CPU::sbc, IndirectX, 6);
    instructions[0xE5] = Instruction(&CPU::sbc, ZeroPage, 3);
    instructions[0xE9] = Instruction(&CPU::sbc, Immediate, 2);
    instructions[0xEd] = Instruction(&CPU::sbc, Absolute, 4);
    instructions[0xF1] = Instruction(&CPU::sbc, IndirectY, 5);
    instructions[0xF5] = Instruction(&CPU::sbc, ZeroPageX, 4);
    instructions[0xF9] = Instruction(&CPU::sbc, AbsoluteY, 4);
    instructions[0xFd] = Instruction(&CPU::sbc, AbsoluteX, 4);

    // SEC
    instructions[0x38] = Instruction(&CPU::sec, Implied, 2);

    // SEI
    instructions[0x78] = Instruction(&CPU::sei, Implied, 2);

    // STA
    instructions[0x81] = Instruction(&CPU::sta, IndirectX, 6);
    instructions[0x85] = Instruction(&CPU::sta, ZeroPage, 3);
    instructions[0x8D] = Instruction(&CPU::sta, Absolute, 4);
    instructions[0x91] = Instruction(&CPU::sta, IndirectY, 6);
    instructions[0x95] = Instruction(&CPU::sta, ZeroPageX, 4);
    instructions[0x99] = Instruction(&CPU::sta, AbsoluteY, 5);
    instructions[0x9D] = Instruction(&CPU::sta, AbsoluteX, 5);

    // STX
    instructions[0x86] = Instruction(&CPU::stx, ZeroPage, 3);
    instructions[0x8E] = Instruction(&CPU::stx, Absolute, 4);
    instructions[0x96] = Instruction(&CPU::stx, ZeroPageY, 4);

    // STY
    instructions[0x84] = Instruction(&CPU::sty, ZeroPage, 3);
    instructions[0x8C] = Instruction(&CPU::sty, Absolute, 4);
    instructions[0x94] = Instruction(&CPU::sty, ZeroPageX, 4);

    // TAX
    instructions[0xAA] = Instruction(&CPU::tax, Implied, 2);

    // TAY
    instructions[0xA8] = Instruction(&CPU::tay, Implied, 2);

    // TSX
    instructions[0xBA] = Instruction(&CPU::tsx, Implied, 2);

    // TXA
    instructions[0x8A] = Instruction(&CPU::txa, Implied, 2);

    // TXS
    instructions[0x9A] = Instruction(&CPU::txs, Implied, 2);

    // TYA
    instructions[0x98] = Instruction(&CPU::tya, Implied, 2);
}

/******************* Helper Methods *******************/

void CPU::advanceNClockCycles (int n) {
    // Real-time throttling used to live here (sleeping ~601ns per cycle).
    // Once the PPU exists, timing is driven by the frame loop stepping a
    // fixed CPU/PPU cycle ratio instead, so this is just bookkeeping now.
    cycle += n;
}

void CPU::pushStack (u8 item) {
    u16 stack_addr = 0x0100 | S;
    bus->cpuWrite(stack_addr, item);

    S--;
}

u8 CPU::pullStack() {
    S++;
    u16 stack_addr = 0x0100 | S;

    return bus->cpuRead(stack_addr);
}

u16 CPU::fetchAddress() {
    // Get Addressing Mode
    u8 op_code = bus->cpuRead(PC);
    AddressingMode mode = instructions[op_code].mode;
    u16 address = 0;
    switch (mode) {
        u16 high_byte;
        u16 low_byte;
        u8 zp_address;
        case IndirectX:
            // Fetch contents of address stored at $## + X and $## + X + 1
            zp_address = bus->cpuRead(++PC);
            low_byte = bus->cpuRead((u8)(zp_address + X));
            high_byte = bus->cpuRead((u8)(zp_address + X + 1));
            address = (high_byte << 8) | low_byte;
            break;
        case ZeroPage:
            // Fetch contents of address stored in first 256 bytes
            address = bus->cpuRead(++PC);
            break;
        case Immediate:
            // Address of the operand byte itself
            address = ++PC;
            break;
        case Absolute:
            // Fetch contents of address stored at $XXXX
            low_byte = bus->cpuRead(++PC);
            high_byte = bus->cpuRead(++PC);
            address = (high_byte << 8) | low_byte;
            break;
        case IndirectY:
            zp_address = bus->cpuRead(++PC);
            low_byte = bus->cpuRead(zp_address);
            high_byte = bus->cpuRead((u8)(zp_address + 1));
            address = ((high_byte << 8) | low_byte) + Y;
            break;
        case ZeroPageX:
            zp_address = bus->cpuRead(++PC);
            address = (u8)(zp_address + X);
            break;
        case ZeroPageY:
            zp_address = bus->cpuRead(++PC);
            address = (u8)(zp_address + Y);
            break;
        case AbsoluteY:
            low_byte = bus->cpuRead(++PC);
            high_byte = bus->cpuRead(++PC);
            address = ((high_byte << 8) | low_byte) + Y;
            break;
        case AbsoluteX:
            low_byte = bus->cpuRead(++PC);
            high_byte = bus->cpuRead(++PC);
            address = ((high_byte << 8) | low_byte) + X;
            break;
        case Accumulator:
        case Relative:
        case Implied:
        case Indirect:
            // These modes never route through here -- their instructions
            // decode the operand inline (branches, BRK, JMP, JSR, and the
            // accumulator form of ASL/LSR/ROL/ROR).
            break;
    }

    return address;
}

u8 CPU::fetch() {
    return bus->cpuRead(fetchAddress());
}

/******************* Instructions *******************/

void CPU::lda () {
    A = fetch();
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


void CPU::adc () {
    u8 previous_A = A;

    u8 value = fetch();
    u16 sum = (u16)previous_A + (u16)value + (u16)C;

    PC++;

    A = sum & 0x00FF;

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

    // Unsigned (Carry) and signed (Overflow) checks. Overflow is set when
    // the two operands share a sign but the result doesn't (same trick as
    // sbc(), credit OneLoneCoder https://github.com/OneLoneCoder/olcNES).
    C = sum & 0xFF00;
    V = (~((u16)previous_A ^ (u16)value) & ((u16)previous_A ^ sum)) & 0x0080;
}

void CPU::AND () {
    A = A & fetch();

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

void CPU::asl () {
    u8 op_code = bus->cpuRead(PC);
    bool accumulator = (instructions[op_code].mode == Accumulator);

    u16 addr = 0;
    u8 value = accumulator ? A : bus->cpuRead(addr = fetchAddress());
    u8 previous = value;
    value = value << 1;

    PC++;

    // Set status flags
    if (previous >= 128)
        C = 1;
    else
        C = 0;

    if (value == 0) {
        Z = 1;
        N = 0;
    }
    else if (value >= 128) {
        Z = 0;
        N = 1;
    }
    else {
        Z = 0;
        N = 0;
    }

    if (accumulator)
        A = value;
    else
        bus->cpuWrite(addr, value);
}

void CPU::bcc () {
    // Only one mode, so don't need switch statement

    // Cast the offset to a char because it is a signed integer
    char offset = bus->cpuRead(PC+1);
    if (C == 0)
        PC += 2 + offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bcs () {
    // Only one mode, so don't need switch statement

    // Cast the offset to a char because it is a signed integer
    char offset = bus->cpuRead(PC+1);
    if (C == 1)
        PC += 2 + offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::beq () {
    // Only one mode, so don't need switch statement

    // Cast the offset to a char because it is a signed integer
    char offset = bus->cpuRead(PC+1);
    if (Z == 1)
        PC += 2 + offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::cmp () {
    u8 result = fetch();

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

void CPU::bit () {
    u8 value = fetch();
    u8 result = value & A;

    PC++;

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

void CPU::bmi () {
    // Only one mode, so don't need switch statement

    // Cast the offset to a char because it is a signed integer
    char offset = bus->cpuRead(PC+1);
    if (N == 1)
        PC += 2 + offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bne () {
    // Only one mode, so don't need switch statement

    // Cast the offset to a char because it is a signed integer
    char offset = bus->cpuRead(PC+1);
    if (Z == 0)
        PC += 2 + offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bpl () {
    // Only one mode, so don't need switch statement

    // Cast the offset to a char because it is a signed integer
    char offset = bus->cpuRead(PC+1);
    if (N == 0)
        PC += 2 + offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::brk () {
    // Only one mode, so no switch

    PC += 2;
    u8 high_byte = PC >> 8;
    u8 low_byte = PC & 0xFF;

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
    // Interrupt Disable flag
    status = (status << 1) | I;
    status = (status << 1) | Z;
    status = (status << 1) | C;

    pushStack(high_byte);
    pushStack(low_byte);
    pushStack(status);

    // Get new PC position
    u16 pc_high_byte = bus->cpuRead(0xFFFF);
    u16 pc_low_byte = bus->cpuRead(0xFFFE);
    PC = (pc_high_byte << 8) | pc_low_byte;

    advanceNClockCycles(7);
}

void CPU::bvc () {
    // Only one mode, so don't need switch statement

    // Cast the offset to a char because it is a signed integer
    char offset = bus->cpuRead(PC+1);
    if (V == 0)
        PC += 2 + offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::bvs () {
    // Only one mode, so don't need switch statement

    // Cast the offset to a char because it is a signed integer
    char offset = bus->cpuRead(PC+1);
    if (V == 1)
        PC += 2 + offset;
    else
        PC += 2;

    advanceNClockCycles(2);
}

void CPU::clc () {
    // Only one mode
    C = 0;
    PC++;
    advanceNClockCycles(2);
}

void CPU::cli () {
    // Only one mode
    I = 0;
    PC++;
    advanceNClockCycles(2);
}

void CPU::clv () {
    V = 0;
    PC++;
    advanceNClockCycles(2);
}

void CPU::cpx () {
    u8 result = fetch();

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

void CPU::cpy () {
    u8 result = fetch();

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

void CPU::dec () {
    u16 addr = fetchAddress();
    u8 value = bus->cpuRead(addr);
    value--;
    bus->cpuWrite(addr, value);

    PC++;

    if (value == 0) {
        Z = 1;
        N = 0;
    }
    else if (value >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::dex () {
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

void CPU::dey () {
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

void CPU::eor () {
    A = A ^ fetch();

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

void CPU::inc () {
    u16 addr = fetchAddress();
    u8 value = bus->cpuRead(addr);
    value++;
    bus->cpuWrite(addr, value);

    PC++;

    if (value == 0) {
        Z = 1;
        N = 0;
    }
    else if (value >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }
}

void CPU::inx () {
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

void CPU::iny () {
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

void CPU::jmp () {
    u16 low_byte;
    u16 high_byte;
    u16 value;
    // This switch statement is an unfortunate side effect of
    // the way I made the fetch method work, and because I'm
    // a dumbass
    u8 op_code = bus->cpuRead(PC);
    AddressingMode mode = instructions[op_code].mode;
    switch (mode) {
        // Absolute
        case Absolute:
            low_byte = bus->cpuRead(++PC);
            high_byte = bus->cpuRead(++PC);
            value = (high_byte << 8) | low_byte;
            PC = value;
            advanceNClockCycles(3);
            break;
        // Indirect
        case Indirect:
            // Set the PC to the address stored at the address given by the programmer
            low_byte = bus->cpuRead(++PC);
            high_byte = bus->cpuRead(++PC);
            value = (high_byte << 8) | low_byte;
            // Address stored at the address given
            low_byte = bus->cpuRead(value);
            high_byte = bus->cpuRead((u16)(value + 1));
            value = (high_byte << 8) | low_byte;
            PC = value;
            advanceNClockCycles(5);
            break;
        default:
            break;
    }
}

void CPU::jsr () {
    u16 low_byte = bus->cpuRead(++PC);
    u16 high_byte = bus->cpuRead(++PC);
    u16 value = (high_byte << 8) | low_byte;
    // Return address
    PC++;
    u8 pc_low_byte = PC & 0xFF;
    u8 pc_high_byte = PC >> 8;

    pushStack(pc_high_byte);
    pushStack(pc_low_byte);
    PC = value;
    advanceNClockCycles(6);
}

void CPU::ldx () {
    X = fetch();

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

void CPU::ldy () {
    Y = fetch();

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

void CPU::lsr () {
    u8 op_code = bus->cpuRead(PC);
    bool accumulator = (instructions[op_code].mode == Accumulator);

    // Least significant bit gets stored in Carry flag
    u16 addr = 0;
    u8 value = accumulator ? A : bus->cpuRead(addr = fetchAddress());
    u8 lsb = value & 1;
    value = value >> 1;

    PC++;

    // Set status flags
    C = lsb;

    if (value == 0) {
        Z = 1;
    }
    else {
        Z = 0;
    }

    // Number will never be negative
    N = 0;

    if (accumulator)
        A = value;
    else
        bus->cpuWrite(addr, value);
}

void CPU::nop () {
    PC++;
    advanceNClockCycles(2);
}

void CPU::OR () {
    A = A | fetch();

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

void CPU::pha () {
    pushStack(A);
    ++PC;
    advanceNClockCycles(3);
}

void CPU::php () {
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
    // Interrupt Disable flag
    status = (status << 1) | I;
    status = (status << 1) | Z;
    status = (status << 1) | C;

    pushStack(status);
    ++PC;
    advanceNClockCycles(3);
}

void CPU::pla () {
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

    ++PC;
    advanceNClockCycles(4);
}

void CPU::plp () {
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

    ++PC;
    advanceNClockCycles(4);
}

void CPU::rol () {
    u8 op_code = bus->cpuRead(PC);
    bool accumulator = (instructions[op_code].mode == Accumulator);

    u16 addr = 0;
    u8 value = accumulator ? A : bus->cpuRead(addr = fetchAddress());

    PC++;

    // Rotate the value left 1
    u8 new_c = value >= 128 ? 1 : 0;
    value = value << 1;
    value |= C;
    C = new_c;

    if (value == 0) {
        Z = 1;
        N = 0;
    }
    else if (value >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }

    if (accumulator)
        A = value;
    else
        bus->cpuWrite(addr, value);
}

void CPU::ror () {
    u8 op_code = bus->cpuRead(PC);
    bool accumulator = (instructions[op_code].mode == Accumulator);

    u16 addr = 0;
    u8 value = accumulator ? A : bus->cpuRead(addr = fetchAddress());

    PC++;

    // Rotate the value right 1
    u8 new_c = value & 1;
    value = value >> 1;
    value |= (C << 7);
    C = new_c;

    if (value == 0) {
        Z = 1;
        N = 0;
    }
    else if (value >= 128) {
        N = 1;
        Z = 0;
    }
    else {
        Z = 0;
        N = 0;
    }

    if (accumulator)
        A = value;
    else
        bus->cpuWrite(addr, value);
}

void CPU::rti () {
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

void CPU::rts () {
    u8 low_byte = pullStack();
    u16 high_byte = pullStack();
    PC = (high_byte << 8) | low_byte;

    advanceNClockCycles(6);
}

void CPU::sbc () {
    u8 result = fetch();
    // Invert bits of result
    u16 value = (u16)result ^ 0x00FF;
    u16 temp = (u16)A + value + (u16)C;

    PC++;

    // Credit to OneLoneCoder (https://github.com/OneLoneCoder/olcNES) for how to set these flags.
    // I really couldn't figure it out.
    C = temp & 0xFF00;
    V = (temp ^ (u16)A) & (temp ^ value) & 0x0080;

    A = temp & 0x00FF;

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
}

void CPU::sec() {
    C = 1;
    ++PC;
}

void CPU::sei() {
    I = 1;
    ++PC;
}

void CPU::sta() {
    u16 addr = fetchAddress();
    bus->cpuWrite(addr, A);

    ++PC;
}

void CPU::stx() {
    u16 addr = fetchAddress();
    bus->cpuWrite(addr, X);

    ++PC;
}

void CPU::sty() {
    u16 addr = fetchAddress();
    bus->cpuWrite(addr, Y);

    ++PC;
}

void CPU::tax() {
    X = A;
    ++PC;

    if (X >= 128)
        N = 1;
    else
        N = 0;

    if (X == 0)
        Z = 1;
    else
        Z = 0;
}

void CPU::tay() {
    Y = A;
    ++PC;

    if (Y >= 128)
        N = 1;
    else
        N = 0;

    if (Y == 0)
        Z = 1;
    else
        Z = 0;
}

void CPU::tsx() {
    X = S;
    ++PC;

    if (X >= 128)
        N = 1;
    else
        N = 0;

    if (X == 0)
        Z = 1;
    else
        Z = 0;
}

void CPU::txa() {
    A = X;
    ++PC;

    if (A >= 128)
        N = 1;
    else
        N = 0;

    if (A == 0)
        Z = 1;
    else
        Z = 0;
}

void CPU::txs() {
    S = X;
    ++PC;
}

void CPU::tya() {
    A = Y;
    ++PC;

    if (A >= 128)
        N = 1;
    else
        N = 0;

    if (A == 0)
        Z = 1;
    else
        Z = 0;
}
