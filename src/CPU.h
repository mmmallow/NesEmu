#ifndef _CPU_H_
#define _CPU_H_

#include <cstdint>
#include <tuple>
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>

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
typedef void (CPU::*Fp)();

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
    void lda();

    // Add w/ Carry
    // Adds either the value given or the value at the 
    // mem address given (addressing mode) to the accumulator
    // along with the carry bit of the status register.
    // A + M + C -> A, C
    void adc();

    // And
    // Bitwise ands either the value given or the value at the
    // mem address given with the accumulator, then stores
    // the result in the accumulator.
    // A AND M -> A
    void AND();

    // Arithmetic Shift Left
    // Shift Left 1 Bit
    // Shift the accumulator or memory left 1 bit
    void asl();

    // Branch on Carry Clear
    // Set program counter to address +- offset given if C == 0
    void bcc();

    // Branch on Carry Set
    // Set program counter to address +- offset given if C == 1
    void bcs();

    // Branch on Result Zero
    // Set program counter to address +- offset given if Z == 1
    void beq();

    // Compare Accumulator
    // Compares the either the value given or the value at the
    // address given with the accumulator, then sets the appropriate
    // status flags based on the result
    void cmp();

    // Bit
    // ANDs the value at the memory address given with the accumulator
    // and sets Z accordingly. It also takes the 7 and 6 bits of the value
    // given and sets the N and V flags respectively
    void bit();

    // Branch on Minus
    // Set program counter to address +- offset given if N == 1
    void bmi();

    // Branch on Result Not Zero
    // Set program counter to address +- offset given if Z == 0
    void bne();

    // Branch on Result Plus
    // Set program counter to address +- offset given if N == 0
    void bpl();

    // Break
    // Triggers a software interrupt. Stores the PC and status flags on the stack
    // and grabs the location stored at $FFFE-$FFFF and sets the PC to that.
    // Also sets the interrupt disable flag.
    void brk();

    // Branch on Overflow Clear
    // Set program counter to address +- offset given if V == 0
    void bvc();

    // Branch on Overflow Set
    // Set program counter to address +- offset given if V == 1
    void bvs();

    // Clear Carry Flag
    // Sets the Carry Flag to 0
    void clc();

    // Clear Interrupt Flag
    // Sets the Interrup Disable Flag to 0
    void cli();

    // Clear Overflow Flag
    // Sets the Overflow Flag to 0
    void clv();

    // Compare Memory and X
    void cpx();

    // Compare Memory and Y
    void cpy();

    // Decrement Memory by 1
    // Decreases the value at the memory address by 1
    void dec();

    // Decrement X by 1
    void dex();
    
    // Decrement Y by 1
    void dey();

    // Exclusive OR Memory w/ Accumulator
    // Bitwise XORs either the value given or the value at the
    // mem address given with the accumulator, then stores
    // the result in the accumulator. 
    void eor();

    // Increment Memory by 1
    // Increments the value at the memory address by 1
    void inc();

    // Increment X by 1;
    void inx();

    // Increment Y by 1;
    void iny();

    // Jump
    // Moves the program to the specified address
    void jmp();

    // Jump Subroutine
    // Stores the PC and Status Flags on the stack then jumps
    // to address specified
    void jsr();

    // Load X with Memory
    // Loads the given memory address into the X register
    void ldx();

    // Load Y with Memory
    // Loads the given memory address into the Y register
    void ldy();

    // Shift 1 Bit Right
    // Shifts value given or value at mem address given right
    // by 1 bit
    void lsr();

    // No Operation
    // Um...pretty obvious what this does I think
    void nop();

    // OR
    // Bitwise ors either the value given or the value at the
    // mem address given with the accumulator, then stores
    // the result in the accumulator.
    // A OR M -> A
    void OR();

    // Push Accumulator on Stack
    void pha();

    // Push Status Register on Stack
    void php();

    // Pull Accumulator from Stack
    // Pulls last value from stack and places it into accumulator
    void pla();

    // Pull Status Register from Stack
    // Pulls last value from stack and places it into the status register
    void plp();

    // Rotate 1 Bit Left
    // Rotates the value given or the value at the mem address given
    // left with the Carry flag. The carry flag becomes the lsb and 
    // the msb becomes the carry flag.
    void rol();

    // Rotate 1 Bit Right
    // Rotates the value given or the value at the mem address given
    // right with the Carry flag. The carry flag becomes the msb and 
    // the lsb becomes the carry flag.
    void ror();

    // Return from Interrupt
    // Returns the program to where it was before the interrupt occurred.
    void rti();

    // Return from Subroutine
    // Returns the program to where it was before the subroutine
    void rts();

    // Subtract Memory from Accumulator with Borrow
    void sbc();

    // Set Carry Flag
    void sec();

    // Set Interrupt Disable Flag
    void sei();

    // Store Accumulator in Memory
    // Stores the value in A at memory location
    void sta();

    // Store X in Memory
    // Stores the value in X at memory location
    void stx();

    // Store Y in Memory
    // Stores the value in Y at memory location
    void sty();

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
