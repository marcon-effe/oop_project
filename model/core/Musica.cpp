#include <iostream>
#include "Musica.h"

#include"../../view/VisitorGUI.h"

Musica::Musica(const std::string &t, const std::string &desc, const Data &du, const Durata &dur, const std::string &g)
: ArtistProduct(t, desc), dataUscita(du), durata(dur), genere(g) {}
  
Musica::Musica(ArtistProduct* ap, const Data& du, const Durata& d, const std::string& g)
: ArtistProduct(*ap), dataUscita(du), durata(d), genere(g) {}

Musica::Musica(const Musica* m)
: ArtistProduct(m), dataUscita(m->getDataUscita()), durata(m->getDurata()), genere(m->getGenere()) {}

Musica::~Musica() {}

Data Musica::getDataUscita() const { return dataUscita; }
void Musica::setDataUscita(const Data &d) { dataUscita = d; }

Durata Musica::getDurata() const { return durata; }
void Musica::setDurata(const Durata &d) { durata = d; }

std::string Musica::getGenere() const { return genere; }
void Musica::setGenere(const std::string &g) { genere = g; }

void Musica::printInfo() const {
    std::cout << "Musica: " << title
              << "\nDescrizione: " << description
              << "\nData uscita: " << dataUscita.getGiorno() << "/" 
                                   << dataUscita.getMese() << "/" 
                                   << dataUscita.getAnno()
              << "\nDurata: " << durata.getOre() << "h " 
                             << durata.getMinuti() << "m " 
                             << durata.getSecondi() << "s"
              << "\nGenere: " << genere << std::endl;
}


//JSON
//Converte un oggetto JSON in un oggetto Musica
Musica::Musica(const QJsonObject &json) 
: ArtistProduct::ArtistProduct(json["ArtistProduct"].toObject()), 
  dataUscita(json["dataUscita"].toObject()), 
  durata(json["durata"].toObject()), 
  genere(json["genere"].toString().toStdString()){}
//Converte l'oggetto Musica in un oggetto JSON
QJsonObject Musica::toJson() const{
  QJsonObject json;
  json["ArtistProduct"] = ArtistProduct::toJson();
  json["dataUscita"] = dataUscita.toJson();
  json["durata"] = durata.toJson();
  json["genere"] = QString::fromStdString(genere);
  return json;
}

//XML
//Converte un oggetto XML in un oggetto Musica
Musica::Musica(const QDomElement &xml)
: ArtistProduct::ArtistProduct(xml.firstChildElement("ArtistProduct")),
  dataUscita(Data(xml.firstChildElement("dataUscita").firstChildElement("Data"))),
  durata(Durata(xml.firstChildElement("Durata"))),
  genere(xml.attribute("genere").toStdString()) {}
//Converte l'oggetto Musica in un oggetto XML
QDomElement Musica::toXml(QDomDocument& doc) const {
  QDomElement musicaEl = doc.createElement("Musica");
  musicaEl.setAttribute("genere", QString::fromStdString(genere));

  QDomElement dataUscitaEl = dataUscita.toXml(doc);
  dataUscitaEl.setTagName("dataUscita"); // override del nome
  musicaEl.appendChild(dataUscitaEl);

  QDomElement durataEl = durata.toXml(doc);
  musicaEl.appendChild(durataEl);

  QDomElement artistaBase = ArtistProduct::toXml(doc);
  musicaEl.appendChild(artistaBase);

  return musicaEl;
}

// OVERLOADING OPERATORI
bool operator==(const Musica& a, const Musica& b) {
  if (!(static_cast<const ArtistProduct&>(a) == static_cast<const ArtistProduct&>(b))) return false;
  if (a.genere != b.genere) return false;
  if (a.dataUscita != b.dataUscita) return false;
  if (a.durata != b.durata) return false;
  return true;
}

bool operator!=(const Musica& a, const Musica& b) {
  return !(a == b);
}