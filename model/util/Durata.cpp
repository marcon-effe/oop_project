#include "Durata.h"

Durata::Durata(unsigned int h, unsigned int m, unsigned int s)
: ore(h), minuti(m), secondi(s){ normalize(); }

Durata::Durata(unsigned int s)
: ore(0), minuti(0), secondi(s) {
    normalize();
}

Durata::~Durata() {}

unsigned int Durata::getOre() const { return ore; }
unsigned int Durata::getMinuti() const { return minuti; }
unsigned int Durata::getSecondi() const { return secondi; }

void Durata::setOre(unsigned int h) {
    ore = h;
}

void Durata::setMinuti(unsigned int m) {
    minuti = m;
    normalize(); 
}

void Durata::setSecondi(unsigned int s) {
    secondi = s;
    normalize(); 
}

void Durata::normalize() {
    if(secondi >= 60) {
        minuti += secondi / 60;
        secondi = secondi % 60;
    }
    if(minuti >= 60) {
        ore += minuti / 60;
        minuti = minuti % 60;
    }
}

std::string Durata::toString() const {
    std::string oreStr = std::to_string(ore);
    std::string minutiStr = std::to_string(minuti);
    std::string secondiStr = std::to_string(secondi);
    
    if (ore < 10)
        oreStr = "0" + oreStr;
    if (minuti < 10)
        minutiStr = "0" + minutiStr;
    if (secondi < 10)
        secondiStr = "0" + secondiStr;
    
    return oreStr + ":" + minutiStr + ":" + secondiStr;
}


//JSON
// Converte un oggetto JSON in un oggetto Durata
Durata::Durata(const QJsonObject& json) {
    ore = json["ore"].toInt();
    minuti = json["minuti"].toInt();
    secondi = json["secondi"].toInt();
    normalize();
}

// Converte l'oggetto Durata in un oggetto JSON
QJsonObject Durata::toJson() const{
    QJsonObject json;
    json["ore"] = static_cast<int>(ore);
    json["minuti"] = static_cast<int>(minuti);
    json["secondi"] = static_cast<int>(secondi);
    return json;
}


//XML
// Converte un oggetto XML in un oggetto Durata
Durata::Durata(const QDomElement& xml) {
    ore = xml.attribute("ore").toInt();
    minuti = xml.attribute("minuti").toInt();
    secondi = xml.attribute("secondi").toInt();
    normalize();
}

// Converte l'oggetto Durata in un oggetto XML
QDomElement Durata::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Durata");
    xml.setAttribute("ore", static_cast<int>(ore));
    xml.setAttribute("minuti", static_cast<int>(minuti));
    xml.setAttribute("secondi", static_cast<int>(secondi));
    return xml;
}


//OVERLOADING OPERATORI
bool operator==(const Durata& lhs, const Durata& rhs) {
    return lhs.getOre() == rhs.getOre() &&
           lhs.getMinuti() == rhs.getMinuti() &&
           lhs.getSecondi() == rhs.getSecondi();
}

bool operator!=(const Durata& lhs, const Durata& rhs) {
    return !(lhs == rhs);
}

bool operator<(const Durata& lhs, const Durata& rhs) {
    return std::make_tuple(lhs.getOre(), lhs.getMinuti(), lhs.getSecondi()) <
           std::make_tuple(rhs.getOre(), rhs.getMinuti(), rhs.getSecondi());
}

bool operator>(const Durata& lhs, const Durata& rhs) {
    return std::make_tuple(lhs.getOre(), lhs.getMinuti(), lhs.getSecondi()) >
           std::make_tuple(rhs.getOre(), rhs.getMinuti(), rhs.getSecondi());
}
