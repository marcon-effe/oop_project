#include <iostream>
#include "TShirt.h"
#include "../../view/VisitorGUI.h"
#include "../../cli/VisitorConsoleEditor.h"

// COSTRUTTORI
// Costruttore standard
TShirt::TShirt(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& taglia, const std::string& colore)
: Merch(owner, t, desc, prezzo, disponibile, quantita, codice), taglia(taglia), colore(colore)
{
    if (!owner) {
        assert(false && "TShirt standard constructor received nullptr owner");
        throw std::invalid_argument("TShirt standard constructor received nullptr owner.");
    }
}

// Costruttore di trasformazione da Merch*
TShirt::TShirt(Merch* base, const std::string& taglia, const std::string& colore)
: Merch(base), taglia(taglia), colore(colore)
{
    if (!base) {
        assert(false && "TShirt constructor from Merch received nullptr base");
        throw std::invalid_argument("TShirt constructor from Merch received nullptr base.");
    }
}

// Costruttore di copia
TShirt::TShirt(const TShirt* t)
: Merch(t), taglia(t->getTaglia()), colore(t->getColore())
{
    if (!t) {
        assert(false && "TShirt copy constructor received nullptr TShirt pointer");
        throw std::invalid_argument("TShirt copy constructor received nullptr TShirt pointer.");
    }
}

// JSON
// Costruttore da JSON
TShirt::TShirt(Artista* owner, const QJsonObject& json)
: Merch(owner, json["merch"].toObject()),
  taglia(json["taglia"].toString().toStdString()),
  colore(json["colore"].toString().toStdString())
{
    if (!owner) {
        assert(false && "TShirt JSON constructor received nullptr owner");
        throw std::invalid_argument("TShirt JSON constructor received nullptr owner.");
    }
}

// Converte l'oggetto TShirt in un oggetto JSON
QJsonObject TShirt::toJson() const {
    QJsonObject json;
    json["type"] = "tshirt";
    json["merch"] = Merch::toJson();
    json["taglia"] = QString::fromStdString(taglia);
    json["colore"] = QString::fromStdString(colore);
    return json;
}

// XML
// Costruttore da XML
TShirt::TShirt(Artista* owner, const QDomElement& xml)
: Merch(owner, xml.firstChildElement("Merch")),
  taglia(xml.attribute("taglia").toStdString()),
  colore(xml.attribute("colore").toStdString())
{
    if (!owner) {
        assert(false && "TShirt XML constructor received nullptr owner");
        throw std::invalid_argument("TShirt XML constructor received nullptr owner.");
    }
}

// Converte l'oggetto TShirt in un oggetto XML
QDomElement TShirt::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("TShirt");
    xml.setAttribute("taglia", QString::fromStdString(taglia));
    xml.setAttribute("colore", QString::fromStdString(colore));
    
    xml.appendChild(Merch::toXml(doc));
    return xml;
}

// DISTRUTTORE
TShirt::~TShirt() {}

// GETTER/SETTER
std::string TShirt::getTaglia() const {
    return taglia;
}

void TShirt::setTaglia(const std::string& t) {
    taglia = t;
}

std::string TShirt::getColore() const {
    return colore;
}

void TShirt::setColore(const std::string& c) {
    colore = c;
}

// PRINT INFO
void TShirt::printInfo() const {
    Merch::printInfo();
    std::cout << "--TSHIRT--" << std::endl;
    std::cout << "Taglia: " << taglia
              << "\nColore: " << colore << std::endl;
}


// VISITOR
void TShirt::accept(VisitorGUI* visitor) const {
    visitor->visit(this);
}

void TShirt::accept(VisitorConsoleEditor* visitor) {
    visitor->visit(this);
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