#include <iostream>

#include "ArtistProduct.h"
#include "../artisti/Artista.h"
#include "../musica/Album.h"
#include "../musica/Singolo.h"
#include "../tour/Tour.h"
#include "../merch/TShirt.h"
#include "../merch/CD.h"
#include "../merch/Vinile.h"

#include "../../data/DataManager.h"
#include "../../view/ErrorManager.h"

unsigned int ArtistProduct::nextProductId = 1;
std::mutex ArtistProduct::idMutex;

ArtistProduct::ArtistProduct(Artista* owner, const std::string& t, const std::string& desc)
    : ID(generateId()), title(t), description(desc), imagePath(""), imageB64("")
{
    if (!owner) {
        assert(false && "Artista pointer must not be nullptr");
        throw std::invalid_argument("ArtistProduct constructor received nullptr Artista pointer.");
    }

    setOwner(owner); // imposta id_artist e nomeArtista
}

ArtistProduct::ArtistProduct(Artista* owner, const std::string& t, const std::string& d, const std::string& i)
    : ID(generateId()), title(t), description(d), imagePath(""), imageB64("")
{
    if (!owner) {
        assert(false && "Artista pointer must not be nullptr");
        throw std::invalid_argument("ArtistProduct constructor received nullptr Artista pointer.");
    }

    setOwner(owner);       // imposta id_artist e nomeArtista
    setImagePath(i);       // crea fisicamente l'immagine, aggiorna imagePath e imageB64
}

ArtistProduct::ArtistProduct(const ArtistProduct* p)
    : ID(generateId())
{
    if (!p) {
        assert(false && "ArtistProduct pointer must not be nullptr");
        throw std::invalid_argument("ArtistProduct copy constructor received nullptr.");
    }

    title = p->getTitle();
    description = p->getDescription();
    imagePath = p->getImagePath();
    imageB64 = p->imageB64;
    nomeArtista = p->nomeArtista;
    id_artist = p->getArtistId();
}

ArtistProduct::~ArtistProduct() {}

unsigned int ArtistProduct::generateId() {
    std::lock_guard<std::mutex> lock(idMutex);
    return nextProductId++;
}

unsigned int ArtistProduct::getId() const {
    return ID;
}

unsigned int ArtistProduct::getArtistId() const {
    return id_artist;
}

void ArtistProduct::setOwner(Artista* nuovoArtista) {
    if (!nuovoArtista) {
        assert(false && "Artista pointer must not be nullptr");
        throw std::invalid_argument("ArtistProduct::setOwner() received nullptr.");
    }

    std::string nuovoNome = nuovoArtista->getNome();
    QString sanitizedNew = QString::fromStdString(DataManager::sanitizeForPath(nuovoNome));
    QString sanitizedOld = QString::fromStdString(DataManager::sanitizeForPath(nomeArtista));

    id_artist = nuovoArtista->getId(); // aggiorna sempre l'ID

    if (nomeArtista == nuovoNome) {
        return; // già aggiornato
    }

    // Aggiorna solo nome se non c'è immagine
    if (imagePath.empty()) {
        nomeArtista = nuovoNome;
        return;
    }

    // Costruzione path immagine
    QString ext = QFileInfo(QString::fromStdString(imagePath)).suffix();
    QString fileName = QString::fromStdString(DataManager::sanitizeForPath(title)) + "." + ext;

    QString oldPath = "view/icons/" + sanitizedOld + "/" + fileName;
    QString newFolder = "view/icons/" + sanitizedNew;
    QString newPath = newFolder + "/" + fileName;

    // Sposta il file fisicamente se esiste
    if (QFile::exists(oldPath)) {
        QDir().mkpath(newFolder);
        QFile::remove(newPath); // sovrascrive eventuale duplicato
        QFile::rename(oldPath, newPath);
    }

    // Aggiorna dati interni
    imagePath = newPath.toStdString();
    nomeArtista = nuovoNome;

    // Ricarica contenuto B64 aggiornato
    QFile copied(newPath);
    if (copied.open(QIODevice::ReadOnly)) {
        QByteArray data = copied.readAll();
        copied.close();
        imageB64 = data.toBase64().toStdString();
    } else {
        imageB64.clear();
    }
}

