#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char* argv[]) {
    if ( argc < 2) {
        std::cout << "read error, please provide output file!" << std::endl;
        return -1;
    }
    std::string line;
    std::ofstream out(argv[1]);
    bool no_input = true;
    while (std::getline(std::cin, line)) { 
        std::cerr << " ====== " << line << std::endl;
        if ( line.rfind("==USEREND==") == 0) {
            break;
        }
        if ( line.rfind("==USERBYE==") == 0) {
            break;
        }
        out << line << std::endl;
        no_input = false;
    }
    out.close();
    if ( no_input ) {
        return -1;
    }
}