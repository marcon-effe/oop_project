#include "ErrorManager.h"
#include <QMessageBox> // se usi Qt
#include <iostream>

#include "ErrorManager.h"
#include <QMessageBox>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

void ErrorManager::showError(const std::string& message) {
    QMessageBox::critical(nullptr, "Errore", QString::fromStdString(message));
}

void ErrorManager::logError(const std::string& message) {
    std::cerr << "[LOG] " << message << std::endl; // stampa anche a terminale
    writeLog(message);
}

void ErrorManager::writeLog(const std::string& message) {
    std::ofstream logfile("error_log.txt", std::ios::app); // modalità append
    if (!logfile.is_open()) {
        std::cerr << "Impossibile aprire error_log.txt" << std::endl;
        return;
    }

    // Ottieni il timestamp attuale
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
#ifdef _WIN32
    localtime_s(&buf, &in_time_t); // Windows
#else
    localtime_r(&in_time_t, &buf); // Linux/macOS
#endif

    logfile << "[" << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << "] ";
    logfile << message << std::endl;

    logfile.close();
}