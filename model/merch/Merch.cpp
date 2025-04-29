#include <iostream>
#include "Merch.h"
#include "../../view/VisitorGUI.h"

// COSTRUTTORI

// Costruttore standard
Merch::Merch(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice)
: NotMusica(owner, t, desc, prezzo, disponibile, quantita), codiceProdotto(codice)
{
    if (!owner) {
        assert(false && "Merch standard constructor received nullptr owner");
        throw std::invalid_argument("Merch standard constructor received nullptr owner.");
    }
}

// Costruttore di trasformazione da NotMusica*
Merch::Merch(NotMusica* base, const std::string& codice)
: NotMusica(base), codiceProdotto(codice)
{
    if (!base) {
        assert(false && "Merch constructor from NotMusica received nullptr base");
        throw std::invalid_argument("Merch constructor from NotMusica received nullptr base.");
    }
}

// Costruttore di copia
Merch::Merch(const Merch* m)
: NotMusica(m), codiceProdotto(m->getCodiceProdotto())
{
    if (!m) {
        assert(false && "Merch copy constructor received nullptr Merch pointer");
        throw std::invalid_argument("Merch copy constructor received nullptr Merch pointer.");
    }
}

// JSON
// Costruttore da JSON
Merch::Merch(Artista* owner, const QJsonObject& json)
: NotMusica(owner, json["not_musica"].toObject()),
  codiceProdotto(json["codiceProdotto"].toString().toStdString())
{
    if (!owner) {
        assert(false && "Merch JSON constructor received nullptr owner");
        throw std::invalid_argument("Merch JSON constructor received nullptr owner.");
    }
}

// Converte l'oggetto Merch in un oggetto JSON
QJsonObject Merch::toJson() const {
    QJsonObject json;
    json["type"] = "merch";
    json["not_musica"] = NotMusica::toJson();
    json["codiceProdotto"] = QString::fromStdString(codiceProdotto);
    return json;
}

// XML
// Costruttore da XML
Merch::Merch(Artista* owner, const QDomElement& xml)
: NotMusica(owner, xml.firstChildElement("NotMusica")),
  codiceProdotto(xml.attribute("codiceProdotto").toStdString())
{
    if (!owner) {
        assert(false && "Merch XML constructor received nullptr owner");
        throw std::invalid_argument("Merch XML constructor received nullptr owner.");
    }
}

// Converte l'oggetto Merch in un oggetto XML
QDomElement Merch::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Merch");
    xml.setAttribute("codiceProdotto", QString::fromStdString(codiceProdotto));

    xml.appendChild(NotMusica::toXml(doc));
    return xml;
}

// DISTRUTTORE
Merch::~Merch() {}

// GETTER/SETTER
std::string Merch::getCodiceProdotto() const {
    return codiceProdotto;
}

void Merch::setCodiceProdotto(const std::string& codice) {
    codiceProdotto = codice;
}

// PRINT INFO
void Merch::printInfo() const {
    NotMusica::printInfo();
    std::cout << "Codice prodotto: " << codiceProdotto << std::endl;
}

// OVERLOADING OPERATORI
bool operator==(const Merch& a, const Merch& b) {
    if (!(static_cast<const NotMusica&>(a) == static_cast<const NotMusica&>(b))) return false;
    if (a.codiceProdotto != b.codiceProdotto) return false;
    return true;
}

bool operator!=(const Merch& a, const Merch& b) {
    return !(a == b);
}