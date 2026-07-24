#ifndef _BUS_H_
#define _BUS_H_

#include <memory>
#include "Types.h"
#include "CPU.h"
#include "PPU.h"
#include "Cartridge.h"

// Owns the CPU-visible address space: 2K of internal RAM (mirrored through
// $1FFF), the PPU's registers at $2000-$3FFF, and the cartridge (PRG-ROM/RAM
// and CHR-ROM/RAM via its mapper). The CPU has no memory of its own; every
// read/write it does comes back through here.
class Bus {
    public:

    Bus();

    CPU cpu;
    PPU ppu;

    void insertCartridge (std::unique_ptr<Cartridge> cart);

    // Resets the CPU (reads the reset vector through the cartridge).
    void reset();

    u8 cpuRead (u16 addr);
    void cpuWrite (u16 addr, u8 value);

    // Runs the system for exactly one CPU instruction: ticks the PPU 3
    // dots per CPU cycle (the NTSC 3:1 ratio), using the instruction's base
    // cycle cost, then fires CPU::nmi() if the PPU requested one. Returns
    // the number of CPU cycles the instruction took.
    u8 clock();

    private:

    u8 ram[2048];
    std::unique_ptr<Cartridge> cartridge;
};

#endif // _BUS_H_
