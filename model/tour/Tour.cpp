#include <iostream>
#include "Tour.h"

#include "../../view/VisitorGUI.h"
#include "../../cli/VisitorConsoleEditor.h"

// Costruttore standard
Tour::Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita)
: NotMusica(owner, t, desc, prezzo, disponibile, quantita) {}

// Costruttore con date
Tour::Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::vector<DataTour>& dt)
: NotMusica(owner, t, desc, prezzo, disponibile, quantita), dateTour(dt) {}

// Costruttore con immagine
Tour::Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& img)
: NotMusica(owner, t, desc, prezzo, disponibile, quantita, img) {}

// Costruttore con immagine e date
Tour::Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& img, const std::vector<DataTour>& dt)
: NotMusica(owner, t, desc, prezzo, disponibile, quantita, img), dateTour(dt) {}

// Costruttore da NotMusica*
Tour::Tour(NotMusica* n, const std::vector<DataTour>& dt)
: NotMusica(n), dateTour(dt) {}

// Costruttore di copia
Tour::Tour(const Tour* t)
: NotMusica(t), dateTour(t->getDateTour()) {}

Tour::~Tour() {}

void Tour::addDataTour(const DataTour& dt) {
    dateTour.push_back(dt);
}

void Tour::removeDataTour(unsigned int index) {
    if (index < dateTour.size())
        dateTour.erase(dateTour.begin() + index);
}

const std::vector<DataTour>& Tour::getDateTour() const {
    return dateTour;
}

std::vector<DataTour>& Tour::getDateTourModificabile() {
    return dateTour;
}

void Tour::printInfo() const {
    NotMusica::printInfo();
    std::cout << "--TOUR--" << std::endl;
    std::cout << "Date del tour:\n";

    for (size_t i = 0; i < dateTour.size(); ++i) {
        std::cout << " " << i + 1 << ". " 
                  << dateTour[i].getGiorno() << "/" 
                  << dateTour[i].getMese() << "/" 
                  << dateTour[i].getAnno() << " - "
                  << dateTour[i].getOrario().getOre() << ":"
                  << dateTour[i].getOrario().getMinuti() << ":"
                  << dateTour[i].getOrario().getSecondi()
                  << " @ " << dateTour[i].getLuogo() << std::endl;
    }
}


// JSON
// Costruttore da JSON
Tour::Tour(Artista* owner, const QJsonObject& json)
: NotMusica(owner, json["not_musica"].toObject()) {
    QJsonArray dateArray = json["dateTour"].toArray();
    for (const QJsonValue& val : dateArray) {
        if (val.isObject()) {
            dateTour.push_back(DataTour(val.toObject()));
        }
    }
}

// Salvataggio in JSON
QJsonObject Tour::toJson() const {
    QJsonObject json;
    json["type"] = "tour";
    json["not_musica"] = NotMusica::toJson();

    QJsonArray dateArray;
    for (const DataTour& dt : dateTour)
        dateArray.append(dt.toJson());
    json["dateTour"] = dateArray;

    return json;
}


// XML
// Costruttore da XML
Tour::Tour(Artista* owner, const QDomElement& xml)
: NotMusica(owner, xml.firstChildElement("NotMusica")) {
    QDomElement dateTourElem = xml.firstChildElement("dateTour");
    if (!dateTourElem.isNull()) {
        QDomNodeList dates = dateTourElem.elementsByTagName("DataTour");
        for (int i = 0; i < dates.size(); ++i) {
            QDomElement d = dates.at(i).toElement();
            if (!d.isNull()) {
                dateTour.push_back(DataTour(d));
            }
        }
    }
}

// Salvataggio in XML
QDomElement Tour::toXml(QDomDocument& doc) const {
    QDomElement xml = doc.createElement("Tour");

    if (!dateTour.empty()) {
        QDomElement dateTourElem = doc.createElement("dateTour");
        for (const DataTour& dt : dateTour) {
            dateTourElem.appendChild(dt.toXml(doc));
        }
        xml.appendChild(dateTourElem);
    }

    xml.appendChild(NotMusica::toXml(doc));
    return xml;
}


// VISITOR
void Tour::accept(VisitorGUI* v) const {
    v->visit(this);
}

void Tour::accept(VisitorConsoleEditor* visitor) {
    visitor->visit(this);
}

// OVERLOADING OPERATORI
bool operator==(const Tour& a, const Tour& b) {
    return static_cast<const NotMusica&>(a) == static_cast<const NotMusica&>(b)
        && a.dateTour == b.dateTour;
}

bool operator!=(const Tour& a, const Tour& b) {
    return !(a == b);
}