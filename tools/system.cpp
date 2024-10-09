#include <iostream>
#include <fstream>
#include <string>

#include "json.hpp"
using json = nlohmann::json;

int main(int argc, const char* argv[]) {
    if ( argc < 3) {
        std::cout << "build system json, need input and output!" << std::endl;
        return -1;
    }

    std::ostringstream ss;
    {
        std::ifstream iif(argv[1]);
        ss << iif.rdbuf();
    }

    json system;
    json role;
    role["role"] = "system";
    role["content"] = ss.str();

    system.push_back(role);
    std::ofstream oof(argv[2]);
    oof << std::setw(4) << system.dump() << std::endl;
}

