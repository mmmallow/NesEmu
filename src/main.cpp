#include <iostream>
#include <fstream>
#include "CPU.h"
#include "tests/InstructionsTest.h"

void loadData (CPU& c) {
    std::string file_name("test_progs/run.6502.nes");

    std::ifstream file;
    file.open(file_name, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open file\n";
    }

    // Read header
    u8 byte;
    for (int i = 0; i < 16; ++i) {
        byte = file.get();
        std::cout << "Byte " << i << ": " << std::hex << byte << std::endl;
    }

    // Load PRG-ROM
    u16 offset = 0x8000;
    for (int i = 0; i < 0x3FFA; ++i) {
        byte = file.get();

        c.mem[offset + i] = byte;
    }

    // Load vectors
    for (int i = 0xFFFA; i < 0xFFFF; ++i) {
        byte = file.get();
        c.mem[i] = byte;
    }

    file.close();
}

int main (int argc, char* argv[]) {
    CPU c(1, 0);

    loadData(c);

    c.reset();

    while (true) {
        u8 instruction = c.mem[c.PC];
        auto instruct = c.instructions[instruction];
        auto command = instruct.instruction;
        (c.*command)();
    }

    return 0;
}
