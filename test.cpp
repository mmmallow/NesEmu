#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char* argv[]) {
    std::string file_name(argv[1]);

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

    file.close();

    return 0;
}
