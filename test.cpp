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

  /*  uint16_t t = 0b1000000000000001;

    uint8_t d = t;
    std::cout << (int)d << std::endl;*/

    int t[10] = {0};
    t[3] = 0b11000000;
    int* p = &(t[3]);
    *p = *p >> 6;

    std::cout << *p << std::endl;
    std::cout << t[3] << std::endl;

/*
    uint8_t value = 0x3;
    uint8_t twos_comp = -(unsigned int)value;
    uint8_t A = 0x2;
    uint8_t C = 0;

    uint16_t diff = A + twos_comp;
    uint8_t diff_u8 = diff;

    std::cout << ((int)(diff) >= 256) << std::endl;
    std::cout << (int)diff << std::endl;
    std::cout << (int)diff_u8 << std::endl;
    std::cout << (int)twos_comp << std::endl;
*/

    return 0;
}
