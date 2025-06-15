#ifndef CONSOLEAPP_H
#define CONSOLEAPP_H

#include <unordered_map>
#include "../model/artisti/Artista.h"

class ConsoleApp {
public:
    ConsoleApp();
    ~ConsoleApp();

    void run();
    std::unordered_map<unsigned int, Artista*> artisti;

private:
    void showMenu();
    void insertArtist();
    void modifyArtist();
    void saveData();
    void loadData();
    void deleteArtist();
    void printAllArtists() const;
    void clearAll();

    Artista* selectArtist();
    Artista* getArtistById(unsigned int id) const;
};

#endif // CONSOLEAPP_H