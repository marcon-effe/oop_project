#ifndef SAFE_INPUT_H
#define SAFE_INPUT_H

#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

class SafeInput {
public:
    // Legge un valore di tipo T (es. int, double), ripetendo il prompt finché non è valido
    template<typename T>
    static typename std::enable_if<!std::is_same<T, std::string>::value, T>::type
    read(const std::string& prompt) {
        T value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Input non valido, riprova.\n";
        }
    }

    // Legge una riga di testo (anche vuota) senza ulteriori validazioni
    static std::string read(const std::string& prompt) {
        std::string line;
        std::cout << prompt;
        std::getline(std::cin, line);
        return line;
    }

    // Legge un booleano tramite 1 (sì) o 0 (no)
    static bool readBool(const std::string& prompt) {
        int tmp = read<int>(prompt);
        return tmp != 0;
    }
};

#endif
