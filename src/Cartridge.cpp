#include "Cartridge.h"
#include "Mapper0.h"
#include <fstream>
#include <iostream>

namespace {
    struct INesHeader {
        char name[4];
        u8 prgRomChunks;   // 16KB units
        u8 chrRomChunks;   // 8KB units (0 means the cartridge uses CHR-RAM)
        u8 flags6;
        u8 flags7;
        u8 prgRamSize;
        u8 flags9;
        u8 flags10;
        char unused[5];
    };
}

Cartridge::Cartridge (const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open ROM: " << path << std::endl;
        return;
    }

    INesHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.name[0] != 'N' || header.name[1] != 'E' ||
        header.name[2] != 'S' || header.name[3] != 0x1A) {
        std::cerr << "Not a valid iNES file: " << path << std::endl;
        return;
    }

    if (header.flags6 & 0x04) {
        // Skip the 512-byte trainer, if present.
        file.seekg(512, std::ios::cur);
    }

    mirroring = (header.flags6 & 0x01) ? Mirroring::Vertical : Mirroring::Horizontal;

    u8 mapperId = (header.flags7 & 0xF0) | (header.flags6 >> 4);

    prgMemory.resize(header.prgRomChunks * 16384);
    file.read(reinterpret_cast<char*>(prgMemory.data()), prgMemory.size());

    if (header.chrRomChunks == 0) {
        // CHR-RAM: the cartridge supplies no CHR-ROM; the PPU can write to it.
        chrMemory.resize(8192);
    } else {
        chrMemory.resize(header.chrRomChunks * 8192);
        file.read(reinterpret_cast<char*>(chrMemory.data()), chrMemory.size());
    }

    switch (mapperId) {
        case 0:
            mapper = std::make_unique<Mapper0>(header.prgRomChunks, header.chrRomChunks);
            break;
        default:
            std::cerr << "Unsupported mapper: " << (int)mapperId << std::endl;
            return;
    }

    valid = true;
}

bool Cartridge::cpuRead (u16 addr, u8& value) {
    u32 mapped;
    if (!mapper->cpuMapRead(addr, mapped))
        return false;

    value = prgMemory[mapped];
    return true;
}

bool Cartridge::cpuWrite (u16 addr, u8 value) {
    u32 mapped;
    if (!mapper->cpuMapWrite(addr, mapped))
        return false;

    prgMemory[mapped] = value;
    return true;
}

bool Cartridge::ppuRead (u16 addr, u8& value) {
    u32 mapped;
    if (!mapper->ppuMapRead(addr, mapped))
        return false;

    value = chrMemory[mapped];
    return true;
}

bool Cartridge::ppuWrite (u16 addr, u8 value) {
    u32 mapped;
    if (!mapper->ppuMapWrite(addr, mapped))
        return false;

    chrMemory[mapped] = value;
    return true;
}
