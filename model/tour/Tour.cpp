#include <iostream>
#include "Tour.h"

Tour::Tour(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita)
: NotMusica(t, desc, prezzo, disponibile, quantita) {}

Tour::Tour(NotMusica* n, const std::vector<DataTour>& dateTour)
: NotMusica(n, n->getPrezzo(), n->getDisponibile(), n->getQuantita()), dateTour(dateTour) {}

Tour::Tour(const Tour* t)
: NotMusica(t), dateTour(t->getDateTour()) {}

Tour::~Tour() {}

void Tour::addDataTour(const DataTour &dt) {
    dateTour.push_back(dt);
}

void Tour::removeDataTour(unsigned int index) {
    if (index < dateTour.size())
        dateTour.erase(dateTour.begin() + index);
}

const std::vector<DataTour>& Tour::getDateTour() const {
    return dateTour;
}

void Tour::printInfo() const {
    std::cout 
        << "Tour: " << title << std::endl
        << "Descrizione: " << description << std::endl
        << "Prezzo: " << getPrezzo() << std::endl
        << "Disponibilita': " << (getDisponibile() ? "Si" : "No") << std::endl
        << "Quantita': " << getQuantita() << std::endl
        << "Date del tour:" << std::endl;

    for (size_t i = 0; i < dateTour.size(); ++i) {
        std::cout << " " << i+1 << ". " << dateTour[i].getGiorno() << "/" 
                  << dateTour[i].getMese() << "/" << dateTour[i].getAnno()
                  << " - " << dateTour[i].getOrario().getOre() << ":"
                  << dateTour[i].getOrario().getMinuti() << ":"
                  << dateTour[i].getOrario().getSecondi() << " at " 
                  << dateTour[i].getLuogo() << std::endl;
    }
}


//JSON
// Converte un oggetto JSON in un oggetto Tour
Tour::Tour(const QJsonObject& json)
: NotMusica(json["not_musica"].toObject()){
    QJsonArray dateArray = json["dateTour"].toArray();
    for (const QJsonValue& val : dateArray) {
        if (val.isObject()) {
            dateTour.push_back(DataTour(val.toObject()));
        }
    }
}
// Converte l'oggetto Tour in un oggetto JSON
QJsonObject Tour::toJson() const{
    QJsonObject json;
    json["type"] = "tour";
    json["not_musica"] = NotMusica::toJson();
    QJsonArray array_dateTour;
    for(DataTour dt : dateTour)
        array_dateTour.append(dt.toJson());
    json["dateTour"] = array_dateTour;
    return json; 
}

//XML
// Converte un oggetto XML in un oggetto Tour
Tour::Tour(const QDomElement& xml)
: NotMusica(xml.firstChildElement("NotMusica")) 
{
    QDomElement dateTourEl = xml.firstChildElement("dateTour");
    if (!dateTourEl.isNull()) {
        QDomNodeList dateNodes = dateTourEl.elementsByTagName("DataTour");
        for (int i = 0; i < dateNodes.size(); ++i) {
            QDomElement dateElement = dateNodes.at(i).toElement();
            if (!dateElement.isNull()) {
                dateTour.push_back(DataTour(dateElement));
            }
        }
    }
}
// Converte l'oggetto Tour in un oggetto XML
QDomElement Tour::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Tour");

    if (!dateTour.empty()) {
        QDomElement dateTourElement = doc.createElement("dateTour");
        for (const DataTour& dt : dateTour) {
            dateTourElement.appendChild(dt.toXml(doc));
        }
        xml.appendChild(dateTourElement);
    }

    xml.appendChild(NotMusica::toXml(doc));
    
    return xml;
}

// VISITOR
void Tour::accept(VisitorGUI* v) const {
    v->visit(this);
}

// OVERLOADING OPERATORI
bool operator==(const Tour& lhs, const Tour& rhs) {
    return static_cast<const NotMusica&>(lhs) == static_cast<const NotMusica&>(rhs)
        && lhs.getDateTour() == rhs.getDateTour();
}

bool operator!=(const Tour& a, const Tour& b) {
    return !(a == b);
}