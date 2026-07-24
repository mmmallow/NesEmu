#ifndef _MAPPER0_H_
#define _MAPPER0_H_

#include "Mapper.h"

// NROM. No bank switching: a single 16K PRG bank is mirrored across both
// halves of $8000-$FFFF, a 32K PRG image is mapped straight through. CHR is
// a single fixed 8K bank -- RAM if the cartridge declared zero CHR-ROM banks.
class Mapper0 : public Mapper {
    public:

    Mapper0 (u8 prgBanks, u8 chrBanks)
    : Mapper(prgBanks, chrBanks)
    { }

    bool cpuMapRead (u16 addr, u32& mappedAddr) override {
        if (addr < 0x8000)
            return false;

        mappedAddr = addr & (prgBanks > 1 ? 0x7FFF : 0x3FFF);
        return true;
    }

    bool cpuMapWrite (u16 addr, u32& mappedAddr) override {
        return cpuMapRead(addr, mappedAddr);
    }

    bool ppuMapRead (u16 addr, u32& mappedAddr) override {
        if (addr > 0x1FFF)
            return false;

        mappedAddr = addr;
        return true;
    }

    bool ppuMapWrite (u16 addr, u32& mappedAddr) override {
        // Only CHR-RAM cartridges (chrBanks == 0) can be written to.
        if (addr > 0x1FFF || chrBanks != 0)
            return false;

        mappedAddr = addr;
        return true;
    }
};

#endif // _MAPPER0_H_
