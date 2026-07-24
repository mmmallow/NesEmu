#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include <string>
#include <vector>
#include <memory>
#include "Types.h"
#include "Mapper.h"

enum class Mirroring { Horizontal, Vertical };

// Owns a ROM's raw PRG/CHR data (parsed from its iNES header) plus the
// Mapper that knows how to translate CPU/PPU addresses into offsets into it.
class Cartridge {
    public:

    explicit Cartridge (const std::string& path);

    bool cpuRead (u16 addr, u8& value);
    bool cpuWrite (u16 addr, u8 value);
    bool ppuRead (u16 addr, u8& value);
    bool ppuWrite (u16 addr, u8 value);

    Mirroring mirroring = Mirroring::Horizontal;

    // False if the file couldn't be read, wasn't a valid iNES file, or uses
    // a mapper that isn't implemented yet.
    bool valid = false;

    private:

    std::vector<u8> prgMemory;
    std::vector<u8> chrMemory;
    std::unique_ptr<Mapper> mapper;
};

#endif // _CARTRIDGE_H_
