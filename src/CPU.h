#ifndef _CPU_H_
#define _CPU_H_

#include <cstdint>
#include <tuple>
#include <chrono>
#include <thread>
#include <iostream>

class CPU;

typedef unsigned char u8;
typedef unsigned short u16;

enum AddressingMode {
    IndirectX,
    ZeroPage,
    Immediate,
    Absolute,
    IndirectY,
    ZeroPageX,
    ZeroPageY,
    AbsoluteY,
    AbsoluteX,
    Accumulator,
    Relative,
    Implied,
    Indirect
};

// Function pointer for instruction set
typedef void (CPU::*Fp)(AddressingMode);

/****************************************************************************
* CPU class emulates the CPU (duh). The NES has a 6502 processor
* which has 6 registers: A, X, Y, PC, S, and P. All are byte wide
* except for the PC which is 2.
* A = Accumulator
* X & Y = General purpose registers
* PC = Program Counter
* S = Stack Pointer
* 
* Status Flags: In the NES this is a register, P, but for simplicity
* and saving the trouble of doing bitwise operations, just doing boolean flags.
*
* N = Negative
* Z = Zero
* V = Overflow
* B = Break
* I = Interrupt Disable
* C = Carry
*
*
* Even though the NES is an 8-bit console, it has a 16-bit address space
* and several addressing modes. 
*****************************************************************************/

class CPU {
    public:


    struct Instruction {
        Fp instruction;
        AddressingMode mode;

        Instruction() : instruction(nullptr), mode (Implied) {}

        Instruction (Fp i, AddressingMode m)
        : instruction(i),
          mode (m)
        { }
    };
    
    CPU(u8 prg_start_low, u8 prg_start_high);
    ~CPU();
    
    // Sets the PC to the correct memory address to run the loaded program
    void init();

    /*********** Registers ***********/
    // Accumulator
    u8 A;
    // General Purpose Regs
    u8 X;
    u8 Y;
    // Program Counter
    u16 PC;
    // Stack pointer
    u8 S;
    // Status Flags
    bool N;
    bool V;
    bool B;
    bool I;
    bool Z;
    bool C;

    // Memory
    u8* mem;

    // Cycle #
    int cycle;

    // Current Cycle Time
    std::chrono::time_point<std::chrono::system_clock> current_cycle_time = std::chrono::high_resolution_clock::now();

    /************** Helper Methods **************/
    void advanceNClockCycles (int n);
    void pushStack (u8 item);
    u8 pullStack();
    u8* fetch();

    /*********** Instruction Methods ***********/
    // Load Accumulator
    // Stores either the value given or the value at
    // the mem address given based on the addressing mode.
    // Sets the N and Z flags.
    void lda(AddressingMode mode);

    // Add w/ Carry
    // Adds either the value given or the value at the 
    // mem address given (addressing mode) to the accumulator
    // along with the carry bit of the status register.
    // A + M + C -> A, C
    void adc(AddressingMode mode);

    // And
    // Bitwise ands either the value given or the value at the
    // mem address given with the accumulator, then stores
    // the result in the accumulator.
    // A AND M -> A
    void AND(AddressingMode mode);

    // Arithmetic Shift Left
    // Shift Left 1 Bit
    // Shift the accumulator or memory left 1 bit
    void asl(AddressingMode mode);

    // Branch on Carry Clear
    // Set program counter to address +- offset given if C == 0
    void bcc(AddressingMode mode);

    // Branch on Carry Set
    // Set program counter to address +- offset given if C == 1
    void bcs(AddressingMode mode);

    // Branch on Result Zero
    // Set program counter to address +- offset given if Z == 1
    void beq(AddressingMode mode);

    // Compare Accumulator
    // Compares the either the value given or the value at the
    // address given with the accumulator, then sets the appropriate
    // status flags based on the result
    void cmp(AddressingMode mode);

    // Bit
    // ANDs the value at the memory address given with the accumulator
    // and sets Z accordingly. It also takes the 7 and 6 bits of the value
    // given and sets the N and V flags respectively
    void bit(AddressingMode mode);

    // Branch on Minus
    // Set program counter to address +- offset given if N == 1
    void bmi(AddressingMode mode);

    // Branch on Result Not Zero
    // Set program counter to address +- offset given if Z == 0
    void bne(AddressingMode mode);

