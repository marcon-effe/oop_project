#include <iostream>
#include "Orario.h"

Orario::Orario(unsigned int o, unsigned int m, unsigned int s)
: ore(0), minuti(0), secondi(0){
    // Uso dei setter per i controlli in costruzione.
    setOre(o);
    setMinuti(m);
    setSecondi(s);
}

Orario::~Orario() {}

unsigned int Orario::getOre() const { 
    return ore; 
}
void Orario::setOre(unsigned int o) { 
    if(o >= 24) {
        // TOOO -> valutare uso di cout 
        std::cerr << "Errore: valore per le ore (" << o 
                  << ") non valido. Deve essere compreso tra 0 e 23." << std::endl;
        return; 
    }
    ore = o;
}

unsigned int Orario::getMinuti() const { 
    return minuti; 
}
void Orario::setMinuti(unsigned int m) { 
    if(m >= 60) {
        std::cerr << "Errore: valore per i minuti (" << m 
                  << ") non valido. Deve essere compreso tra 0 e 59." << std::endl;
        return; 
    }
    minuti = m;
}

unsigned int Orario::getSecondi() const { 
    return secondi; 
}
void Orario::setSecondi(unsigned int s) { 
    if(s >= 60) {
        std::cerr << "Errore: valore per i secondi (" << s 
                  << ") non valido. Deve essere compreso tra 0 e 59." << std::endl;
        return;
    }
    secondi = s;
}

Orario Orario::getOrario() const {
    return *this;
}

std::string Orario::toString() const {
    std::string str;
    str += std::to_string(ore) + ":";
    str += std::to_string(minuti) + ":";
    str += std::to_string(secondi);
    return str;
}


//JSON
// Converte un oggetto JSON in un oggetto Orario
Orario::Orario(const QJsonObject& json) 
: ore(0), minuti(0), secondi(0){
    setOre(json["ore"].toInt());
    setMinuti(json["minuti"].toInt());
    setSecondi(json["secondi"].toInt());
}
// Converte l'oggetto Orario in un oggetto JSON
QJsonObject Orario::toJson() const{
    QJsonObject json;
    json["ore"] = static_cast<int>(ore);
    json["minuti"] = static_cast<int>(minuti);
    json["secondi"] = static_cast<int>(secondi);
    return json;
}

//XML
// Converte un oggetto XML in un oggetto Orario
Orario::Orario(const QDomElement& xml)
: ore(0), minuti(0), secondi(0){
    setOre(xml.attribute("ore").toInt());
    setMinuti(xml.attribute("minuti").toInt());
    setSecondi(xml.attribute("secondi").toInt());
}
// Converte l'oggetto Orario in un oggetto XML
QDomElement Orario::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Orario");
    xml.setAttribute("ore", static_cast<int>(ore));
    xml.setAttribute("minuti", static_cast<int>(minuti));
    xml.setAttribute("secondi", static_cast<int>(secondi));
    return xml;
}


//OVERLOADING OPERATORI
bool operator==(const Orario& lhs, const Orario& rhs) {
    return lhs.getOre() == rhs.getOre() &&
           lhs.getMinuti() == rhs.getMinuti() &&
           lhs.getSecondi() == rhs.getSecondi();
}

bool operator!=(const Orario& lhs, const Orario& rhs) {
    return !(lhs == rhs);
}

bool operator<(const Orario& lhs, const Orario& rhs) {
    return std::make_tuple(lhs.getOre(), lhs.getMinuti(), lhs.getSecondi()) <
           std::make_tuple(rhs.getOre(), rhs.getMinuti(), rhs.getSecondi());
}

bool operator>(const Orario& lhs, const Orario& rhs) {
    return std::make_tuple(lhs.getOre(), lhs.getMinuti(), lhs.getSecondi()) >
           std::make_tuple(rhs.getOre(), rhs.getMinuti(), rhs.getSecondi());
}