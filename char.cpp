#include <iostream>
#include <string>

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            std::cout << std::endl;
            continue;
        }
        std::cout << line[0];   
        for (size_t i = 1; i < line.length(); i++) {
            if (line[i] > line[i - 1]) {
                std::cout << line[i];
            } else {
                std::cout << '_';
            }
        }
        std::cout << std::endl;
    }
    return 0;
}
