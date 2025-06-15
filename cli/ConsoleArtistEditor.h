#ifndef CONSOLEARTISTEDITOR_H
#define CONSOLEARTISTEDITOR_H

#include "../model/artisti/Artista.h"
#include <unordered_map>

class ConsoleArtistEditor {
public:
    static void modificaArtista(const std::unordered_map<unsigned int, Artista*>& artisti, Artista* artista);
};

#endif
