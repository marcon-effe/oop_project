#include <iostream>
#include "Disco.h"

#include"../../view/VisitorGUI.h"

Disco::Disco(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice, const std::string &produttoreStampe, const std::string &codiceRiconoscimento, const std::string &tipoProdotto)
: Merch(t, desc, prezzo, disponibile, quantita, codice), produttoreStampe(produttoreStampe), codiceRiconoscimento(codiceRiconoscimento), tipoProdotto(tipoProdotto) {}

Disco::Disco(Merch* m, std::string ps, std::string cr, std::string tp)
: Merch(m), produttoreStampe(ps), codiceRiconoscimento(cr), tipoProdotto(tp) {}

Disco::Disco(const Disco* d)
: Merch(d), produttoreStampe(d->getProduttoreStampe()), codiceRiconoscimento(d->getCodiceRiconoscimento()), tipoProdotto(d->getTipoProdotto()) {}



Disco::~Disco() {}

std::string Disco::getProduttoreStampe() const {
    return produttoreStampe;
}
void Disco::setProduttoreStampe(const std::string &ps) {
    produttoreStampe = ps;
}

std::string Disco::getCodiceRiconoscimento() const {
    return codiceRiconoscimento;
}
void Disco::setCodiceRiconoscimento(const std::string &cr) {
    codiceRiconoscimento = cr;
}

std::string Disco::getTipoProdotto() const {
    return tipoProdotto;
}
void Disco::setTipoProdotto(const std::string &tp) {
    tipoProdotto = tp;
}

void Disco::printInfo() const {
    std::cout << "Disco: " << title << "\nDescrizione: " << description
              << "\nPrezzo: " << prezzo << "\nDisponibile: " << (disponibile ? "Si" : "No")
              << "\nQuantita': " << quantita
              << "\nCodice prodotto: " << codiceProdotto
              << "\nProduttore stampe: " << produttoreStampe
              << "\nCodice riconoscimento: " << codiceRiconoscimento
              << "\nTipo prodotto: " << tipoProdotto << std::endl;
}


//JSON
//Converte un oggetto Json in un oggetto Disco
Disco::Disco(const QJsonObject& json)
: Merch(json["merch"].toObject()),
    produttoreStampe(json["produttoreStampe"].toString().toStdString()),
    codiceRiconoscimento(json["codiceRiconoscimento"].toString().toStdString()),
    tipoProdotto(json["tipoProdotto"].toString().toStdString()){}
// Converte un oggetto Disco in un oggetto JSON
QJsonObject Disco::toJson() const{
    QJsonObject json;
    json["merch"] = Merch::toJson();
    json["produttoreStampe"] = QString::fromStdString(produttoreStampe);
    json["codiceRiconoscimento"] = QString::fromStdString(codiceRiconoscimento);
    json["tipoProdotto"] = QString::fromStdString(tipoProdotto);
    return json;
}

//XML
// Converte un oggetto Disco in un oggetto XML
Disco::Disco(const QDomElement& xml)
: Merch(xml.firstChildElement("Merch")),
    produttoreStampe(xml.attribute("produttoreStampe").toStdString()),
    codiceRiconoscimento(xml.attribute("codiceRiconoscimento").toStdString()),
    tipoProdotto(xml.attribute("tipoProdotto").toStdString()){}
// Converte un oggetto Disco in un oggetto XML
QDomElement Disco::toXml(QDomDocument& doc) const{
    QDomElement xml = doc.createElement("Disco");
    xml.setAttribute("produttoreStampe", QString::fromStdString(produttoreStampe));
    xml.setAttribute("codiceRiconoscimento", QString::fromStdString(codiceRiconoscimento));
    xml.setAttribute("tipoProdotto", QString::fromStdString(tipoProdotto));
    
    xml.appendChild(Merch::toXml(doc));
    return xml;
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