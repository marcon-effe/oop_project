#ifndef ERROR_MANAGER_H
#define ERROR_MANAGER_H
#include <string>

#include <string>

class ErrorManager {
public:
    static void showError(const std::string& message);
    static void logError(const std::string& message);
private:
    static void writeLog(const std::string& message);
};

#endif // ERROR_MANAGER_H