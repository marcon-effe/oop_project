#include <iostream>
#include "CD.h"
#include "../../view/VisitorGUI.h"


CD::CD(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita,
       const std::string& codice, const std::string& produttoreStampe, const std::string& codiceRiconoscimento,
       const std::string& tipoProdotto, const std::string& formato)
: Disco(owner, t, desc, prezzo, disponibile, quantita, codice, produttoreStampe, codiceRiconoscimento, tipoProdotto), formato(formato) {}

CD::CD(Disco* base, const std::string& formato)
: Disco(base), formato(formato) {}

CD::CD(const CD* other)
: Disco(other), formato(other->getFormato()) {}

CD::~CD() {}

std::string CD::getFormato() const {
    return formato;
}

void CD::setFormato(const std::string& f) {
    formato = f;
}

void CD::printInfo() const {
    Disco::printInfo();
    std::cout << "--CD--" << std::endl;
    std::cout << "Formato: " << formato << std::endl;
}

// JSON
CD::CD(Artista* owner, const QJsonObject& json)
: Disco(owner, json["disco"].toObject()),
  formato(json["formato"].toString().toStdString()) {}

QJsonObject CD::toJson() const {
    QJsonObject json;
    json["type"] = "cd";
    json["disco"] = Disco::toJson();
    json["formato"] = QString::fromStdString(formato);
    return json;
}

// XML
CD::CD(Artista* owner, const QDomElement& xml)
: Disco(owner, xml.firstChildElement("Disco")),
  formato(xml.attribute("formato").toStdString()) {}

QDomElement CD::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("CD");
    xml.setAttribute("formato", QString::fromStdString(formato));
    xml.appendChild(Disco::toXml(doc));
    return xml;
}

// Visitor
void CD::accept(VisitorGUI* visitor) const {
    visitor->visit(this);
}

// OVERLOADING OPERATORI
bool operator==(const CD& a, const CD& b) {
    if (!(static_cast<const Disco&>(a) == static_cast<const Disco&>(b))) return false;
    return a.formato == b.formato;
}

bool operator!=(const CD& a, const CD& b) {
    return !(a == b);
}