std::string ArtistProduct::getNomeArtista() const {
    return nomeArtista;
}

std::string ArtistProduct::getTitle() const { 
    return title; 
}

void ArtistProduct::setTitle(const std::string& t) {
    if (title == t) return;

    if (!imagePath.empty()) {
        QString ext = QFileInfo(QString::fromStdString(imagePath)).suffix();
        QString sanitizedArtist = QString::fromStdString(DataManager::sanitizeForPath(nomeArtista));
        QString oldSanitizedTitle = QString::fromStdString(DataManager::sanitizeForPath(title));
        QString newSanitizedTitle = QString::fromStdString(DataManager::sanitizeForPath(t));

        QString folder = "view/icons/" + sanitizedArtist;
        QString oldFile = oldSanitizedTitle + "." + ext;
        QString newFile = newSanitizedTitle + "." + ext;

        QString oldPath = folder + "/" + oldFile;
        QString newPath = folder + "/" + newFile;

        // Controllo conflitto
        if (QFile::exists(newPath)) {
            throw std::runtime_error("Impossibile rinominare: esiste già un prodotto con nome \"" + t + "\".");
        }

        // Rinomina effettiva se file esiste
        if (QFile::exists(oldPath)) {
            if (!QFile::rename(oldPath, newPath)) {
                throw std::runtime_error("Errore durante la rinomina del file immagine da \"" +
                    oldFile.toStdString() + "\" a \"" + newFile.toStdString() + "\".");
            }
        }

        imagePath = newPath.toStdString();  // usa il path reale, non più `:/icons/...`

        QFile img(newPath);
        if (img.open(QIODevice::ReadOnly)) {
            QByteArray data = img.readAll();
            img.close();
            imageB64 = data.toBase64().toStdString();
        } else {
            imageB64 = "";
        }
    }

    title = t;
}

std::string ArtistProduct::getDescription() const { 
    return description; 
}
void ArtistProduct::setDescription(const std::string &d) {
    description = d;
}

std::string ArtistProduct::getImagePath() const {
    return imagePath;
}

std::string ArtistProduct::getImageB64() const {
    return imageB64;
}

void ArtistProduct::setImageB64(const std::string& b64) {
    imageB64 = b64;
}

void ArtistProduct::setImagePath(const std::string& ip) {
    if(ip == imagePath) {
        return;
    }

    if(ip.empty()) {
        imagePath = "";
        imageB64 = "";
        return;
    }
    
    QFile file(QString::fromStdString(ip));
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        throw std::runtime_error("Il file immagine specificato non esiste o non è leggibile.");
    }

    QByteArray data = file.readAll();
    file.close();

    if (data.isEmpty()) {
        throw std::runtime_error("Il file immagine è vuoto o corrotto.");
    }

    QString ext = QFileInfo(QString::fromStdString(ip)).suffix().toLower();
    if (ext.isEmpty()) ext = "png"; // fallback se l'estensione non è rilevabile

    QString sanitizedArtist = QString::fromStdString(DataManager::sanitizeForPath(nomeArtista));
    QString sanitizedTitle  = QString::fromStdString(DataManager::sanitizeForPath(title));

    QString folder = "view/icons/" + sanitizedArtist;
    QString fileName = sanitizedTitle + "." + ext;
    QString localPath = folder + "/" + fileName;

    QDir().mkpath(folder);

    // Rimuove eventuali versioni precedenti con estensione diversa
    QDir dir(folder);
    QStringList existing = dir.entryList(QStringList() << sanitizedTitle + ".*", QDir::Files);
    for (const QString& e : existing) {
        dir.remove(e);
    }

    QFile out(localPath);
    if (!out.open(QIODevice::WriteOnly) || out.write(data) != data.size()) {
        throw std::runtime_error("Errore nella scrittura del nuovo file immagine.");
    }
    out.close();

    // Aggiorna campi interni
    imageB64 = data.toBase64().toStdString();
    imagePath = localPath.toStdString();
}

