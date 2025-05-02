#include <iostream>
#include "Album.h"
#include "../../view/VisitorGUI.h"

// COSTRUTTORI

// Costruttore standard senza tracce
Album::Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::string& lbl)
: Musica(owner, t, desc, du, dur, g), label(lbl)
{
    if (!owner) {
        assert(false && "Album standard constructor received nullptr owner");
        throw std::invalid_argument("Album standard constructor received nullptr owner.");
    }
}

// Costruttore standard con tracce
Album::Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::vector<Traccia>& tra, const std::string& lbl)
: Musica(owner, t, desc, du, dur, g), tracce(tra), label(lbl)
{
    if (!owner) {
        assert(false && "Album standard constructor (with tracce) received nullptr owner");
        throw std::invalid_argument("Album standard constructor (with tracce) received nullptr owner.");
    }
}

// Costruttore di trasformazione da Musica*
Album::Album(Musica* m, const std::vector<Traccia>& t, const std::string& l)
: Musica(m), tracce(t), label(l)
{
    if (!m) {
        assert(false && "Album constructor from Musica received nullptr");
        throw std::invalid_argument("Album constructor from Musica received nullptr.");
    }
}

// Costruttore di copia
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
QJsonObject Album::toJson() const {
    QJsonObject json;
    json["type"] = "album";
    json["musica"] = Musica::toJson();
    
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
QDomElement Album::toXml(QDomDocument& doc) const {
    QDomElement albumEl = doc.createElement("Album");
    albumEl.setAttribute("label", QString::fromStdString(label));

    QDomElement tracceElement = doc.createElement("tracce");
    for (const Traccia& t : tracce) {
        QDomElement tracciaElement = t.toXml(doc);
        tracceElement.appendChild(tracciaElement);
    }
    
    albumEl.appendChild(tracceElement);
    albumEl.appendChild(Musica::toXml(doc));
    
    return albumEl;
}

// DISTRUTTORE
Album::~Album() {}

// METODI SPECIFICI
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

Durata Album::getDurataTotale() const {
    unsigned int ore = 0, minuti = 0, secondi = 0;

    for (const Traccia& t : tracce) {
        Durata d = t.getDurata();
        ore += d.getOre();
        minuti += d.getMinuti();
        secondi += d.getSecondi();
    }

    return Durata(ore, minuti, secondi); // Normalizzazione automatica
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

// VISITOR
void Album::accept(VisitorGUI* visitor) const {
    visitor->visit(this);
}

// OVERLOADING OPERATORI
bool operator==(const Album& a, const Album& b) {
    if (!(static_cast<const Musica&>(a) == static_cast<const Musica&>(b))) return false;
    if (a.label != b.label) return false;
    if (a.tracce != b.tracce) return false;
    return true;
}

bool operator!=(const Album& a, const Album& b) {
    return !(a == b);
}