#include <iostream>
#include "DataTour.h"

DataTour::DataTour(unsigned int g, unsigned int m, unsigned int a, unsigned int o, unsigned int min, unsigned int s, const std::string &l)
: Data(g, m, a), Orario(o, min, s), luogo(l) {}

DataTour::~DataTour() {}

Data DataTour::getData() const {
    return *static_cast<const Data*>(this);
}

void DataTour::setData(const Data& d) {
    Data::operator=(d);
}

Orario DataTour::getOrario() const {
    return *static_cast<const Orario*>(this);
}

void DataTour::setOrario(const Orario& o) {
    Orario::operator=(o);
}

std::string DataTour::getLuogo() const {
    return luogo;
}

void DataTour::setLuogo(const std::string &l) {
    luogo = l;
}


//JSON
// Converte un oggetto JSON in un oggetto DataTour
DataTour::DataTour(const QJsonObject& json) 
: Data::Data(json["data"].toObject()), 
    Orario::Orario(json["orario"].toObject()), 
    luogo(json["luogo"].toString().toStdString()){}
// Converte l'oggetto DataTour in un oggetto JSON
QJsonObject DataTour::toJson() const{
    QJsonObject json;
    json["data"] = Data::toJson();
    json["orario"] = Orario::toJson();
    json["luogo"] = QString::fromStdString(luogo);
    return json;
}

// XML
// Converte un oggetto XML in un oggetto DataTour
DataTour::DataTour(const QDomElement& xml)
: Data::Data(xml.firstChildElement("Data")),
  Orario::Orario(xml.firstChildElement("Orario")),
  luogo(xml.attribute("luogo").toStdString()) {}
// Converte l'oggetto DataTour in un oggetto XML
QDomElement DataTour::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("DataTour");
    xml.setAttribute("luogo", QString::fromStdString(luogo));

    QDomElement dataElem = Data::toXml(doc);
    xml.appendChild(dataElem);

    QDomElement orarioElem = Orario::toXml(doc);
    xml.appendChild(orarioElem);

    return xml;
}


// OVERLOADING OPERATORI
bool operator==(const DataTour& lhs, const DataTour& rhs) {
    return static_cast<const Data&>(lhs) == static_cast<const Data&>(rhs)
        && static_cast<const Orario&>(lhs) == static_cast<const Orario&>(rhs)
        && lhs.getLuogo() == rhs.getLuogo();
}

bool operator!=(const DataTour& a, const DataTour& b) {
    return !(a == b);
}