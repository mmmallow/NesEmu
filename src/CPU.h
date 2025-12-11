#ifndef _CPU_H_
#define _CPU_H_

#include <cstdint>
#include <tuple>
#include <chrono>
#include <thread>

class CPU;

typedef uint8_t u8;
typedef uint16_t u16;

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
* P = Status Flags
*
* Even though the NES is an 8-bit console, it has a 16-bit address space
* and several addressing modes. 
*****************************************************************************/

class CPU {
    public:
    
    CPU();
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
    // Status Reg
    u8 P;

    // Memory
    u8 mem[0x10000] = {0};

    // Cycle #
    int cycle;

    // Current Cycle Time
    std::chrono::time_point<std::chrono::system_clock> current_cycle_time;

    void advanceNClockCycles (int n);

    /*********** Instruction Methods ***********/
    // Load Accumulator
    // Stores either the value given or the value at
    // the mem address given based on the addressing mode.
    void lda(u8 mode);

    // Add w/ Carry
    // Adds either the value given or the value at the 
    // mem address given (addressing mode) to the accumulator
    // along with the carry bit of the status register.
    // A + M + C -> A, C
    void adc(u8 mode);



    /************* Instruction Enum ***************
    * The instructions array holds tuples of function pointers
    * to the instruction methods and u8's for the addressing modes
    * of that specific instruction. Each index corresponds to the 
    * instruction at the location in the instruction table
    * at https://www.masswerk.at/6502/6502_instruction_set.html
    * **********************************************/

    std::tuple<Fp, u8> instructions[256];
};

#endif // _CPU_H_
