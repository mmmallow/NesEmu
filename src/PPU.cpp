#include "PPU.h"
#include <cstring>

const u8 PPU::palette[64][3] = {
    {84,84,84},    {0,30,116},    {8,16,144},    {48,0,136},    {68,0,100},    {92,0,48},     {84,4,0},      {60,24,0},
    {32,42,0},     {8,58,0},      {0,64,0},      {0,60,0},      {0,50,60},     {0,0,0},       {0,0,0},       {0,0,0},
    {152,150,152}, {8,76,196},    {48,50,236},   {92,30,228},   {136,20,176},  {160,20,100},  {152,34,32},   {120,60,0},
    {84,90,0},     {40,114,0},    {8,124,0},     {0,118,40},    {0,102,120},   {0,0,0},       {0,0,0},       {0,0,0},
    {236,238,236}, {76,154,236},  {120,124,236}, {176,98,236},  {228,84,236},  {236,88,180},  {236,106,100}, {212,136,32},
    {160,170,0},   {116,196,0},   {76,208,32},   {56,204,108},  {56,180,204},  {60,60,60},    {0,0,0},       {0,0,0},
    {236,238,236}, {168,204,236}, {188,188,236}, {212,178,236}, {236,174,236}, {236,174,212}, {236,180,176}, {228,196,144},
    {204,210,120}, {180,222,120}, {168,226,144}, {152,226,180}, {160,214,228}, {160,162,160}, {0,0,0},       {0,0,0},
};

PPU::PPU() {
    std::memset(nametable, 0, sizeof(nametable));
    std::memset(paletteRAM, 0, sizeof(paletteRAM));
    std::memset(oam, 0, sizeof(oam));
    std::memset(framebuffer, 0, sizeof(framebuffer));
}

u16 PPU::mirrorNametable (u16 addr) const {
    addr &= 0x0FFF;
    u16 table = addr / 0x400;
    u16 offset = addr % 0x400;

    bool vertical = cartridge && cartridge->mirroring == Mirroring::Vertical;
    // Vertical mirroring: tables 0/2 share physical bank 0, 1/3 share bank 1.
    // Horizontal mirroring: tables 0/1 share physical bank 0, 2/3 share bank 1.
    u16 physicalTable = vertical ? (table & 0x01) : (table >> 1);
    return (physicalTable * 0x400) + offset;
}

u8 PPU::ppuRead (u16 addr) {
    addr &= 0x3FFF;

    u8 value = 0;
    if (addr <= 0x1FFF) {
        if (cartridge) cartridge->ppuRead(addr, value);
    }
    else if (addr <= 0x3EFF) {
        value = nametable[mirrorNametable(addr)];
    }
    else {
        addr &= 0x1F;
        if (addr == 0x10 || addr == 0x14 || addr == 0x18 || addr == 0x1C)
            addr -= 0x10;
        value = paletteRAM[addr];
    }

    return value;
}

void PPU::ppuWrite (u16 addr, u8 value) {
    addr &= 0x3FFF;

    if (addr <= 0x1FFF) {
        if (cartridge) cartridge->ppuWrite(addr, value);
    }
    else if (addr <= 0x3EFF) {
        nametable[mirrorNametable(addr)] = value;
    }
    else {
        addr &= 0x1F;
        if (addr == 0x10 || addr == 0x14 || addr == 0x18 || addr == 0x1C)
            addr -= 0x10;
        paletteRAM[addr] = value;
    }
}

u8 PPU::cpuRead (u8 reg) {
    u8 value = 0;

    switch (reg) {
        case 2: // PPUSTATUS
            value = status;
            status &= ~0x80; // clear vblank
            addrLatch = false;
            break;
        case 4: // OAMDATA
            value = oam[oamAddr];
            break;
        case 7: { // PPUDATA (reads are buffered one behind, except palette)
            u16 addr = v & 0x3FFF;
            value = dataBuffer;
            dataBuffer = ppuRead(v);
            if (addr >= 0x3F00)
                value = dataBuffer;
            v += (ctrl & 0x04) ? 32 : 1;
            break;
        }
        default:
            break;
    }

    return value;
}

