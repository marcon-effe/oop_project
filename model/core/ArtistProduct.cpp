#include <iostream>

#include "ArtistProduct.h"
#include "../artisti/Artista.h"
#include "../musica/Album.h"
#include "../musica/Singolo.h"
#include "../tour/Tour.h"
#include "../merch/TShirt.h"
#include "../merch/CD.h"
#include "../merch/Vinile.h"

unsigned int ArtistProduct::nextProductId = 0;
std::mutex ArtistProduct::idMutex;

ArtistProduct::ArtistProduct(Artista* owner, const std::string& t, const std::string& desc)
: ID(generateId()), title(t), description(desc), imagePath("") {
    if (!owner) {
        assert(false && "Artista pointer must not be nullptr");
        throw std::invalid_argument("ArtistProduct constructor received nullptr Artista pointer.");
    }
    id_artist = owner->getId();
}

ArtistProduct::ArtistProduct(Artista* owner, const std::string& t, const std::string& d, const std::string& i)
: ID(generateId()), title(t), description(d), imagePath(i) {
    if (!owner) {
        assert(false && "Artista pointer must not be nullptr");
        throw std::invalid_argument("ArtistProduct constructor received nullptr Artista pointer.");
    }
    id_artist = owner->getId();
}

ArtistProduct::ArtistProduct(const ArtistProduct* p)
: ID(generateId()), title(p->getTitle()), description(p->getDescription()), imagePath(p->getImagePath()) {
    if (!p) {
        assert(false && "ArtistProduct pointer must not be nullptr");
        throw std::invalid_argument("ArtistProduct constructor received nullptr ArtistProduct pointer.");
    }
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

void ArtistProduct::setArtistId(Artista* owner) {
    if (!owner) {
        assert(false && "Artista pointer must not be nullptr");
        throw std::invalid_argument("ArtistProduct setArtistId() received nullptr Artista pointer.");
    }
    id_artist = owner->getId();
}


std::string ArtistProduct::getTitle() const { 
    return title; 
}
void ArtistProduct::setTitle(const std::string &t) {
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

void ArtistProduct::setImagePath(const std::string &path) {
    imagePath = path;
}

void ArtistProduct::printInfo() const{
    std::cout << "--ARTIST PRODUCT--" << std::endl;
    std::cout << "ArtistProduct ID: " << getId() << std::endl;
    std::cout << "Artist ID: " << getArtistId() << std::endl;
    std::cout << "Title: " << getTitle() << std::endl;
    std::cout << "Description: " << getDescription() << std::endl;
    std::cout << "Image Path: " << getImagePath() << std::endl;
}


//JSON
// Converte un oggetto JSON in un oggetto ArtistProduct
ArtistProduct::ArtistProduct(Artista* owner, const QJsonObject& json)
: ID(generateId()),
    title(json["title"].toString().toStdString()),
    description(json["description"].toString().toStdString()),
    imagePath(json["imagePath"].toString().toStdString())
{
    // So di avere il setter ma preferisco riscreverlo per sapere esattamente dove fallisce in caso di eccezione
    if (!owner) {
        assert(false && "ArtistProduct JSON constructor received nullptr owner");
        throw std::invalid_argument("ArtistProduct JSON constructor received nullptr owner.");
    }
    id_artist = owner->getId(); 
}

// Converte l'oggetto ArtistProduct in un oggetto JSON
QJsonObject ArtistProduct::toJson() const{
    QJsonObject json;
    json["title"] = QString::fromStdString(title);
    json["description"] = QString::fromStdString(description);
    json["imagePath"] = QString::fromStdString(imagePath);
    return json;
}

// Converte un oggetto JSON nella giusta sottoclasse concreta di ArtistProduct e restituisce un puntatore a quell'oggetto
ArtistProduct* ArtistProduct::createJson(Artista* owner, const QJsonObject& json) {
    std::string type = json["type"].toString().toStdString();
    if (type.empty()) {
        assert(false && "ArtistProduct::createJson() ricevuto JSON senza campo 'type'");
        throw std::invalid_argument("ArtistProduct::createJson() JSON malformato: campo 'type' mancante.");
    }

    if (type == "album") return new Album(owner, json);
    if (type == "singolo") return new Singolo(owner, json);
    if (type == "tour") return new Tour(owner, json);
    if (type == "tshirt") return new TShirt(owner, json);
    if (type == "cd") return new CD(owner, json);
    if (type == "vinile") return new Vinile(owner, json);

    throw std::runtime_error("Tipo prodotto sconosciuto durante il parsing JSON: " + type);
}

//XML
// Converte un oggetto XML in un oggetto ArtistProduct
ArtistProduct::ArtistProduct(Artista* owner, const QDomElement& el)
:   ID(generateId()),
    title(el.attribute("title").toStdString()),
    description(el.attribute("description").toStdString()),
    imagePath(el.attribute("imagePath").toStdString())
{
    // So di avere il setter ma preferisco riscreverlo per sapere esattamente dove fallisce in caso di eccezione
    if (!owner) {
        assert(false && "ArtistProduct XML constructor received nullptr owner");
        throw std::invalid_argument("ArtistProduct XML constructor received nullptr owner.");
    }
    id_artist = owner->getId();
}

// Converte l'oggetto ArtistProduct in un oggetto XML
QDomElement ArtistProduct::toXml(QDomDocument& doc) const {
    QDomElement el = doc.createElement("ArtistProduct");
    el.setAttribute("title", QString::fromStdString(title));
    el.setAttribute("description", QString::fromStdString(description));
    el.setAttribute("imagePath", QString::fromStdString(imagePath));
    return el;
}

// Converte un oggetto XML nella giusta sottoclasse concreta di ArtistProduct e restituisce un puntatore a quell'oggetto
ArtistProduct* ArtistProduct::createXml(Artista* owner, const QDomElement& el) {
    QString tag = el.tagName();

    if (tag == "Album")   return new Album(owner, el);
    if (tag == "Singolo") return new Singolo(owner, el);
    if (tag == "CD")      return new CD(owner, el);
    if (tag == "Vinile")  return new Vinile(owner, el);
    if (tag == "TShirt")  return new TShirt(owner, el);
    if (tag == "Tour")    return new Tour(owner, el);

    throw std::runtime_error("Tag prodotto XML sconosciuto: " + tag.toStdString());
}


// OVERLOADING OPERATORI
bool operator==(const ArtistProduct& a, const ArtistProduct& b) {
    if (a.title != b.title) {
        std::cerr << "❌ Title diverso: " << a.title << " vs " << b.title << std::endl;
        return false;
    }
    if (a.description != b.description) {
        std::cerr << "❌ Description diversa: " << a.description << " vs " << b.description << std::endl;
        return false;
    }
    if (a.imagePath != b.imagePath) {
        std::cerr << "❌ ImagePath diverso: " << a.imagePath << " vs " << b.imagePath << std::endl;
        return false;
    }
    return true;
}

bool operator!=(const ArtistProduct& a, const ArtistProduct& b) {
    return !(a == b);
}
