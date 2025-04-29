#include <iostream>

#include "Musica.h"
#include "../../view/VisitorGUI.h"

// COSTRUTTORI

// Costruttore standard
Musica::Musica(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g)
: ArtistProduct(owner, t, desc), dataUscita(du), durata(dur), genere(g)
{
    if (!owner) {
        assert(false && "Musica standard constructor received nullptr owner");
        throw std::invalid_argument("Musica standard constructor received nullptr owner.");
    }
}

// Costruttore di trasformazione da ArtistProduct*
Musica::Musica(ArtistProduct* base, const Data& du, const Durata& dur, const std::string& g)
: ArtistProduct(*base), dataUscita(du), durata(dur), genere(g)
{
    if (!base) {
        assert(false && "Musica constructor from ArtistProduct received nullptr base");
        throw std::invalid_argument("Musica constructor from ArtistProduct received nullptr base.");
    }
}

// Costruttore di copia da Musica*
Musica::Musica(const Musica* m)
: ArtistProduct(m), dataUscita(m->getDataUscita()), durata(m->getDurata()), genere(m->getGenere())
{
    if (!m) {
        assert(false && "Musica copy constructor received nullptr Musica pointer");
        throw std::invalid_argument("Musica copy constructor received nullptr Musica pointer.");
    }
}

//JSON
// Converte un oggetto JSON in un oggetto Musica
Musica::Musica(Artista* owner, const QJsonObject& json)
: ArtistProduct(owner, json["ArtistProduct"].toObject()),
  dataUscita(json["dataUscita"].toObject()),
  durata(json["durata"].toObject()),
  genere(json["genere"].toString().toStdString())
{
    if (!owner) {
        assert(false && "Musica JSON constructor received nullptr owner");
        throw std::invalid_argument("Musica JSON constructor received nullptr owner.");
    }
}

// Converte l'oggetto Musica in un oggetto JSON
QJsonObject Musica::toJson() const {
    QJsonObject json;
    json["type"] = "musica"; // tipo specifico
    json["ArtistProduct"] = ArtistProduct::toJson();
    json["dataUscita"] = dataUscita.toJson();
    json["durata"] = durata.toJson();
    json["genere"] = QString::fromStdString(genere);
    return json;
}


//XML
// Converte un oggetto XML in un oggetto Musica
Musica::Musica(Artista* owner, const QDomElement& xml)
: ArtistProduct(owner, xml),
  dataUscita(Data(xml.firstChildElement("dataUscita").firstChildElement("Data"))),
  durata(Durata(xml.firstChildElement("Durata"))),
  genere(xml.attribute("genere").toStdString())
{
    if (!owner) {
        assert(false && "Musica XML constructor received nullptr owner");
        throw std::invalid_argument("Musica XML constructor received nullptr owner.");
    }
}

// Converte l'oggetto Musica in un oggetto XML
QDomElement Musica::toXml(QDomDocument& doc) const {
    QDomElement musicaEl = doc.createElement("Musica");
    musicaEl.setAttribute("genere", QString::fromStdString(genere));

    QDomElement dataUscitaEl = dataUscita.toXml(doc);
    dataUscitaEl.setTagName("dataUscita");
    musicaEl.appendChild(dataUscitaEl);

    QDomElement durataEl = durata.toXml(doc);
    musicaEl.appendChild(durataEl);

    QDomElement artistaBase = ArtistProduct::toXml(doc);
    musicaEl.appendChild(artistaBase);

    return musicaEl;
}

// DISTRUTTORE
Musica::~Musica() {}

// GETTER/SETTER
Data Musica::getDataUscita() const { return dataUscita; }
void Musica::setDataUscita(const Data& d) { dataUscita = d; }

Durata Musica::getDurata() const { return durata; }
void Musica::setDurata(const Durata& d) { durata = d; }

std::string Musica::getGenere() const { return genere; }
void Musica::setGenere(const std::string& g) { genere = g; }

// PRINT INFO
void Musica::printInfo() const {
    ArtistProduct::printInfo();
    std::cout << "--MUSICA--" << std::endl;
    std::cout << "Data di uscita: " << dataUscita.toString() << std::endl;
    std::cout << "Durata: " << durata.toString() << std::endl;
    std::cout << "Genere: " << genere << std::endl;
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