void PPU::cpuWrite (u8 reg, u8 value) {
    switch (reg) {
        case 0: // PPUCTRL
            ctrl = value;
            t = (t & 0xF3FF) | ((value & 0x03) << 10);
            break;
        case 1: // PPUMASK
            mask = value;
            break;
        case 3: // OAMADDR
            oamAddr = value;
            break;
        case 4: // OAMDATA
            oam[oamAddr++] = value;
            break;
        case 5: // PPUSCROLL
            if (!addrLatch) {
                fineX = value & 0x07;
                t = (t & 0xFFE0) | (value >> 3);
                addrLatch = true;
            }
            else {
                t = (t & 0x8FFF) | ((u16)(value & 0x07) << 12);
                t = (t & 0xFC1F) | ((u16)(value & 0xF8) << 2);
                addrLatch = false;
            }
            break;
        case 6: // PPUADDR
            if (!addrLatch) {
                t = (t & 0x00FF) | ((u16)(value & 0x3F) << 8);
                addrLatch = true;
            }
            else {
                t = (t & 0xFF00) | value;
                v = t;
                addrLatch = false;
            }
            break;
        case 7: // PPUDATA
            ppuWrite(v, value);
            v += (ctrl & 0x04) ? 32 : 1;
            break;
        default:
            break;
    }
}

void PPU::incrementY() {
    if ((v & 0x7000) != 0x7000) {
        v += 0x1000;
    }
    else {
        v &= ~0x7000;
        u16 y = (v & 0x03E0) >> 5;
        if (y == 29) {
            y = 0;
            v ^= 0x0800;
        }
        else if (y == 31) {
            y = 0;
        }
        else {
            y++;
        }
        v = (v & ~0x03E0) | (y << 5);
    }
}

void PPU::transferX() {
    // Copy horizontal bits (coarse X, nametable X) from t into v.
    v = (v & 0xFBE0) | (t & 0x041F);
}

void PPU::transferY() {
    // Copy vertical bits (fine Y, coarse Y, nametable Y) from t into v.
    v = (v & 0x841F) | (t & 0x7BE0);
}

