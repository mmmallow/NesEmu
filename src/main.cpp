#include <iostream>
#include <memory>
#include "Bus.h"
#include "Cartridge.h"

int main (int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    std::string file_name("test_progs/run.6502.nes");

    auto cartridge = std::make_unique<Cartridge>(file_name);
    if (!cartridge->valid) {
        std::cerr << "Failed to load cartridge: " << file_name << std::endl;
        return 1;
    }

    Bus bus;
    bus.insertCartridge(std::move(cartridge));
    bus.reset();

    while (true) {
        bus.clock();
    }

    return 0;
}
