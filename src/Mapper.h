#ifndef _MAPPER_H_
#define _MAPPER_H_

#include "Types.h"

// A Mapper translates CPU/PPU addresses into offsets within a cartridge's
// PRG/CHR memory. Each mapper chip (NROM, MMC1, ...) gets its own subclass.
class Mapper {
    public:

    Mapper (u8 prgBanks, u8 chrBanks)
    : prgBanks(prgBanks),
      chrBanks(chrBanks)
    { }

    virtual ~Mapper() = default;

    virtual bool cpuMapRead (u16 addr, u32& mappedAddr) = 0;
    virtual bool cpuMapWrite (u16 addr, u32& mappedAddr) = 0;
    virtual bool ppuMapRead (u16 addr, u32& mappedAddr) = 0;
    virtual bool ppuMapWrite (u16 addr, u32& mappedAddr) = 0;

    protected:

    u8 prgBanks;
    u8 chrBanks;
};

#endif // _MAPPER_H_
