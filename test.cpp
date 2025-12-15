#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <cstdint>
#include <functional>

typedef void (*Fp)(uint8_t, uint16_t);

void lda (uint8_t mode, uint16_t value) {
    std::cout << "Mode: " << (int)mode << " | Value: " << value << std::endl;
}

int main(int argc, char* argv[]) {
    /*std::string file_name(argv[1]);

    std::ifstream file;
    file.open(file_name, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open file\n";
        return -1;
    }

    int byte;
    for (int i = 0; i < 16; ++i) {
        byte = file.get();
        unsigned char b = static_cast<unsigned char>(byte);
        std::cout << "Byte " << i << ": " << std::hex << (int)b << std::endl;
    }

    file.close();*/

    /*
    char ram[0x10000] = {0};
    std::cout << ram[0xFFFF] << std::endl;

    unsigned char t = '(';
    t += ')';
    std::cout << t << std::endl;*/

    /*
    enum inst { LDA, NUM_FUNCS };
    Fp instructions[NUM_FUNCS];

    instructions[LDA] = lda;

    std::cout << "LDA: " << LDA << std::endl;
    instructions[LDA](1, 16);
    */

    unsigned char t = 0b11110000;

    std::cout << (int8_t)t << std::endl;

    return 0;
}
