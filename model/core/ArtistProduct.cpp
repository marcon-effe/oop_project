#include <iostream>

#include "ArtistProduct.h"
#include "../musica/Album.h"
#include "../musica/Singolo.h"
#include "../tour/Tour.h"
#include "../merch/TShirt.h"
#include "../merch/CD.h"
#include "../merch/Vinile.h"

ArtistProduct::ArtistProduct(const std::string &t, const std::string &desc)
: title(t), description(desc), imagePath("") {}

ArtistProduct::ArtistProduct(const std::string& t, const std::string& d, const std::string& i)
: title(t), description(d), imagePath(i) {}

ArtistProduct::ArtistProduct(const ArtistProduct* p)
: title(p->getTitle()), description(p->getDescription()), imagePath(p->getImagePath()) {}

ArtistProduct::~ArtistProduct() {}

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
    std::cout << "Title: " << title << std::endl;
    std::cout << "Description: " << description << std::endl;
    std::cout << "Image Path: " << imagePath << std::endl;
}


//JSON
// Converte un oggetto JSON in un oggetto ArtistProduct
ArtistProduct::ArtistProduct(const QJsonObject& json)
: title(json["title"].toString().toStdString()), description(json["description"].toString().toStdString()), imagePath(json["imagePath"].toString().toStdString()) {}
// Converte l'oggetto ArtistProduct in un oggetto JSON
QJsonObject ArtistProduct::toJson() const{
    QJsonObject json;
    json["title"] = QString::fromStdString(title);
    json["description"] = QString::fromStdString(description);
    json["imagePath"] = QString::fromStdString(imagePath);
    return json;
}
// Converte un oggetto JSON nella giusta sottoclasse concreta di ArtistProduct e restituisce un puntatore a quell'oggetto
ArtistProduct* ArtistProduct::createJson(const QJsonObject& json) {
    std::string type = json["type"].toString().toStdString();

    if (type == "album") return new Album(json);
    if (type == "singolo") return new Singolo(json);
    if (type == "tour") return new Tour(json);
    if (type == "tshirt") return new TShirt(json);
    if (type == "cd") return new CD(json);
    if (type == "vinile") return new Vinile(json);

    qWarning() << "Tipo prodotto sconosciuto: " << QString::fromStdString(type);
    return nullptr;
}

//XML
// Converte un oggetto XML in un oggetto ArtistProduct
ArtistProduct::ArtistProduct(const QDomElement& xml) {
    title = xml.attribute("title").toStdString();
    description = xml.attribute("description").toStdString();
    imagePath = xml.attribute("imagePath").toStdString();
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
ArtistProduct* ArtistProduct::createXml(const QDomElement& el) {
    QString tag = el.tagName();

    if (tag == "Album")   return new Album(el);
    if (tag == "Singolo") return new Singolo(el);
    if (tag == "CD")      return new CD(el);
    if (tag == "Vinile")  return new Vinile(el);
    if (tag == "TShirt")  return new TShirt(el);
    if (tag == "Tour")    return new Tour(el);

    return nullptr;
}


// OVERLOADING OPERATORI
bool operator==(const ArtistProduct& a, const ArtistProduct& b) {
    if (a.title != b.title) return false;
    if (a.description != b.description) return false;
    if (a.imagePath != b.imagePath) return false;
    return true;
}

bool operator!=(const ArtistProduct& a, const ArtistProduct& b) {
    return !(a == b);
}
