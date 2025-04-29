#include <iostream>
#include "Disco.h"
#include "../../view/VisitorGUI.h"

// COSTRUTTORI

// Costruttore standard
Disco::Disco(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& produttoreStampe, const std::string& codiceRiconoscimento, const std::string& tipoProdotto)
: Merch(owner, t, desc, prezzo, disponibile, quantita, codice),
  produttoreStampe(produttoreStampe),
  codiceRiconoscimento(codiceRiconoscimento),
  tipoProdotto(tipoProdotto)
{
    if (!owner) {
        assert(false && "Disco standard constructor received nullptr owner");
        throw std::invalid_argument("Disco standard constructor received nullptr owner.");
    }
}

// Costruttore di trasformazione da Merch*
Disco::Disco(Merch* base, const std::string& produttoreStampe, const std::string& codiceRiconoscimento, const std::string& tipoProdotto)
: Merch(base),
  produttoreStampe(produttoreStampe),
  codiceRiconoscimento(codiceRiconoscimento),
  tipoProdotto(tipoProdotto)
{
    if (!base) {
        assert(false && "Disco constructor from Merch received nullptr base");
        throw std::invalid_argument("Disco constructor from Merch received nullptr base.");
    }
}

// Costruttore di copia
Disco::Disco(const Disco* d)
: Merch(d),
  produttoreStampe(d->getProduttoreStampe()),
  codiceRiconoscimento(d->getCodiceRiconoscimento()),
  tipoProdotto(d->getTipoProdotto())
{
    if (!d) {
        assert(false && "Disco copy constructor received nullptr Disco pointer");
        throw std::invalid_argument("Disco copy constructor received nullptr Disco pointer.");
    }
}

// JSON

// Costruttore da JSON
Disco::Disco(Artista* owner, const QJsonObject& json)
: Merch(owner, json["merch"].toObject()),
  produttoreStampe(json["produttoreStampe"].toString().toStdString()),
  codiceRiconoscimento(json["codiceRiconoscimento"].toString().toStdString()),
  tipoProdotto(json["tipoProdotto"].toString().toStdString())
{
    if (!owner) {
        assert(false && "Disco JSON constructor received nullptr owner");
        throw std::invalid_argument("Disco JSON constructor received nullptr owner.");
    }
}

// Converte l'oggetto Disco in un oggetto JSON
QJsonObject Disco::toJson() const {
    QJsonObject json;
    json["type"] = "disco";
    json["merch"] = Merch::toJson();
    json["produttoreStampe"] = QString::fromStdString(produttoreStampe);
    json["codiceRiconoscimento"] = QString::fromStdString(codiceRiconoscimento);
    json["tipoProdotto"] = QString::fromStdString(tipoProdotto);
    return json;
}

// XML

// Costruttore da XML
Disco::Disco(Artista* owner, const QDomElement& xml)
: Merch(owner, xml.firstChildElement("Merch")),
  produttoreStampe(xml.attribute("produttoreStampe").toStdString()),
  codiceRiconoscimento(xml.attribute("codiceRiconoscimento").toStdString()),
  tipoProdotto(xml.attribute("tipoProdotto").toStdString())
{
    if (!owner) {
        assert(false && "Disco XML constructor received nullptr owner");
        throw std::invalid_argument("Disco XML constructor received nullptr owner.");
    }
}

// Converte l'oggetto Disco in un oggetto XML
QDomElement Disco::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Disco");
    xml.setAttribute("produttoreStampe", QString::fromStdString(produttoreStampe));
    xml.setAttribute("codiceRiconoscimento", QString::fromStdString(codiceRiconoscimento));
    xml.setAttribute("tipoProdotto", QString::fromStdString(tipoProdotto));

    xml.appendChild(Merch::toXml(doc));
    return xml;
}

// DISTRUTTORE

Disco::~Disco() {}

// GETTER/SETTER

std::string Disco::getProduttoreStampe() const {
    return produttoreStampe;
}

void Disco::setProduttoreStampe(const std::string& ps) {
    produttoreStampe = ps;
}

std::string Disco::getCodiceRiconoscimento() const {
    return codiceRiconoscimento;
}

void Disco::setCodiceRiconoscimento(const std::string& cr) {
    codiceRiconoscimento = cr;
}

std::string Disco::getTipoProdotto() const {
    return tipoProdotto;
}

void Disco::setTipoProdotto(const std::string& tp) {
    tipoProdotto = tp;
}

// PRINT INFO

void Disco::printInfo() const {
    Merch::printInfo();
    std::cout << "--DISCO--" << std::endl;
    std::cout << "Produttore stampe: " << produttoreStampe
              << "\nCodice riconoscimento: " << codiceRiconoscimento
              << "\nTipo prodotto: " << tipoProdotto << std::endl;
}


// OVERLOADING OPERATORI

bool operator==(const Disco& a, const Disco& b) {
    if (!(static_cast<const Merch&>(a) == static_cast<const Merch&>(b))) return false;
    if (a.produttoreStampe != b.produttoreStampe) return false;
    if (a.codiceRiconoscimento != b.codiceRiconoscimento) return false;
    if (a.tipoProdotto != b.tipoProdotto) return false;
    return true;
}

bool operator!=(const Disco& a, const Disco& b) {
    return !(a == b);
}