void ArtistProduct::printInfo() const {

    std::cout << "--ARTIST PRODUCT--"
            <<"ID: " << ID << "\n"
            << "Artista: " << nomeArtista << "\n"
            << "Titolo: " << title << "\n"
            << "Descrizione: " << description << "\n"
            << "Path immagine: " << imagePath << "\n" << std::endl;
}


static std::string restoreProductImageFromB64(const std::string& artistName, const std::string& productTitle, const std::string& imageB64, const std::string& originalExt) {
    if (imageB64.empty()) return "";

    // Sanifica nomi per i path
    std::string sanitizedArtist = DataManager::sanitizeForPath(artistName);
    std::string sanitizedTitle = DataManager::sanitizeForPath(productTitle);
    
    QString folder = "view/icons/" + QString::fromStdString(sanitizedArtist);
    QString ext = QString::fromStdString(originalExt).toLower();
    if (ext.isEmpty()) ext = "png"; // fallback

    QString fileName = QString::fromStdString(sanitizedTitle) + "." + ext;
    QString localPath = folder + "/" + fileName;

    // Crea directory se mancante
    QDir().mkpath(folder);

    // Rimuove versioni precedenti dello stesso prodotto (qualsiasi estensione)
    QDir dir(folder);
    QStringList existing = dir.entryList(QStringList() << QString::fromStdString(sanitizedTitle) + ".*", QDir::Files);
    for (const QString& f : existing) {
        dir.remove(f);
    }

    // Scrive il file decodificato
    QFile out(localPath);
    if (out.open(QIODevice::WriteOnly)) {
        out.write(QByteArray::fromBase64(QString::fromStdString(imageB64).toUtf8()));
        out.close();
        return localPath.toStdString();
    }

    ErrorManager::logError("Impossibile scrivere immagine base64 del prodotto '" + productTitle + "' per artista '" + artistName + "'");
    return "";
}

//JSON
// Converte un oggetto JSON in un oggetto ArtistProduct
ArtistProduct::ArtistProduct(Artista* owner, const QJsonObject& json)
    : ID(generateId()),
      title(json["title"].toString().toStdString()),
      description(json["description"].toString().toStdString()),
      imagePath("")
{
    if (!owner) {
        assert(false && "ArtistProduct JSON constructor received nullptr owner");
        throw std::invalid_argument("ArtistProduct JSON constructor received nullptr owner.");
    }

    setOwner(owner); // imposta id_artist e nomeArtista

    imageB64 = json["imageB64"].toString().toStdString();

    if (!imageB64.empty()) {
        QString ext = QFileInfo(QString::fromStdString(json["imagePath"].toString().toStdString())).suffix();
        imagePath = restoreProductImageFromB64(nomeArtista, title, imageB64, ext.toStdString());
    }
}

// Converte l'oggetto ArtistProduct in un oggetto JSON
QJsonObject ArtistProduct::toJson() const {
    QJsonObject json;
    json["title"] = QString::fromStdString(title);
    json["description"] = QString::fromStdString(description);
    json["imagePath"] = QString::fromStdString(imagePath);
    json["imageB64"] = QString::fromStdString(imageB64);
    return json;
}

// Converte un oggetto JSON nella giusta sottoclasse concreta di ArtistProduct e restituisce un puntatore a quell'oggetto
ArtistProduct* ArtistProduct::createJson(Artista* owner, const QJsonObject& json) {
    if (!json.contains("type") || !json["type"].isString()) {
        assert(false && "ArtistProduct::createJson() ricevuto JSON senza campo 'type'");
        throw std::invalid_argument("ArtistProduct::createJson() JSON malformato: campo 'type' mancante o non valido.");
    }

    std::string type = json["type"].toString().toStdString();

    if (type == "album")   return new Album(owner, json);
    if (type == "singolo") return new Singolo(owner, json);
    if (type == "tour")    return new Tour(owner, json);
    if (type == "tshirt")  return new TShirt(owner, json);
    if (type == "cd")      return new CD(owner, json);
    if (type == "vinile")  return new Vinile(owner, json);

    throw std::runtime_error("ArtistProduct::createJson() tipo prodotto sconosciuto: \"" + type + "\"");
}

