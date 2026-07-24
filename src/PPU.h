#ifndef _PPU_H_
#define _PPU_H_

#include "Types.h"
#include "Cartridge.h"

/****************************************************************************
* Emulates the 2C02 PPU: registers, VRAM/palette/OAM memory, and rendering.
*
* Rendering is done a full scanline at a time (at the start of each visible
* scanline) rather than dot-by-dot. Real hardware fetches and shifts pixels
* one at a time across 341 dots/scanline; this instead recomputes an entire
* row's worth of background+sprite pixels in one shot from the current
* scroll ("loopy") registers. That's not cycle-exact -- it can't reproduce
* mid-scanline raster tricks -- but it's dramatically simpler and is
* sufficient for games that only change scroll during vblank (which covers
* ordinary horizontal-scrolling platformers).
*
* vblank/NMI timing is still tracked with real per-scanline/per-dot
* granularity (see clock()), since games depend on that to know when it's
* safe to touch PPU memory and to detect sprite-0 hit for split-screen
* effects (e.g. Super Mario Bros.'s status bar).
****************************************************************************/
class PPU {
    public:

    PPU();

    void attachCartridge (Cartridge* c) { cartridge = c; }

    // CPU-facing register interface. `reg` must already be masked to 0-7,
    // i.e. addr & 0x0007 ($2000-$2007, mirrored through $3FFF).
    u8 cpuRead (u8 reg);
    void cpuWrite (u8 reg, u8 value);

    // Advances the PPU by exactly one PPU dot. The Bus calls this 3 times
    // per CPU cycle (NTSC's 3:1 PPU:CPU clock ratio).
    void clock();

    // Set for one clock() call when vblank begins with NMI enabled; the Bus
    // checks and clears this to fire CPU::nmi().
    bool nmiRequested = false;

    // Set for one clock() call at the end of each frame (scanline 241,
    // dot 1); the frontend polls and clears this to know when to present
    // the framebuffer.
    bool frameComplete = false;

    // 256x240 RGB888 framebuffer, row-major.
    u8 framebuffer[256 * 240 * 3];

    private:

    Cartridge* cartridge = nullptr;

    // 2K of physical nametable VRAM (the other 2K of the logical 4K comes
    // from mirroring -- see mirrorNametable()).
    u8 nametable[2048];
    u8 paletteRAM[32];
    u8 oam[256];
    u8 oamAddr = 0;

    // PPUCTRL / PPUMASK / PPUSTATUS
    u8 ctrl = 0;
    u8 mask = 0;
    u8 status = 0;

    // "Loopy" scroll registers -- see https://www.nesdev.org/wiki/PPU_scrolling
    // Layout: fineY(3) | nametableY(1) | nametableX(1) | coarseY(5) | coarseX(5)
    u16 v = 0;
    u16 t = 0;
    u8 fineX = 0;
    bool addrLatch = false;
    u8 dataBuffer = 0;

    int scanline = -1;
    int cycle = 0;

    void renderScanline();
    void incrementY();
    void transferX();
    void transferY();

    u8 ppuRead (u16 addr);
    void ppuWrite (u16 addr, u8 value);
    u16 mirrorNametable (u16 addr) const;

    // Standard NES (2C02, NTSC) 64-color reference palette.
    static const u8 palette[64][3];
};

#endif // _PPU_H_
