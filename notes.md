
# CPU Gameplan & Notes

> General idea is to initialize CPU then read from .nes file which acts like ROM. First 16 KB
> are header, rest is game data. From there, interpret the instruction data using functions modeling
> the instruction set. Have RAM structure using array. Probably need to look into addressing modes
> and other more nuanced things, but for now just do the basics.

* For opcodes, might want to use the Command pattern. Since they're all codes and they each have a
  specific function, it seems like it might be a good fit.

* CPU should probably be a class. Should have all the registers as attributes and RAM
  * For now, just create an array of u8 that acts as the RAM.

* *Registers:* A = Accumulator. X and Y are general purpose registers. Program Counter is 2 bytes wide.
  S = 8-bit stack pointer (0x0100-0x01FF). P = 8-bit status register, holds flags for use by the ALU.
  * _Stack Pointer:_ Byte wide, holds only the low-byte of an address as an offset from 0x0100.
  * _Addressing Modes:_ There are multiple addressing modes the CPU can use. Zero page refers to the
    first 256 bytes of RAM (0x0000-0x00FF). Immediate means the value given is the value used in the
    instruction (if lda #$10 is the instruction, 10 would be placed in the accumulator). Absolute addressing
    is the 16-bit address of any memory location. Relative is an 8 bit offset from current PC (used by branch
    instructions). Indirect can go to anywhere in memory (used by jump instruction).
    * Instructions are broken down as such:
      | op - AAA | mode - BBB | grp - CC |
      |----------|------------|----------|
      | 101      | 001        | 01       |
      | 101      | 010        | 01       |
    * The instructions above are lda $20 and lda #$20 (zero page and immediate). The hex are 0xa520
      and 0xa920. 101 = lda, 001 = zero page, 010 = immediate, and 01 = group.
  * _Updating PC:_ The next address for the PC depends on the addressing mode. Updated PC should look
    like PC = PC + instruction_length. So for absolute addressing, the instruction length would be
    3 bytes (1 for instruction, 2 for absolute 16-bit address). For zero page it would be 2 bytes
    (1 for instruction and 1 for zero page 8-bit address).
  * _Status Register:_ Byte wide, contains flags meant for use by the ALU. Flags are as follows -
    NV1BDIZC.
    * N = Negative, gets set if the leftmost bit of a result is set (only on signed ints)
    * V = Overflow, tracks overflows for signed operations
    * 1 = Not used, always set to 1
    * B = Break, tracks whether an interrupt was signaled by a break instruction or hardware
    * I = Interrupt Disable, enables/disables certain interrupts, 2 instructions to set or clear this bit
    * Z = Zero, set whenever the result is 0, often used to check if two values are equal (A - B == 0)
    * C = Carry (unsigned overflow), set whenever the result of an unsigned operation is bigger than 255

* *Interrupts:*  ~~Since CPU stops whatever it's doing during interrupt, might want a smaller version of a PCB
  and have some kind of interrupt listener working on another thread. Could also just use the
  stack and push all of the register values on and pop off in opposite order.~~ 
  (Stack is located at 0x0100-0x01FF). Interrupts are polled at specific points during CPU cycle,
  typically before fetching next instruction, so if there is an interrupt, it gets handled and 
  the address of the next instruction gets stashed on the stack until the interrupt is finished.
  * IRQ's are detected using a level detector that checks for a low signal during the second
    half of a CPU cycle. From there, the IRQ vector is set and is handled during the first
    half of the next cycle. So this is what needs to be a second thread and the level detection
    can be blocked off by a mutex lock so that it only occurs during the second half of the cycle.
  * NMI's are detected using an edge detector that checks if the signal goes from high to low during
    the second half of the CPU cycle. The signal stays high until the NMI is handled, which occurs 
    during the first half of the next cycle. NMI's are triggered when the PPU has finished drawing
    a frame, specifically scanline 241, which allows the CPU to safely access PPU memory.
  * If both NMI and IRQ are pending, the NES prioritizes NMI. The IRQ gets ignored until the next
    polling cycle.

* *Start Up:* A, X, and Y are initialized to 0. The PC starts out at 0xFFFC (RESET Vector). Stack pointer
  starts out at 0x01FD. C, Z, D, V, and N flags start as 0. 

* Game programmer chooses the addressing mode

* Programmer also sets the RESET vector (located at 0xFFFC & 0xFFFD as a 16-bit address) which tells
  the CPU where in memory to set the Program Counter and start executing.

* Definitely want the whole thing to be at least semi-modular, makes for easier testing and
  most likely makes the GUI easier to implement as well if decide to implement at all.

## Plan For Now

* Create the CPU class with some basic instructions
* Write a simple program into memory (don't forget to set the RESET vector) that adds 2 numbers
  (stored in x and y) and puts the result in the accumulator

