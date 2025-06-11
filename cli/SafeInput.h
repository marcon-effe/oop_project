#ifndef SAFE_INPUT_H
#define SAFE_INPUT_H

#include <iostream>
#include <limits>
#include <string>

class SafeInput {
public:
    template<typename T>
    static T read(const std::string& prompt) {
        T value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            std::cout << "Input non valido, riprova.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    static bool readBool(const std::string& prompt) {
        int tmp = read<int>(prompt + " (1=sì, 0=no): ");
        return tmp != 0;
    }
};
#endif