#include "dataManager.h"


//JSON
// Funzione per salvare i dati in un file JSON
bool DataManager::saveToFileJson(const std::vector<Artista*>& artisti, const std::string& filePath) {
    QJsonArray jsonArray;
    for (const Artista* a : artisti) {
        jsonArray.append(a->toJson());
    }

    QJsonObject json;
    json["artisti"] = jsonArray;

    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Errore: impossibile aprire il file per la scrittura -> " << filePath.c_str();
        return false;
    }

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();
    
    return true;
}
// Funzione per caricare i dati da un file JSON
std::vector<Artista*> DataManager::loadFromFileJson(const std::string& filePath) {
    std::vector<Artista*> artisti;

    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Errore: impossibile aprire il file per la lettura ->" << filePath.c_str();
        return artisti;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Errore di parsing JSON:" << parseError.errorString();
        return artisti;
    }

    QJsonObject root = doc.object();
    QJsonArray jsonArtisti = root["artisti"].toArray();

    for (const QJsonValue& artistaVal : jsonArtisti) {
        if (!artistaVal.isObject()) continue;

        QJsonObject artistaObj = artistaVal.toObject();
        Artista* a = new Artista(artistaObj);
        artisti.push_back(a);
    }

    return artisti;
}


//XML
// Funzione per validare un file XML con uno schema XSD
bool DataManager::validateXmlWithSchema(const std::string& xmlPath, const std::string& xsdPath){
    // 1. Assicurati che entrambi i percorsi siano assoluti
    QString xmlPathAbs    = QFileInfo(QString::fromStdString(xmlPath)).absoluteFilePath();
    QString schemaPathAbs = QFileInfo(QString::fromStdString(xsdPath)).absoluteFilePath();

    QProcess validator;
    // 2. Aggiungi --noout per sopprimere l’output sul documento valido
    QStringList arguments { "--noout", "--schema", schemaPathAbs, xmlPathAbs };
    validator.start("xmllint", arguments);
    if (!validator.waitForStarted()) {
    qWarning() << "Impossibile avviare xmllint:" << validator.errorString();
    return false;
    }
    validator.waitForFinished();

    int exitCode = validator.exitCode();
    // 3. Leggi **sia** stderr **sia** stdout
    QByteArray stdOut = validator.readAllStandardOutput();
    QByteArray stdErr = validator.readAllStandardError();

    if (exitCode != 0) {
    qWarning() << "Validazione XML fallita, exitCode =" << exitCode;
    if (!stdErr.isEmpty())
    qWarning() << "stderr:" << stdErr;
    if (!stdOut.isEmpty())
    qWarning() << "stdout:" << stdOut;
    return false;
    }

    return true;
}


// Funzione per salvare i dati in un file XML
bool DataManager::saveToFileXml(const std::vector<Artista*>& artisti, const std::string& filePath) {
    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Errore: impossibile aprire il file per la scrittura -> " << filePath.c_str();
        return false;
    }

    QDomDocument doc;
    QDomElement root = doc.createElement("artisti");
    root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xsi:noNamespaceSchemaLocation", "artisti.xsd");
    doc.appendChild(root);

    for (const Artista* a : artisti) {
        QDomElement artistaElement = a->toXml(doc);  // usa il document condiviso
        root.appendChild(artistaElement);            // non serve importNode
    }

    QTextStream out(&file);
    doc.save(out, 4); // indentazione
    file.close();

    return true;
}
// Funzione per caricare i dati da un file XML
std::vector<Artista*> DataManager::loadFromFileXml(const std::string& filePath) {
    std::vector<Artista*> artisti;

    const std::string schemaPath = filePath.substr(0, filePath.find_last_of("/\\") + 1) + "artisti.xsd";

    if (!validateXmlWithSchema(filePath, schemaPath)) {
        qWarning() << "File XML non valido secondo lo schema.";
        return artisti;
    }

    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Errore apertura file: " << filePath.c_str();
        return artisti;
    }

    QDomDocument doc;
    if (!doc.setContent(&file)) {
        qWarning() << "Errore parsing XML";
        file.close();
        return artisti;
    }

    QDomElement root = doc.documentElement();
    QDomNodeList artistiNodes = root.elementsByTagName("Artista");

    for (int i = 0; i < artistiNodes.count(); ++i) {
        QDomElement artistaEl = artistiNodes.at(i).toElement();
        Artista* a = new Artista(artistaEl);
        artisti.push_back(a);
    }

    file.close();
    return artisti;
}