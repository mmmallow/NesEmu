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

// Function pointer for instruction set
typedef void (CPU::*Fp)(u8);

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

    void advanceNClockCycles (int n);

    /*********** Instruction Methods ***********/
    // Load Accumulator
    // Stores either the value given or the value at
    // the mem address given based on the addressing mode.
    // Sets the N and Z flags.
    void lda(u8 mode);

    // Add w/ Carry
    // Adds either the value given or the value at the 
    // mem address given (addressing mode) to the accumulator
    // along with the carry bit of the status register.
    // A + M + C -> A, C
    void adc(u8 mode);

    // And
    // Bitwise ands either the value given or the value at the
    // mem address given with the accumulator, then stores
    // the result in the accumulator.
    // A AND M -> A
    void AND(u8 mode);

    // Arithmetic Shift Left
    // Shift Left 1 Bit
    // Shift the accumulator or memory left 1 bit
    void asl(u8 mode);

    // Branch on Carry Clear
    // Set program counter to address +- offset given if C == 0
    void bcc(u8 mode);

    // Branch on Carry Set
    // Set program counter to address +- offset given if C == 1
    void bcs(u8 mode);

    // Branch on Result Zero
    // Set program counter to address +- offset given if Z == 1
    void beq(u8 mode);


    /************* Instruction Set ***************
    * The instructions array holds tuples of function pointers
    * to the instruction methods and u8's for the addressing modes
    * of that specific instruction. Each index corresponds to the 
    * instruction at the location in the instruction table
    * at https://www.masswerk.at/6502/6502_instruction_set.html
    * **********************************************/

    std::tuple<Fp, u8> instructions[256];
};

#endif // _CPU_H_
