#include <iostream>
#include "Vinile.h"

#include "../../view/VisitorGUI.h"

// Costruttori
Vinile::Vinile(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile,
               unsigned int quantita, const std::string& codice, const std::string& produttoreStampe,
               const std::string& codiceRiconoscimento, const std::string& tipoProdotto,
               unsigned int rpm, unsigned int diametro)
: Disco(owner, t, desc, prezzo, disponibile, quantita, codice, produttoreStampe, codiceRiconoscimento, tipoProdotto),
  rpm(rpm), diametro(diametro) {}

Vinile::Vinile(Disco* d, unsigned int rpm, unsigned int diametro)
: Disco(d), rpm(rpm), diametro(diametro) {}

Vinile::Vinile(const Vinile* v)
: Disco(v), rpm(v->getRpm()), diametro(v->getDiametro()) {}

Vinile::~Vinile() {}

// Getter e Setter
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

// Stampa info
void Vinile::printInfo() const {
    Disco::printInfo();
    std::cout << "RPM: " << rpm << "\nDiametro: " << diametro << " pollici" << std::endl;
}

// JSON
// Converte un oggetto JSON in un oggetto Vinile
Vinile::Vinile(Artista* owner, const QJsonObject& json)
: Disco(owner, json["disco"].toObject()),
  rpm(json["rpm"].toInt()),
  diametro(json["diametro"].toInt()) {}

// Converte l'oggetto Vinile in un oggetto JSON
QJsonObject Vinile::toJson() const {
    QJsonObject json;
    json["type"] = "vinile";
    json["disco"] = Disco::toJson();
    json["rpm"] = static_cast<int>(rpm);
    json["diametro"] = static_cast<int>(diametro);
    return json;
}

// XML
// Converte un oggetto XML in un oggetto Vinile
Vinile::Vinile(Artista* owner, const QDomElement& xml)
: Disco(owner, xml.firstChildElement("Disco")),
  rpm(xml.attribute("rpm").toUInt()),
  diametro(xml.attribute("diametro").toUInt()) {}

// Converte l'oggetto Vinile in un oggetto XML
QDomElement Vinile::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Vinile");
    xml.setAttribute("rpm", static_cast<int>(rpm));
    xml.setAttribute("diametro", static_cast<int>(diametro));
    xml.appendChild(Disco::toXml(doc));
    return xml;
}

// Visitor
void Vinile::accept(VisitorGUI* v) const {
    v->visit(this);
}

// OVERLOADING OPERATORI
bool operator==(const Vinile& a, const Vinile& b) {
    if (!(static_cast<const Disco&>(a) == static_cast<const Disco&>(b))) return false;
    if (a.rpm != b.rpm) return false;
    if (a.diametro != b.diametro) return false;
    return true;
}

bool operator!=(const Vinile& a, const Vinile& b) {
    return !(a == b);
}