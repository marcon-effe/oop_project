#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <vector>

#include "../model/artisti/Artista.h"

// Gerarchia prodotti
#include "../model/core/ArtistProduct.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/tour/Tour.h"
#include "../model/merch/TShirt.h"
#include "../model/merch/Disco.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"

#include "../include/data_format.h"

class DataManager {
    public:
        static bool saveToFileJson(const std::vector<Artista*>& artisti, const std::string& filePath);
        static std::vector<Artista*> loadFromFileJson(const std::string& filePath);
        
        static bool validateXmlWithSchema(const std::string& xmlFilePath, const std::string& schemaFilePath);
        static bool saveToFileXml(const std::vector<Artista*>& artisti, const std::string& filePath);
        static std::vector<Artista*> loadFromFileXml(const std::string& filePath);

};
#endif
