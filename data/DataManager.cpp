#include "DataManager.h"
#include <stdexcept>
#include <cassert>
#include <unordered_map>


std::string DataManager::sanitizeForPath(const std::string& raw) {
    std::string cleaned = raw;

    // Rimuove caratteri illegali
    static const std::regex illegal(R"([\\/:*?"<>|])");
    cleaned = std::regex_replace(cleaned, illegal, "");

    // Rimuove spazi all'inizio/fine
    cleaned.erase(0, cleaned.find_first_not_of(" \t\n\r\f\v"));
    cleaned.erase(cleaned.find_last_not_of(" \t\n\r\f\v") + 1);

    // Sostituisce multipli spazi con _
    static const std::regex spaces(R"(\s+)");
    cleaned = std::regex_replace(cleaned, spaces, "_");

    // Blacklist parole riservate (Windows)
    static const std::set<std::string> reserved = {
        "con", "prn", "aux", "nul", "com1", "com2", "com3",
        "lpt1", "lpt2", "lpt3"
    };
    std::string lower;
    std::transform(cleaned.begin(), cleaned.end(), std::back_inserter(lower), ::tolower);
    if (reserved.count(lower)) cleaned += "_";

    return cleaned;
}

void DataManager::clearIconsDirectory() {
    QDir iconsDir("view/icons/");
    if (!iconsDir.exists()) return;

    QStringList entries = iconsDir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    for (const QString& entry : entries) {
        if (entry == "placeholder.png" || entry == "info.png") continue;

        QString fullPath = iconsDir.absoluteFilePath(entry);
        QFileInfo info(fullPath);

        if (info.isDir()) {
            QDir(fullPath).removeRecursively();
        } else {
            QFile::remove(fullPath);
        }
    }
}

// JSON
// Funzione per salvare i dati in un file JSON
bool DataManager::saveToFileJson(const std::unordered_map<unsigned int, Artista*>& artisti, const std::string& filePath, bool reduced) {
    QJsonArray jsonArray;
    for (const auto& pair : artisti) {
        assert(pair.second != nullptr);
        jsonArray.append(pair.second->toJson(reduced));
    }

    QJsonObject json;
    json["artisti"] = jsonArray;

    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::WriteOnly)) {
        throw std::runtime_error("Impossibile aprire il file per la scrittura: " + filePath);
    }

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    return true;
}

// Funzione per caricare i dati da un file JSON
std::unordered_map<unsigned int, Artista*> DataManager::loadFromFileJson(const std::string& filePath) {
    std::unordered_map<unsigned int, Artista*> artisti;

    // Pulisce la cartella view/icons/ mantenendo solo placeholder.png e info.png
    DataManager::clearIconsDirectory();

    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Impossibile aprire il file per la lettura: " + filePath);
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        throw std::runtime_error("Errore di parsing JSON: " + parseError.errorString().toStdString());
    }

    QJsonObject root = doc.object();
    QJsonArray jsonArtisti = root["artisti"].toArray();

    for (const QJsonValue& artistaVal : jsonArtisti) {
        if (!artistaVal.isObject()) continue;

        QJsonObject artistaObj = artistaVal.toObject();
        Artista* a = Artista::createFromJson(artistaObj);
        unsigned int id = a->getId();
        artisti[id] = a;
    }

    return artisti;
}

// XML
// Funzione per validare un file XML con uno schema XSD
bool DataManager::validateXmlWithSchema(const std::string& xmlPath, const std::string& xsdPath) {
    QString xmlPathAbs = QFileInfo(QString::fromStdString(xmlPath)).absoluteFilePath();
    QString schemaPathAbs = QFileInfo(QString::fromStdString(xsdPath)).absoluteFilePath();

    QProcess validator;
    QStringList arguments { "--noout", "--schema", schemaPathAbs, xmlPathAbs };
    validator.start("xmllint", arguments);
    if (!validator.waitForStarted()) {
        throw std::runtime_error("Impossibile avviare xmllint: " + validator.errorString().toStdString());
    }
    validator.waitForFinished();

    int exitCode = validator.exitCode();
    QByteArray stdOut = validator.readAllStandardOutput();
    QByteArray stdErr = validator.readAllStandardError();

    if (exitCode != 0) {
        std::string errorMsg = "Validazione XML fallita, exitCode = " + std::to_string(exitCode);
        if (!stdErr.isEmpty())
            errorMsg += "\nstderr: " + stdErr.toStdString();
        if (!stdOut.isEmpty())
            errorMsg += "\nstdout: " + stdOut.toStdString();
        throw std::runtime_error(errorMsg);
    }

    return true;
}

// Funzione per salvare i dati in un file XML
bool DataManager::saveToFileXml(const std::unordered_map<unsigned int, Artista*>& artisti, const std::string& filePath, bool reduced) {
    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("Impossibile aprire il file per la scrittura: " + filePath);
    }

    QDomDocument doc;
    QDomElement root = doc.createElement("artisti");
    root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xsi:noNamespaceSchemaLocation", "artisti.xsd");
    doc.appendChild(root);

    for (const auto& pair : artisti) {
        assert(pair.second != nullptr);
        QDomElement artistaElement = pair.second->toXml(doc, reduced);
        root.appendChild(artistaElement);
    }

    QTextStream out(&file);
    doc.save(out, 4);
    file.close();

    return true;
}

std::unordered_map<unsigned int, Artista*> DataManager::loadFromFileXml(const std::string& filePath) {
    std::unordered_map<unsigned int, Artista*> artisti;

    // Pulisce la cartella view/icons/ mantenendo solo placeholder.png e info.png
    DataManager::clearIconsDirectory();

    // Costruisce un percorso relativo all'eseguibile (che nel progetto è la cartella "root")
    // QCoreApplication::applicationDirPath() restituisce, di solito, la cartella dell'eseguibile.
    QString exeDir = QCoreApplication::applicationDirPath();
    QDir dir(exeDir);

    // Se il binario si trova in build‐Debug/, saliamo di un livello per arrivare a root/
    if (dir.dirName().startsWith("build")) {
        dir.cdUp(); // ora dir punta a root del progetto
    }
    // A questo punto costruiamo il path completo per l’XSD sotto root/saves/xml/
    QString schemaPathQt = dir.filePath("saves/xml/artisti.xsd");
    std::string schemaPath = schemaPathQt.toStdString();

    if (!validateXmlWithSchema(filePath, schemaPath)) {
        throw std::runtime_error("File XML non valido secondo lo schema.");
    }

    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Errore apertura file: " + filePath);
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        file.close();
        throw std::runtime_error("Errore parsing XML");
    }

    QDomElement root = doc.documentElement();
    QDomNodeList artistiNodes = root.elementsByTagName("Artista");

    for (int i = 0; i < artistiNodes.count(); ++i) {
        QDomElement artistaEl = artistiNodes.at(i).toElement();
        Artista* a = Artista::createFromXml(artistaEl);
        artisti[a->getId()] = a;
    }

    file.close();
    return artisti;
}


void DataManager::cleanUpArtist(std::unordered_map<unsigned int, Artista*>& artisti) {
    for(auto& pair : artisti) {
        delete pair.second;
    }
    artisti.clear();
}

void DataManager::cleanUpProducts(std::unordered_map<unsigned int, ArtistProduct*>& products) {
    for (auto& kv : products) {
        delete kv.second;
    }
    products.clear();
}