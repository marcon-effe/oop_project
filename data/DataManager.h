#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QDomDocument>
#include <QDomElement>
#include <QDomAttr>
#include <QTextStream>
#include <QProcess>
#include <QFileInfo>
#include <QDir>

#include <unordered_map>
#include <string>
#include <regex>
#include <set>
#include <string>
#include <algorithm>

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

class DataManager {
private:
    static void clearIconsDirectory();
public:
    static std::string sanitizeForPath(const std::string& raw);

    // JSON
    static bool saveToFileJson(const std::unordered_map<unsigned int, Artista*>& artisti, const std::string& filePath);
    static std::unordered_map<unsigned int, Artista*> loadFromFileJson(const std::string& filePath);
    
    // XML
    static bool validateXmlWithSchema(const std::string& xmlFilePath, const std::string& schemaFilePath);
    static bool saveToFileXml(const std::unordered_map<unsigned int, Artista*>& artisti, const std::string& filePath);
    static std::unordered_map<unsigned int, Artista*> loadFromFileXml(const std::string& filePath);
};

#endif // DATAMANAGER_H