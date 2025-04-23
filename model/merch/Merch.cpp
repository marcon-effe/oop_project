#include <iostream>
#include "Merch.h"

#include"../../view/VisitorGUI.h"

Merch::Merch(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice)
: NotMusica(t, desc, prezzo, disponibile, quantita), codiceProdotto(codice) {}

Merch::Merch(NotMusica* nm, std::string cp)
: NotMusica(nm), codiceProdotto(cp) {}

Merch::Merch(const Merch* m)
: NotMusica(m), codiceProdotto(m->getCodiceProdotto()) {}

Merch::~Merch() {}

std::string Merch::getCodiceProdotto() const {
    return codiceProdotto;
}

void Merch::setCodiceProdotto(const std::string &codice) {
    codiceProdotto = codice;
}

void Merch::printInfo() const {
    std::cout << "Merch prodotto: " << title << "\nDescrizione: " << description
              << "\nPrezzo: " << prezzo << "\nDisponibile: " << (disponibile ? "Si" : "No")
              << "\nQuantita': " << quantita
              << "\nCodice prodotto: " << codiceProdotto << std::endl;
}


//JSON
// Converte un oggetto JSON in un oggetto Merch
Merch::Merch(const QJsonObject& json)
: NotMusica(json["not_musica"].toObject()),
    codiceProdotto(json["codiceProdotto"].toString().toStdString()){}
// Converte l'oggetto Merch in un oggetto JSON
QJsonObject Merch::toJson() const{
    QJsonObject json;
    json["not_musica"] = NotMusica::toJson();
    json["codiceProdotto"] = QString::fromStdString(codiceProdotto);
    return json;
}

//XML
// Converte un oggetto XML in un oggetto Merch
Merch::Merch(const QDomElement& xml)
: NotMusica(xml.firstChildElement("NotMusica")),
    codiceProdotto(xml.attribute("codiceProdotto").toStdString()) {}
// Converte l'oggetto Merch in un oggetto XML
QDomElement Merch::toXml(QDomDocument& doc) const{
    QDomElement xml = doc.createElement("Merch");
    xml.setAttribute("codiceProdotto", QString::fromStdString(codiceProdotto));

    xml.appendChild(NotMusica::toXml(doc));

    return xml;
}

// OVERLOADING OPERATORI
bool operator==(const Merch& a, const Merch& b) {
    if (!(static_cast<const NotMusica&>(a) == static_cast<const NotMusica&>(b))) return false;
    if (a.prezzo != b.prezzo) return false;
    if (a.disponibile != b.disponibile) return false;
    if (a.quantita != b.quantita) return false;
    if (a.codiceProdotto != b.codiceProdotto) return false;
    return true;
}

bool operator!=(const Merch& a, const Merch& b) {
    return !(a == b);
}