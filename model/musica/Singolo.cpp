#include <iostream>
#include "Singolo.h"
#include "../../visitors/VisitorInterfaceConst.h"
#include "../../visitors/VisitorInterfaceNotConst.h"

// COSTRUTTORI
Singolo::Singolo(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const Traccia& track, bool remix, int chartPos)
: Musica(owner, t, desc, du, dur, g), mainTrack(track), isRemix(remix), chartPosition(chartPos)
{
    if (!owner) {
        assert(false && "Singolo standard constructor received nullptr owner");
        throw std::invalid_argument("Singolo standard constructor received nullptr owner.");
    }
}

Singolo::Singolo(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const Traccia& track, bool remix, int chartPos, const std::string& img)
: Musica(owner, t, desc, du, dur, g, img), mainTrack(track), isRemix(remix), chartPosition(chartPos)
{
    if (!owner) {
        assert(false && "Singolo constructor with imagePath received nullptr owner");
        throw std::invalid_argument("Singolo constructor with imagePath received nullptr owner.");
    }
}

Singolo::Singolo(Musica* m, const Traccia& t, bool remix, int chartPos)
: Musica(m), mainTrack(t), isRemix(remix), chartPosition(chartPos)
{
    if (!m) {
        assert(false && "Singolo constructor from Musica received nullptr");
        throw std::invalid_argument("Singolo constructor from Musica received nullptr.");
    }
}

Singolo::Singolo(const Singolo* s)
: Musica(s), mainTrack(s->getMainTrack()), isRemix(s->getIsRemix()), chartPosition(s->getChartPosition())
{
    if (!s) {
        assert(false && "Singolo copy constructor received nullptr Singolo pointer");
        throw std::invalid_argument("Singolo copy constructor received nullptr Singolo pointer.");
    }
}

//JSON
// Costruttore da JSON
Singolo::Singolo(Artista* owner, const QJsonObject& json)
: Musica(owner, json["musica"].toObject()),
  mainTrack(json["mainTrack"].toObject()),
  isRemix(json["isRemix"].toBool()),
  chartPosition(json["chartPosition"].toInt())
{
    if (!owner) {
        assert(false && "Singolo JSON constructor received nullptr owner");
        throw std::invalid_argument("Singolo JSON constructor received nullptr owner.");
    }
}

// Converte l'oggetto Singolo in un oggetto JSON
QJsonObject Singolo::toJson(bool reduced) const {
    QJsonObject json;
    json["type"] = "singolo";
    json["musica"] = Musica::toJson(reduced);
    json["mainTrack"] = mainTrack.toJson();
    json["isRemix"] = isRemix;
    json["chartPosition"] = chartPosition;
    return json;
}

//XML
// Costruttore da XML
Singolo::Singolo(Artista* owner, const QDomElement& xml)
: Musica(owner, xml.firstChildElement("Musica")),
  mainTrack(xml.firstChildElement("mainTrack")),
  isRemix(xml.attribute("isRemix") == "true"),
  chartPosition(xml.attribute("chartPosition").toInt())
{
    if (!owner) {
        assert(false && "Singolo XML constructor received nullptr owner");
        throw std::invalid_argument("Singolo XML constructor received nullptr owner.");
    }
}

// Converte l'oggetto Singolo in un oggetto XML
QDomElement Singolo::toXml(QDomDocument& doc, bool reduced) const {
    QDomElement singoloEl = doc.createElement("Singolo");
    
    singoloEl.setAttribute("isRemix", isRemix ? "true" : "false");
    singoloEl.setAttribute("chartPosition", QString::number(chartPosition));
    
    // Inserimento della traccia principale
    QDomElement track = mainTrack.toXml(doc);
    track.setTagName("mainTrack");
    singoloEl.appendChild(track);

    // Inserimento dell'elemento <Musica>
    singoloEl.appendChild(Musica::toXml(doc, reduced));

    return singoloEl;
}

Singolo::~Singolo() {}

Traccia Singolo::getMainTrack() const { return mainTrack; }
void Singolo::setMainTrack(const Traccia& t) { mainTrack = t; }

bool Singolo::getIsRemix() const { return isRemix; }
void Singolo::setIsRemix(bool r) { isRemix = r; }

int Singolo::getChartPosition() const { return chartPosition; }
void Singolo::setChartPosition(int pos) { chartPosition = pos; }

Traccia& Singolo::getTracciaModificabile() {
    return mainTrack;
}

void Singolo::updateDurata() {
    this->setDurata(mainTrack.getDurata());
}

void Singolo::printInfo() const {
    Musica::printInfo();
    std::cout << "--SINGOLO--" << std::endl;
    std::cout << "Singolo - Traccia principale: " << mainTrack.getNome()
              << "\nRemix: " << (isRemix ? "Sì" : "No")
              << "\nPosizione in classifica: " << chartPosition << std::endl;
}

void Singolo::accept(VisitorInterfaceConst* visitor) const {
    visitor->visit(this);
}

void Singolo::accept(VisitorInterfaceNotConst* visitor) {
    visitor->visit(this);
}

bool operator==(const Singolo& a, const Singolo& b) {
    if (!(static_cast<const Musica&>(a) == static_cast<const Musica&>(b))) return false;
    if (a.mainTrack != b.mainTrack) return false;
    if (a.isRemix != b.isRemix) return false;
    if (a.chartPosition != b.chartPosition) return false;
    return true;
}

bool operator!=(const Singolo& a, const Singolo& b) {
    return !(a == b);
}