    // Branch on Result Plus
    // Set program counter to address +- offset given if N == 0
    void bpl(AddressingMode mode);

    // Break
    // Triggers a software interrupt. Stores the PC and status flags on the stack
    // and grabs the location stored at $FFFE-$FFFF and sets the PC to that.
    // Also sets the interrupt disable flag.
    void brk(AddressingMode mode);

    // Branch on Overflow Clear
    // Set program counter to address +- offset given if V == 0
    void bvc(AddressingMode mode);

    // Branch on Overflow Set
    // Set program counter to address +- offset given if V == 1
    void bvs(AddressingMode mode);

    // Clear Carry Flag
    // Sets the Carry Flag to 0
    void clc(AddressingMode mode);

    // Clear Interrupt Flag
    // Sets the Interrup Disable Flag to 0
    void cli(AddressingMode mode);

    // Clear Overflow Flag
    // Sets the Overflow Flag to 0
    void clv(AddressingMode mode);

    // Compare Memory and X
    void cpx(AddressingMode mode);

    // Compare Memory and Y
    void cpy(AddressingMode mode);

    // Decrement Memory by 1
    // Decreases the value at the memory address by 1
    void dec(AddressingMode mode);

    // Decrement X by 1
    void dex(AddressingMode mode);
    
    // Decrement Y by 1
    void dey(AddressingMode mode);

    // Exclusive OR Memory w/ Accumulator
    // Bitwise XORs either the value given or the value at the
    // mem address given with the accumulator, then stores
    // the result in the accumulator. 
    void eor(AddressingMode mode);

    // Increment Memory by 1
    // Increments the value at the memory address by 1
    void inc(AddressingMode mode);

    // Increment X by 1;
    void inx(AddressingMode mode);

    // Increment Y by 1;
    void iny(AddressingMode mode);

    // Jump
    // Moves the program to the specified address
    void jmp(AddressingMode mode);

    // Jump Subroutine
    // Stores the PC and Status Flags on the stack then jumps
    // to address specified
    void jsr(AddressingMode mode);

    // Load X with Memory
    // Loads the given memory address into the X register
    void ldx(AddressingMode mode);

    // Load Y with Memory
    // Loads the given memory address into the Y register
    void ldy(AddressingMode mode);

    // Shift 1 Bit Right
    // Shifts value given or value at mem address given right
    // by 1 bit
    void lsr(AddressingMode mode);

    // No Operation
    // Um...pretty obvious what this does I think
    void nop(AddressingMode mode);

    // OR
    // Bitwise ors either the value given or the value at the
    // mem address given with the accumulator, then stores
    // the result in the accumulator.
    // A OR M -> A
    void OR(AddressingMode mode);

    // Push Accumulator on Stack
    void pha(AddressingMode mode);

    // Push Status Register on Stack
    void php(AddressingMode mode);

    // Pull Accumulator from Stack
    // Pulls last value from stack and places it into accumulator
    void pla(AddressingMode mode);

    // Pull Status Register from Stack
    // Pulls last value from stack and places it into the status register
    void plp(AddressingMode mode);

    // Rotate 1 Bit Left
    // Rotates the value given or the value at the mem address given
    // left with the Carry flag. The carry flag becomes the lsb and 
    // the msb becomes the carry flag.
    void rol(AddressingMode mode);

    // Rotate 1 Bit Right
    // Rotates the value given or the value at the mem address given
    // right with the Carry flag. The carry flag becomes the msb and 
    // the lsb becomes the carry flag.
    void ror(AddressingMode mode);

    // Return from Interrupt
    // Returns the program to where it was before the interrupt occurred.
    void rti(AddressingMode mode);

    // Return from Subroutine
    // Returns the program to where it was before the subroutine
    void rts(AddressingMode mode);

    // Subtract Memory from Accumulator with Borrow
    void sbc(AddressingMode mode);

    /************* Instruction Set ***************
    * The instructions array holds Instruction struct containing
    * function pointers to the instruction methods and an enum for the 
    * addressing modes of that specific instruction. Each index 
    * corresponds to the instruction at the location in the instruction 
    * table (that instruction's op code)
    * https://www.masswerk.at/6502/6502_instruction_set.html
    * **********************************************/

    Instruction instructions[256];
};

#endif // _CPU_H_'
