#include <iostream>
#include "NotMusica.h"

#include"../../view/VisitorGUI.h"

NotMusica::NotMusica(const std::string &t, const std::string &desc, double p, bool d, unsigned int q)
: ArtistProduct(t, desc), prezzo(p), disponibile(d), quantita(q) {}

NotMusica::NotMusica(ArtistProduct* p, double pr, bool disp, unsigned int q)
: ArtistProduct(p), prezzo(pr), disponibile(disp), quantita(q) {}

NotMusica::NotMusica(const NotMusica* nm)
: ArtistProduct(nm), prezzo(nm->getPrezzo()), disponibile(nm->getDisponibile()), quantita(nm->getQuantita()) {}

NotMusica::~NotMusica() {}

double NotMusica::getPrezzo() const {
    return prezzo;
}
void NotMusica::setPrezzo(double p) {
    prezzo = p;
}

bool NotMusica::getDisponibile() const {
    return disponibile;
}
void NotMusica::setDisponibile(bool d) {
    disponibile = d;
}

unsigned int NotMusica::getQuantita() const {
    return quantita;
}
void NotMusica::setQuantita(unsigned int q) {
    quantita = q;
}

void NotMusica::printInfo() const {
    ArtistProduct::printInfo();
    std::cout << "Prezzo: " << prezzo << std::endl;
    std::cout << "Disponibile: " << (disponibile ? "Si" : "No") << std::endl;
    std::cout << "Quantita: " << quantita << std::endl;
}


//JSON
// Converte un oggetto JSON in un oggetto NotMusica
NotMusica::NotMusica(const QJsonObject& json)
: ArtistProduct(json["ArtistProduct"].toObject()),
    prezzo(json["prezzo"].toInt()),
    disponibile(json["disponibile"].toBool()),
    quantita(json["quantita"].toInt()){}
//Converte l'oggetto NotMusica in un oggetto JSON
QJsonObject NotMusica::toJson() const{
    QJsonObject json;
    json["ArtistProduct"] = ArtistProduct::toJson();
    json["prezzo"] = prezzo;
    json["disponibile"] = disponibile;
    json["quantita"] = static_cast<int>(quantita);
    return json;
}

//XML
// Converte un oggetto XML in un oggetto NotMusica
NotMusica::NotMusica(const QDomElement& xml)
: ArtistProduct(xml.firstChildElement("ArtistProduct")),
    prezzo(xml.attribute("prezzo").toDouble()),
    disponibile(xml.attribute("disponibile").toInt()),
    quantita(xml.attribute("quantita").toInt()) {}
//Converte l'oggetto NotMusica in un oggetto XML   
QDomElement NotMusica::toXml(QDomDocument& doc) const {
    QDomElement notMusicaEl = doc.createElement("NotMusica");
    notMusicaEl.setAttribute("prezzo", QString::number(prezzo, 'f', 2));
    notMusicaEl.setAttribute("disponibile", disponibile ? 1 : 0);
    notMusicaEl.setAttribute("quantita", static_cast<int>(quantita));

    QDomElement artistProductEl = ArtistProduct::toXml(doc);
    notMusicaEl.appendChild(artistProductEl);

    return notMusicaEl;
}

// OVERLOADING OPERATORI
bool operator==(const NotMusica& a, const NotMusica& b) {
    if (!(static_cast<const ArtistProduct&>(a) == static_cast<const ArtistProduct&>(b))) return false;
    return true;
}

bool operator!=(const NotMusica& a, const NotMusica& b) {
    return !(a == b);
}