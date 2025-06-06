#include <iostream>
#include "Traccia.h"
#include <QDebug>

#include "../../visitors/VisitorInterfaceNotConst.h"

Traccia::Traccia(const std::string &n, const std::vector<std::string> &parts, const Durata &d, const std::string &t, bool ht)
: nome(n), partecipanti(parts), durata(d), testo(t), hasTesto(ht){
    if (!ht) testo = "";
}

Traccia::~Traccia() {}

std::string Traccia::getNome() const {
    return nome;
}

void Traccia::setNome(const std::string &n) {
    nome = n;
}

Durata Traccia::getDurata() const {
    return durata;
}

void Traccia::setDurata(const Durata &d) {
    durata = d;
}

std::string Traccia::getTesto() const {
    return testo;
}

void Traccia::setTesto(const std::string &t) {
    testo = t;
    hasTesto = !t.empty();
}

void Traccia::setHasTesto(bool ht) {
    hasTesto = ht;
    if (!ht) testo = "";
}

bool Traccia::hasTestoPresent() const {
    return hasTesto;
}

const std::vector<std::string>& Traccia::getPartecipanti() const {
    return partecipanti;
}

void Traccia::addPartecipante(const std::string &a) {
    partecipanti.push_back(a);
}

void Traccia::removePartecipante(const std::string &a) {
    auto it = std::find(partecipanti.begin(), partecipanti.end(), a);
    if (it != partecipanti.end())
        partecipanti.erase(it);
    else
        throw std::invalid_argument("Partecipante non trovato.");
}

void Traccia::removePartecipante(unsigned int index) {
    if (index >= partecipanti.size())
        throw std::out_of_range("Indice partecipante non valido.");
    partecipanti.erase(partecipanti.begin() + index);
}

void Traccia::editPartecipante(unsigned int index, const std::string& nuovoNome) {
    if(index >= partecipanti.size())
        throw std::out_of_range("Indice non valido.");
    partecipanti[index] = nuovoNome;
}

void Traccia::setPartecipanti(const std::vector<std::string>& parts) {
    this->partecipanti.clear();
    this->partecipanti.shrink_to_fit();
    partecipanti = parts;
}


//JSON
//Converte un oggetto JSON in un oggetto Traccia
Traccia::Traccia(const QJsonObject &json) 
:   nome(json["nome"].toString().toStdString()), 
    durata(json["durata"].toObject()),
    testo(json["testo"].toString().toStdString()),
    hasTesto(json["has_testo"].toBool()){
        QJsonArray array_partecipanti = json["partecipanti"].toArray();
        for (const QJsonValue &v : array_partecipanti)
            partecipanti.push_back(v.toString().toStdString());
    }
//Converte l'oggetto Traccia in un oggetto JSON
QJsonObject Traccia::toJson() const {
    QJsonObject json;
    json["nome"] = QString::fromStdString(nome);
    
    QJsonArray array_partecipanti;
    for (const std::string &p : partecipanti)
        array_partecipanti.append(QString::fromStdString(p));

    json["partecipanti"] = array_partecipanti;
    json["durata"] = durata.toJson();
    json["testo"] = QString::fromStdString(testo);
    json["has_testo"] = hasTesto;
    return json;
}

//XML
//Converte un oggetto XML in un oggetto Traccia
Traccia::Traccia(const QDomElement &xml)
:   nome(xml.attribute("nome").toStdString()),
    durata(Durata(xml.firstChildElement("Durata"))),
    testo(xml.firstChildElement("testo").text().toStdString()),
    hasTesto(xml.attribute("has_testo").toInt())
    {
        QDomElement partecipantiEl = xml.firstChildElement("partecipanti");
        if(!partecipantiEl.isNull()) {
            QDomNodeList partecipantiList = partecipantiEl.elementsByTagName("partecipante");
            for (int i = 0; i < partecipantiList.size(); ++i) {
                QDomElement elem = partecipantiList.at(i).toElement();
                std::string nomePartecipante = elem.attribute("nome").toStdString();
                partecipanti.push_back(nomePartecipante);
            }
        }
    }
//Converte l'oggetto Traccia in un oggetto XML
QDomElement Traccia::toXml(QDomDocument& doc) const {
    QDomElement tracciaElem = doc.createElement("Traccia");
    tracciaElem.setAttribute("nome", QString::fromStdString(nome));
    tracciaElem.setAttribute("has_testo", hasTesto);
    
    QDomElement partecipantiElem = doc.createElement("partecipanti");
    for (const std::string &p : partecipanti) {
        QDomElement partecipanteElem = doc.createElement("partecipante");
        partecipanteElem.setAttribute("nome", QString::fromStdString(p));  // <-- GIUSTO ORA
        partecipantiElem.appendChild(partecipanteElem);
    }
    tracciaElem.appendChild(partecipantiElem);

    QDomElement durataElem = durata.toXml(doc);
    tracciaElem.appendChild(durataElem);
    
    QDomElement testoElem = doc.createElement("testo");
    testoElem.appendChild(doc.createTextNode(QString::fromStdString(testo)));
    tracciaElem.appendChild(testoElem);
    
    return tracciaElem;
}

void Traccia::accept(VisitorInterfaceNotConst* visitor) {
    visitor->visit(this);
}

bool operator==(const Traccia& lhs, const Traccia& rhs) {
    return lhs.getNome() == rhs.getNome() &&
           lhs.getDurata() == rhs.getDurata() &&
           lhs.hasTestoPresent() == rhs.hasTestoPresent() &&
           lhs.getTesto() == rhs.getTesto() &&
           lhs.getPartecipanti() == rhs.getPartecipanti();
}

bool operator!=(const Traccia& a, const Traccia& b) {
    return !(a == b);
}