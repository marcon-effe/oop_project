#include <iostream>
#include "Vinile.h"

#include"../../view/VisitorGUI.h"

Vinile::Vinile(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice, const std::string &produttoreStampe, const std::string &codiceRiconoscimento, const std::string &tipoProdotto, unsigned int rpm, unsigned int diametro)
: Disco(t, desc, prezzo, disponibile, quantita, codice, produttoreStampe, codiceRiconoscimento, tipoProdotto), rpm(rpm), diametro(diametro) {}

Vinile::Vinile(Disco* d, unsigned int rpm, unsigned int diametro)
: Disco(d, d->getProduttoreStampe(), d->getCodiceRiconoscimento(), d->getTipoProdotto()), rpm(rpm), diametro(diametro) {}

Vinile::Vinile(const Vinile* v)
: Disco(v), rpm(v->getRpm()), diametro(v->getDiametro()) {}

Vinile::~Vinile() {}

unsigned int Vinile::getRpm() const {
    return rpm;
}
void Vinile::setRpm(unsigned int r) {
    rpm = r;
}

unsigned int Vinile::getDiametro() const {
    return diametro;
}
void Vinile::setDiametro(unsigned int d) {
    diametro = d;
}

void Vinile::printInfo() const {
    Disco::printInfo();
    std::cout << "RPM: " << rpm << "\nDiametro: " << diametro << " pollici" << std::endl;
}


//JSON
//Converte un oggetto JSON in un oggetto Vinile
Vinile::Vinile(const QJsonObject& json)
: Disco(json["disco"].toObject()),
    rpm(json["rpm"].toInt()),
    diametro(json["diametro"].toInt()){}
//Converte l'oggetto Vinile in un oggetto JSON
QJsonObject Vinile::toJson() const{
    QJsonObject json;
    json["type"] = "vinile";
    json["disco"] = Disco::toJson();
    json["rpm"] = static_cast<int>(rpm);
    json["diametro"] = static_cast<int>(diametro);
    return json;
}

//XML
//Converte un oggetto XML in un oggetto Vinile
Vinile::Vinile(const QDomElement& xml)
: Disco(xml.firstChildElement("Disco")),
    rpm(xml.attribute("rpm").toInt()),
    diametro(xml.attribute("diametro").toInt()) {}
//Converte l'oggetto Vinile in un oggetto XML
QDomElement Vinile::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Vinile");
    xml.setAttribute("rpm", static_cast<int>(rpm));
    xml.setAttribute("diametro", static_cast<int>(diametro));

    xml.appendChild(Disco::toXml(doc));
    return xml;
}


void Vinile::accept(VisitorGUI* v) const {
    v->visit(this);
}

// OVERLOADING OPERATORI
bool operator==(const Vinile& a, const Vinile& b) {
    if (!(static_cast<const Disco&>(a) == static_cast<const Disco&>(b))) return false;
    if (a.diametro != b.diametro) return false;
    if (a.rpm != b.rpm) return false;
    return true;
}

bool operator!=(const Vinile& a, const Vinile& b) {
    return !(a == b);
}