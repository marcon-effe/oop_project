#include <iostream>
#include "TShirt.h"

#include"../../view/VisitorGUI.h"

TShirt::TShirt(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice, const std::string &taglia, const std::string &colore)
: Merch(t, desc, prezzo, disponibile, quantita, codice), taglia(taglia), colore(colore) {}

TShirt::TShirt(Merch* m, std::string t, std::string c)
: Merch(m), taglia(t), colore(c) {}

TShirt::TShirt(const TShirt* t)
: Merch(t), taglia(t->getTaglia()), colore(t->getColore()) {}

TShirt::~TShirt() {}

std::string TShirt::getTaglia() const {
    return taglia;
}

void TShirt::setTaglia(const std::string &t) {
    taglia = t;
}

std::string TShirt::getColore() const {
    return colore;
}

void TShirt::setColore(const std::string &c) {
    colore = c;
}

void TShirt::printInfo() const {
    std::cout << "T-Shirt: " << title << "\nDescrizione: " << description
              << "\nPrezzo: " << prezzo << "\nDisponibile: " << (disponibile ? "Si" : "No")
              << "\nQuantita': " << quantita
              << "\nCodice prodotto: " << codiceProdotto
              << "\nTaglia: " << taglia
              << "\nColore: " << colore << std::endl;
}


//JSON
// Converte un oggetto JSON in un oggetto TShirt
TShirt::TShirt(const QJsonObject& json)
: Merch(json["merch"].toObject()),
    taglia(json["taglia"].toString().toStdString()),
    colore(json["colore"].toString().toStdString()){}
// Converte l'oggetto TShirt in un oggetto JSON
QJsonObject TShirt::toJson() const{
    QJsonObject json;
    json["type"] = "tshirt";
    json["merch"] = Merch::toJson();
    json["taglia"] = QString::fromStdString(taglia);
    json["colore"] = QString::fromStdString(colore);
    return json;
}

//XML
// Converte un oggetto XML in un oggetto TShirt
TShirt::TShirt(const QDomElement& xml)
: Merch(xml.firstChildElement("Merch")),
    taglia(xml.attribute("taglia").toStdString()),
    colore(xml.attribute("colore").toStdString()) {}
// Converte l'oggetto TShirt in un oggetto XML
QDomElement TShirt::toXml(QDomDocument& doc) const{
    QDomElement xml = doc.createElement("TShirt");
    xml.setAttribute("taglia", QString::fromStdString(taglia));
    xml.setAttribute("colore", QString::fromStdString(colore));
    
    xml.appendChild(Merch::toXml(doc));
    return xml;
}


void TShirt::accept(VisitorGUI* v) const {
    v->visit(this);
}

// OVERLOADING OPERATORI
bool operator==(const TShirt& a, const TShirt& b) {
    if (!(static_cast<const Merch&>(a) == static_cast<const Merch&>(b))) return false;
    if (a.taglia != b.taglia) return false;
    if (a.colore != b.colore) return false;
    return true;
}

bool operator!=(const TShirt& a, const TShirt& b) {
    return !(a == b);
}
