#include <iostream>
#include "Singolo.h"

#include"../../view/VisitorGUI.h"

Singolo::Singolo(const std::string &t, const std::string &desc, const Data &du, const Durata &dur, const std::string &g, const Traccia &track, bool remix, int chartPos)
: Musica(t, desc, du, dur, g), mainTrack(track), isRemix(remix), chartPosition(chartPos) {}

Singolo::Singolo(Musica* m, Traccia t, bool r, int c)
: Musica(m), mainTrack(t), isRemix(r), chartPosition(c) {}
  
Singolo::Singolo(const Singolo* s)
: Musica(s), mainTrack(s->getMainTrack()), isRemix(s->getIsRemix()), chartPosition(s->getChartPosition()) {}

Singolo::~Singolo() {}

Traccia Singolo::getMainTrack() const { return mainTrack; }
void Singolo::setMainTrack(const Traccia &t) { mainTrack = t; }

bool Singolo::getIsRemix() const { return isRemix; }
void Singolo::setIsRemix(bool r) { isRemix = r; }

int Singolo::getChartPosition() const { return chartPosition; }
void Singolo::setChartPosition(int pos) { chartPosition = pos; }

void Singolo::printInfo() const {
    Musica::printInfo();
    std::cout << "Singolo - Traccia principale: " << mainTrack.getNome()
              << "\nRemix: " << (isRemix ? "Sì" : "No")
              << "\nPosizione in classifica: " << chartPosition << std::endl;
}


//JSON
// Costruttore da JSON
Singolo::Singolo(const QJsonObject& json)
:Musica(json["musica"].toObject()),
  mainTrack(json["mainTrack"].toObject()),
  isRemix(json["isRemix"].toBool()),
  chartPosition(json["chartPosition"].toInt()){}
//Converte l'oggetto Singolo in un oggetto JSON
QJsonObject Singolo::toJson() const{
  QJsonObject json;
  json["type"] = "singolo";
  json["musica"] = Musica::toJson();
  json["mainTrack"] = mainTrack.toJson();
  json["isRemix"] = isRemix;
  json["chartPosition"] = chartPosition;
  return json;
}

//XML
// Costruttore da XML
Singolo::Singolo(const QDomElement& xml)
: Musica(xml.firstChildElement("Musica")),
  mainTrack(xml.firstChildElement("mainTrack")),
  isRemix(xml.attribute("isRemix") == "true"),
  chartPosition(xml.attribute("chartPosition").toInt()){}
// Converte l'oggetto Singolo in un oggetto XML
QDomElement Singolo::toXml(QDomDocument& doc) const {
  QDomElement singoloEl = doc.createElement("Singolo");
  
  singoloEl.setAttribute("isRemix", isRemix ? "true" : "false");
  singoloEl.setAttribute("chartPosition", QString::number(chartPosition));
  
  // Inserimento della traccia principale
  QDomElement track = mainTrack.toXml(doc);
  track.setTagName("mainTrack");
  singoloEl.appendChild(track);

  // Inserimento dell'elemento <musica>
  singoloEl.appendChild(Musica::toXml(doc));

  return singoloEl;
}

void Singolo::accept(VisitorGUI* v) const {
  v->visit(this);
}

// OVERLOADING OPERATORI
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
