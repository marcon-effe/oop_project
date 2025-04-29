#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <unordered_map>
#include <string>

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
    // JSON
    static bool saveToFileJson(const std::unordered_map<unsigned int, Artista*>& artisti, const std::string& filePath);
    static std::unordered_map<unsigned int, Artista*> loadFromFileJson(const std::string& filePath);
    
    // XML
    static bool validateXmlWithSchema(const std::string& xmlFilePath, const std::string& schemaFilePath);
    static bool saveToFileXml(const std::unordered_map<unsigned int, Artista*>& artisti, const std::string& filePath);
    static std::unordered_map<unsigned int, Artista*> loadFromFileXml(const std::string& filePath);
};

#endif // DATAMANAGER_H