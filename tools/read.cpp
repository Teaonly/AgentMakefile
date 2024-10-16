#include <iostream>
#include <fstream>

int main(int argc, const char* argv[]) {
    if ( argc < 3) {
        std::cout << "read error, please provide prefix and output file!" << std::endl;
        return -1;
    }

    // print user prefix
    std::cout << argv[1] ;
    std::cout.flush();

    std::string line;
    std::ofstream out(argv[2]);
    bool no_input = true;
    while (std::getline(std::cin, line)) {      
        out << line << std::endl;
        no_input = false;
    }
    out.close();
    if ( no_input ) {
        return -1;
    }
    return 0;
}