//XML
// Converte un oggetto XML in un oggetto ArtistProduct
ArtistProduct::ArtistProduct(Artista* owner, const QDomElement& xml)
    : ID(generateId()), imagePath(""), imageB64("")
{
    if (!owner) {
        assert(false && "ArtistProduct XML constructor received nullptr owner");
        throw std::invalid_argument("ArtistProduct XML constructor received nullptr owner.");
    }

    setOwner(owner); // inizializza id_artist e nomeArtista

    // Cerca sottoelemento <ArtistProduct>
    QDomElement apElem = xml.tagName() == "ArtistProduct" ? xml : xml.firstChildElement("ArtistProduct");
    if (apElem.isNull()) {
        throw std::invalid_argument("ArtistProduct XML constructor: tag <ArtistProduct> mancante o malformato.");
    }

    title = apElem.attribute("title").toStdString();
    description = apElem.attribute("description").toStdString();
    imagePath = apElem.attribute("imagePath").toStdString();

    QDomElement imgEl = apElem.firstChildElement("imageB64");
    imageB64 = imgEl.isNull() ? "" : imgEl.text().toStdString();

    if (!imageB64.empty()) {
        QString ext = QFileInfo(QString::fromStdString(imagePath)).suffix();
        imagePath = restoreProductImageFromB64(nomeArtista, title, imageB64, ext.toStdString());
    }
}

// Converte l'oggetto ArtistProduct in un oggetto XML
QDomElement ArtistProduct::toXml(QDomDocument& doc) const {
    QDomElement el = doc.createElement("ArtistProduct");
    el.setAttribute("title", QString::fromStdString(title));
    el.setAttribute("description", QString::fromStdString(description));
    el.setAttribute("imagePath", QString::fromStdString(imagePath));

    if (!imageB64.empty()) {
        QDomElement imgEl = doc.createElement("imageB64");
        imgEl.appendChild(doc.createTextNode(QString::fromStdString(imageB64)));
        el.appendChild(imgEl);
    }

    return el;
}

// Converte un oggetto XML nella giusta sottoclasse concreta di ArtistProduct e restituisce un puntatore a quell'oggetto
ArtistProduct* ArtistProduct::createXml(Artista* owner, const QDomElement& el) {
    if (el.isNull() || el.tagName().isEmpty()) {
        throw std::invalid_argument("ArtistProduct::createXml() ha ricevuto un elemento XML nullo o senza tag.");
    }

    QString tag = el.tagName();

    if (tag == "Album")   return new Album(owner, el);
    if (tag == "Singolo") return new Singolo(owner, el);
    if (tag == "CD")      return new CD(owner, el);
    if (tag == "Vinile")  return new Vinile(owner, el);
    if (tag == "TShirt")  return new TShirt(owner, el);
    if (tag == "Tour")    return new Tour(owner, el);

    throw std::runtime_error("ArtistProduct::createXml() - tag prodotto XML sconosciuto: \"" + tag.toStdString() + "\"");
}


// OVERLOADING OPERATORI
bool operator==(const ArtistProduct& a, const ArtistProduct& b) {
    if (a.title != b.title) {
        std::cerr << "[DIFF] Title diverso: \"" << a.title << "\" vs \"" << b.title << "\"" << std::endl;
        return false;
    }
    if (a.description != b.description) {
        std::cerr << "[DIFF] Description diversa: \"" << a.description << "\" vs \"" << b.description << "\"" << std::endl;
        return false;
    }
    if (a.imagePath != b.imagePath) {
        std::cerr << "[DIFF] ImagePath diverso: \"" << a.imagePath << "\" vs \"" << b.imagePath << "\"" << std::endl;
        return false;
    }
    if (a.imageB64 != b.imageB64) {
        std::cerr << "[DIFF] ImageB64 diversa (contenuto immagine differente)" << std::endl;
        return false;
    }
    return true;
}

bool operator!=(const ArtistProduct& a, const ArtistProduct& b) {
    return !(a == b);
}
