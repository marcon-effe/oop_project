#include <iostream>
#include "Data.h"

// Costruttore: usa i setter così da applicare i controlli
Data::Data(unsigned int g, unsigned int m, unsigned int a) {
    setMese(m);
    setAnno(a);
    setGiorno(g);
}

Data::~Data() {}

unsigned int Data::getGiorno() const { return giorno; }
void Data::setGiorno(unsigned int g) {
    // Trova il max giorni per il mese corrente
    unsigned int maxGiorni = 31;
    switch (mese) {
        case 4: case 6: case 9: case 11:
            maxGiorni = 30;
            break;
        case 2:     // Possibilità di aggiungere il controllo sull'anno bisestile
            maxGiorni = 28;
            break;
    }
    if (g < 1) g = 1;
    if (g > maxGiorni) g = maxGiorni;
    giorno = g;
}

unsigned int Data::getMese() const { return mese; }
void Data::setMese(unsigned int m) {
    // Esempio: valori ammessi tra 1 e 12
    if (m < 1) 
        mese = 1;
    else if (m > 12) 
        mese = 12;
    else 
        mese = m;
}

unsigned int Data::getAnno() const { return anno; }
void Data::setAnno(unsigned int a) {
    // Esempio: anno minimo 1, senza limite superiore
    if (a < 1) 
        anno = 1;
    else 
        anno = a;
}

std::string Data::toString() const {
    std::string dayStr = std::to_string(giorno);
    // Aggiunta dello 0 -> per estetica
    if (giorno < 10) {
        dayStr = "0" + dayStr;
    }

    std::string monthStr = std::to_string(mese);
    if (mese < 10) {
        monthStr = "0" + monthStr;
    }

    std::string yearStr = std::to_string(anno);

    return dayStr + "/" + monthStr + "/" + yearStr;
}

// JSON
// Converte un oggetto JSON in un oggetto Data
Data::Data(const QJsonObject& json) {
    setMese(json["mese"].toInt());
    setAnno(json["anno"].toInt());
    setGiorno(json["giorno"].toInt());
}
// Converte l'oggetto Data in un oggetto JSON
QJsonObject Data::toJson() const{
    QJsonObject json;
    json["giorno"] = static_cast<int>(giorno);
    json["mese"] = static_cast<int>(mese);
    json["anno"] = static_cast<int>(anno);
    return json;
}

//XML
// Converte un oggetto XML in un oggetto Data
Data::Data(const QDomElement& xml) {
    setMese(xml.attribute("mese").toInt());
    setAnno(xml.attribute("anno").toInt());
    setGiorno(xml.attribute("giorno").toInt());
}
// Converte l'oggetto Data in un oggetto XML
QDomElement Data::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Data");
    xml.setAttribute("giorno", static_cast<int>(giorno));
    xml.setAttribute("mese", static_cast<int>(mese));
    xml.setAttribute("anno", static_cast<int>(anno));
    return xml;
}

//OVERLOADING OPERATORI
bool operator==(const Data& lhs, const Data& rhs) {
    return lhs.getGiorno() == rhs.getGiorno()
        && lhs.getMese() == rhs.getMese()
        && lhs.getAnno() == rhs.getAnno();
}

bool operator!=(const Data& lhs, const Data& rhs) {
    return !(lhs == rhs);
}

bool operator<(const Data& lhs, const Data& rhs) {
    return std::make_tuple(lhs.getAnno(), lhs.getMese(), lhs.getGiorno()) <std::make_tuple(rhs.getAnno(), rhs.getMese(), rhs.getGiorno());
}

bool operator>(const Data& lhs, const Data& rhs) {
    return rhs < lhs;
}