void PPU::renderScanline() {
    bool showBg = mask & 0x08;
    bool showSprites = mask & 0x10;

    // Tracks which background pixels were non-transparent, for sprite
    // priority and sprite-0-hit checks.
    u8 bgOpaque[256] = {0};

    if (showBg) {
        u16 fineYVal = (v >> 12) & 0x7;
        u16 coarseY = (v >> 5) & 0x1F;
        u16 ntY = (v >> 11) & 0x1;
        u16 coarseX = v & 0x1F;
        u16 ntX = (v >> 10) & 0x1;

        u16 bgPatternBase = (ctrl & 0x10) ? 0x1000 : 0x0000;

        int x = 0;
        u8 fineXOffset = fineX;
        while (x < 256) {
            u16 ntAddr = 0x2000 | (ntY << 11) | (ntX << 10) | (coarseY << 5) | coarseX;
            u8 tileIndex = ppuRead(ntAddr);

            u16 attrAddr = 0x23C0 | (ntY << 11) | (ntX << 10) | ((coarseY >> 2) << 3) | (coarseX >> 2);
            u8 attrByte = ppuRead(attrAddr);
            u8 shift = ((coarseY & 0x02) << 1) | (coarseX & 0x02);
            u8 palIndex = (attrByte >> shift) & 0x03;

            u16 patternAddr = bgPatternBase + tileIndex * 16 + fineYVal;
            u8 lo = ppuRead(patternAddr);
            u8 hi = ppuRead(patternAddr + 8);

            for (int px = fineXOffset; px < 8 && x < 256; ++px, ++x) {
                u8 bit = 7 - px;
                u8 colorBits = (((hi >> bit) & 1) << 1) | ((lo >> bit) & 1);

                if (colorBits != 0)
                    bgOpaque[x] = 1;

                u8 paletteAddr = colorBits == 0 ? 0x00 : (palIndex * 4 + colorBits);
                u8 paletteEntry = ppuRead(0x3F00 + paletteAddr) & 0x3F;
                int fbIdx = (scanline * 256 + x) * 3;
                framebuffer[fbIdx + 0] = palette[paletteEntry][0];
                framebuffer[fbIdx + 1] = palette[paletteEntry][1];
                framebuffer[fbIdx + 2] = palette[paletteEntry][2];
            }
            fineXOffset = 0;

            coarseX++;
            if (coarseX > 31) {
                coarseX = 0;
                ntX ^= 1;
            }
        }
    }
    else {
        u8 paletteEntry = ppuRead(0x3F00) & 0x3F;
        for (int x = 0; x < 256; ++x) {
            int fbIdx = (scanline * 256 + x) * 3;
            framebuffer[fbIdx + 0] = palette[paletteEntry][0];
            framebuffer[fbIdx + 1] = palette[paletteEntry][1];
            framebuffer[fbIdx + 2] = palette[paletteEntry][2];
        }
    }

    if (showSprites) {
        struct SpriteMatch { u8 index; u8 y; u8 tile; u8 attr; u8 x; };
        SpriteMatch matches[8];
        int found = 0;

        for (int i = 0; i < 64 && found < 8; ++i) {
            u8 spriteY = oam[i * 4 + 0];
            int rowInSprite = scanline - spriteY;
            if (rowInSprite >= 0 && rowInSprite < 8) {
                matches[found] = { (u8)i, spriteY, oam[i * 4 + 1], oam[i * 4 + 2], oam[i * 4 + 3] };
                found++;
            }
        }

        u16 spritePatternBase = (ctrl & 0x08) ? 0x1000 : 0x0000;

        // Draw back-to-front (highest OAM index first) so sprite 0, if
        // present, ends up drawn last -- i.e. on top, matching hardware
        // priority (lower OAM index wins overlaps).
        for (int m = found - 1; m >= 0; --m) {
            SpriteMatch& s = matches[m];
            int rowInSprite = scanline - s.y;
            bool flipV = s.attr & 0x80;
            bool flipH = s.attr & 0x40;
            bool behindBg = s.attr & 0x20;
            u8 palIndex = s.attr & 0x03;

            int patRow = flipV ? (7 - rowInSprite) : rowInSprite;
            u16 patternAddr = spritePatternBase + s.tile * 16 + patRow;
            u8 lo = ppuRead(patternAddr);
            u8 hi = ppuRead(patternAddr + 8);

            for (int px = 0; px < 8; ++px) {
                int screenX = s.x + px;
                if (screenX > 255)
                    continue;

                u8 bit = flipH ? px : (7 - px);
                u8 colorBits = (((hi >> bit) & 1) << 1) | ((lo >> bit) & 1);
                if (colorBits == 0)
                    continue;

                if (s.index == 0 && bgOpaque[screenX] && showBg && screenX != 255)
                    status |= 0x40; // sprite 0 hit

                if (behindBg && bgOpaque[screenX])
                    continue;

                u8 paletteEntry = ppuRead(0x3F10 + palIndex * 4 + colorBits) & 0x3F;
                int fbIdx = (scanline * 256 + screenX) * 3;
                framebuffer[fbIdx + 0] = palette[paletteEntry][0];
                framebuffer[fbIdx + 1] = palette[paletteEntry][1];
                framebuffer[fbIdx + 2] = palette[paletteEntry][2];
            }
        }
    }
}

void PPU::clock() {
    if (scanline >= 0 && scanline <= 239 && cycle == 0) {
        renderScanline();
    }

    if ((scanline >= 0 && scanline <= 239) || scanline == -1) {
        if (cycle == 256) incrementY();
        if (cycle == 257) transferX();
        if (scanline == -1 && cycle == 280) transferY();
    }

    if (scanline == 241 && cycle == 1) {
        status |= 0x80;
        if (ctrl & 0x80)
            nmiRequested = true;
        frameComplete = true;
    }

    if (scanline == -1 && cycle == 1) {
        status &= ~0xE0; // clear vblank, sprite 0 hit, sprite overflow
    }

    cycle++;
    if (cycle > 340) {
        cycle = 0;
        scanline++;
        if (scanline > 260) {
            scanline = -1;
        }
    }
}
