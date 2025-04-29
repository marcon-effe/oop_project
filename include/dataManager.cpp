#include "dataManager.h"
#include <stdexcept>
#include <cassert>
#include <unordered_map>

// JSON
// Funzione per salvare i dati in un file JSON
bool DataManager::saveToFileJson(const std::unordered_map<unsigned int, Artista*>& artisti, const std::string& filePath) {
    QJsonArray jsonArray;
    for (const auto& pair : artisti) {
        assert(pair.second != nullptr);
        jsonArray.append(pair.second->toJson());
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
bool DataManager::saveToFileXml(const std::unordered_map<unsigned int, Artista*>& artisti, const std::string& filePath) {
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
        QDomElement artistaElement = pair.second->toXml(doc);
        root.appendChild(artistaElement);
    }

    QTextStream out(&file);
    doc.save(out, 4);
    file.close();

    return true;
}

// Funzione per caricare i dati da un file XML
std::unordered_map<unsigned int, Artista*> DataManager::loadFromFileXml(const std::string& filePath) {
    std::unordered_map<unsigned int, Artista*> artisti;

    const std::string schemaPath = filePath.substr(0, filePath.find_last_of("/\\") + 1) + "artisti.xsd";

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
        Artista* a = new Artista(artistaEl);
        unsigned int id = a->getId();
        artisti[id] = a;
    }

    file.close();
    return artisti;
}