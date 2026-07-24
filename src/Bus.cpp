#include "Bus.h"
#include <algorithm>

Bus::Bus() {
    cpu.attachBus(this);
    std::fill(std::begin(ram), std::end(ram), 0x00);
}

void Bus::insertCartridge (std::unique_ptr<Cartridge> cart) {
    cartridge = std::move(cart);
    ppu.attachCartridge(cartridge.get());
}

void Bus::reset() {
    cpu.reset();
}

u8 Bus::cpuRead (u16 addr) {
    u8 value = 0;

    if (addr <= 0x1FFF) {
        value = ram[addr & 0x07FF];
    }
    else if (addr <= 0x3FFF) {
        value = ppu.cpuRead(addr & 0x0007);
    }
    else if (addr <= 0x4017) {
        // APU / controller ports -- not wired up yet.
    }
    else if (cartridge) {
        cartridge->cpuRead(addr, value);
    }

    return value;
}

void Bus::cpuWrite (u16 addr, u8 value) {
    if (addr <= 0x1FFF) {
        ram[addr & 0x07FF] = value;
    }
    else if (addr <= 0x3FFF) {
        ppu.cpuWrite(addr & 0x0007, value);
    }
    else if (addr <= 0x4017) {
        // APU / controller ports -- not wired up yet.
    }
    else if (cartridge) {
        cartridge->cpuWrite(addr, value);
    }
}

u8 Bus::clock() {
    u8 cpuCycles = cpu.step();

    for (u8 i = 0; i < cpuCycles * 3; ++i) {
        ppu.clock();
    }

    if (ppu.nmiRequested) {
        ppu.nmiRequested = false;
        cpu.nmi();
    }

    return cpuCycles;
}
