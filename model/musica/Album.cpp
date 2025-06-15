#include <iostream>
#include "Album.h"
#include "../../visitors/VisitorInterfaceConst.h"
#include "../../visitors/VisitorInterfaceNotConst.h"

// COSTRUTTORI
Album::Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::string& lbl)
: Musica(owner, t, desc, du, dur, g), label(lbl)
{
    if (!owner) {
        assert(false && "Album standard constructor received nullptr owner");
        throw std::invalid_argument("Album standard constructor received nullptr owner.");
    }
}

Album::Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::vector<Traccia>& tra, const std::string& lbl)
: Musica(owner, t, desc, du, dur, g), tracce(tra), label(lbl)
{
    if (!owner) {
        assert(false && "Album standard constructor (with tracce) received nullptr owner");
        throw std::invalid_argument("Album standard constructor (with tracce) received nullptr owner.");
    }
}

Album::Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::string& lbl, const std::string& img)
: Musica(owner, t, desc, du, dur, g, img), label(lbl)
{
    if (!owner) {
        assert(false && "Album constructor with imagePath received nullptr owner");
        throw std::invalid_argument("Album constructor with imagePath received nullptr owner.");
    }
}

Album::Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::vector<Traccia>& tra, const std::string& lbl, const std::string& img)
: Musica(owner, t, desc, du, dur, g, img), tracce(tra), label(lbl)
{
    if (!owner) {
        assert(false && "Album constructor with imagePath and tracce received nullptr owner");
        throw std::invalid_argument("Album constructor with imagePath and tracce received nullptr owner.");
    }
}

Album::Album(Musica* m, const std::vector<Traccia>& t, const std::string& l)
: Musica(m), tracce(t), label(l)
{
    if (!m) {
        assert(false && "Album constructor from Musica received nullptr");
        throw std::invalid_argument("Album constructor from Musica received nullptr.");
    }
}

Album::Album(const Album* a)
: Musica(a), tracce(a->getTracce()), label(a->getLabel())
{
    if (!a) {
        assert(false && "Album copy constructor received nullptr Album pointer");
        throw std::invalid_argument("Album copy constructor received nullptr Album pointer.");
    }
}

//JSON
// Converte un oggetto JSON in un oggetto Album
Album::Album(Artista* owner, const QJsonObject& json)
: Musica(owner, json["musica"].toObject()), label(json["label"].toString().toStdString())
{
    if (!owner) {
        assert(false && "Album JSON constructor received nullptr owner");
        throw std::invalid_argument("Album JSON constructor received nullptr owner.");
    }

    QJsonArray tracceArray = json["tracce"].toArray();
    for (const QJsonValue& val : tracceArray) {
        if (val.isObject()) {
            tracce.push_back(Traccia(val.toObject()));
        }
    }
}

// Converte l'oggetto Album in un oggetto JSON
QJsonObject Album::toJson(bool reduced) const {
    QJsonObject json;
    json["type"] = "album";
    json["musica"] = Musica::toJson(reduced);
    
    QJsonArray array_tracce;
    for (const Traccia& t : tracce)
        array_tracce.append(t.toJson());
    json["tracce"] = array_tracce;

    json["label"] = QString::fromStdString(label);
    return json;
}

//XML
// Converte un oggetto XML in un oggetto Album
Album::Album(Artista* owner, const QDomElement& xml)
: Musica(owner, xml.firstChildElement("Musica"))
{
    if (!owner) {
        assert(false && "Album XML constructor received nullptr owner");
        throw std::invalid_argument("Album XML constructor received nullptr owner.");
    }

    QDomNodeList tracceList = xml.firstChildElement("tracce").elementsByTagName("Traccia");
    for (int i = 0; i < tracceList.size(); ++i) {
        QDomElement tracciaElement = tracceList.at(i).toElement();
        if (!tracciaElement.isNull()) {
            tracce.push_back(Traccia(tracciaElement));
        }
    }

    label = xml.attribute("label").toStdString();
}

// Converte l'oggetto Album in un oggetto XML
QDomElement Album::toXml(QDomDocument& doc, bool reduced) const {
    QDomElement albumEl = doc.createElement("Album");
    albumEl.setAttribute("label", QString::fromStdString(label));

    QDomElement tracceElement = doc.createElement("tracce");
    for (const Traccia& t : tracce) {
        QDomElement tracciaElement = t.toXml(doc);
        tracceElement.appendChild(tracciaElement);
    }
    
    albumEl.appendChild(tracceElement);
    albumEl.appendChild(Musica::toXml(doc, reduced));
    
    return albumEl;
}

Album::~Album() {}

std::string Album::getLabel() const { return label; }
void Album::setLabel(const std::string& l) { label = l; }

void Album::addTraccia(const Traccia& tr) {
    tracce.push_back(tr);
}

void Album::removeTraccia(const std::string& nomeTraccia) {
    for (auto it = tracce.begin(); it != tracce.end(); ++it) {
        if (it->getNome() == nomeTraccia) {
            tracce.erase(it);
            break;
        }
    }
}

const std::vector<Traccia>& Album::getTracce() const { return tracce; }

void Album::updateDurata() {
    int totOre = 0, totMin = 0, totSec = 0;
    for (const auto& tr : tracce) {
        totOre += tr.getDurata().getOre();
        totMin += tr.getDurata().getMinuti();
        totSec += tr.getDurata().getSecondi();
    }
    setDurata(Durata(totOre, totMin, totSec));
}

void Album::printInfo() const {
    Musica::printInfo();
    std::cout << "--ALBUM--" << std::endl;
    std::cout << "Etichetta: " << label << "\n";
    std::cout << "Tracce dell'album (" << tracce.size() << "):" << std::endl;
    for (size_t i = 0; i < tracce.size(); ++i) {
        std::cout << (i+1) << ". " << tracce[i].getNome() << std::endl;
    }
}

Traccia& Album::getTracciaModificabile(unsigned int index) {
    if (index >= tracce.size())
        throw std::out_of_range("Indice traccia non valido nell'album.");
    return tracce.at(index);
}

void Album::setTracce(const std::vector<Traccia>& t) {
    this->tracce.clear();
    this->tracce.shrink_to_fit();
    tracce = t;
}


void Album::accept(VisitorInterfaceConst* visitor) const {
    visitor->visit(this);
}

void Album::accept(VisitorInterfaceNotConst* visitor) {
    visitor->visit(this);
}


bool operator==(const Album& a, const Album& b) {
    if (!(static_cast<const Musica&>(a) == static_cast<const Musica&>(b))) return false;
    if (a.label != b.label) return false;
    if (a.tracce != b.tracce) return false;
    return true;
}

bool operator!=(const Album& a, const Album& b) {
    return !(a == b);
}