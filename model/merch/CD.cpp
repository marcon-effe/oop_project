#include <iostream>
#include "CD.h"

#include"../../view/VisitorGUI.h"

CD::CD(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice, const std::string &produttoreStampe, const std::string &codiceRiconoscimento, const std::string &tipoProdotto, const std::string &formato)
: Disco(t, desc, prezzo, disponibile, quantita, codice, produttoreStampe, codiceRiconoscimento, tipoProdotto), formato(formato) {}

CD::CD(Disco* d, std::string f)
: Disco(d), formato(f) {}

CD::CD(const CD* c)
: Disco(c), formato(c->getFormato()) {}
  


CD::~CD() {}

std::string CD::getFormato() const {
    return formato;
}

void CD::setFormato(const std::string &f) {
    formato = f;
}

void CD::printInfo() const {
    Disco::printInfo();
    std::cout << "Formato: " << formato << std::endl;
}


//JSON
//Converte un oggetto JSON in un oggetto CD
CD::CD(const QJsonObject& json)
: Disco(json["disco"].toObject()),
    formato(json["formato"].toString().toStdString()){}
//Converte l'oggetto CD in un oggetto JSON
QJsonObject CD::toJson() const{
    QJsonObject json;
    json["type"] = "cd";
    json["disco"] = Disco::toJson();
    json["formato"] = QString::fromStdString(formato);
    return json;
}

//XML
//Converte un oggetto XML in un oggetto CD
CD::CD(const QDomElement& xml)
: Disco(xml.firstChildElement("Disco")),
    formato(xml.attribute("formato").toStdString()) {}
//Converte l'oggetto CD in un oggetto XML
QDomElement CD::toXml(QDomDocument& doc) const{
    QDomElement xml = doc.createElement("CD");
    xml.setAttribute("formato", QString::fromStdString(formato));
    xml.appendChild(Disco::toXml(doc));
    return xml;
}


void CD::accept(VisitorGUI* v) const {
    v->visit(this);
}

// OVERLOADING OPERATORI
bool operator==(const CD& a, const CD& b) {
    if (!(static_cast<const Disco&>(a) == static_cast<const Disco&>(b))) return false;
    if (a.formato != b.formato) return false;
    return true;
}

bool operator!=(const CD& a, const CD& b) {
    return !(a == b);
}