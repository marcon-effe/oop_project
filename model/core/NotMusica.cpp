#include <iostream>

#include "NotMusica.h"
#include "../../view/VisitorGUI.h"

// COSTRUTTORI

// Costruttore standard
NotMusica::NotMusica(Artista* owner, const std::string& t, const std::string& desc, double p, bool d, unsigned int q)
: ArtistProduct(owner, t, desc), prezzo(p), disponibile(d), quantita(q)
{
    if (!owner) {
        assert(false && "NotMusica standard constructor received nullptr owner");
        throw std::invalid_argument("NotMusica standard constructor received nullptr owner.");
    }
}

// Costruttore con immagine
NotMusica::NotMusica(Artista* owner, const std::string& t, const std::string& desc, double p, bool d, unsigned int q, const std::string& img)
: ArtistProduct(owner, t, desc, img), prezzo(p), disponibile(d), quantita(q)
{
    if (!owner) {
        assert(false && "NotMusica constructor with imagePath received nullptr owner");
        throw std::invalid_argument("NotMusica constructor with imagePath received nullptr owner.");
    }
}

// Costruttore di trasformazione da ArtistProduct*
NotMusica::NotMusica(ArtistProduct* base, double pr, bool disp, unsigned int q)
: ArtistProduct(*base), prezzo(pr), disponibile(disp), quantita(q)
{
    if (!base) {
        assert(false && "NotMusica constructor from ArtistProduct received nullptr base");
        throw std::invalid_argument("NotMusica constructor from ArtistProduct received nullptr base.");
    }
}

// Costruttore di copia da NotMusica*
NotMusica::NotMusica(const NotMusica* nm)
: ArtistProduct(nm), prezzo(nm->getPrezzo()), disponibile(nm->getDisponibile()), quantita(nm->getQuantita())
{
    if (!nm) {
        assert(false && "NotMusica copy constructor received nullptr NotMusica pointer");
        throw std::invalid_argument("NotMusica copy constructor received nullptr NotMusica pointer.");
    }
}

//JSON
// Converte un oggetto JSON in un oggetto NotMusica
NotMusica::NotMusica(Artista* owner, const QJsonObject& json)
: ArtistProduct(owner, json["ArtistProduct"].toObject()),
  prezzo(json["prezzo"].toDouble()),
  disponibile(json["disponibile"].toBool()),
  quantita(static_cast<unsigned int>(json["quantita"].toInt()))
{
    if (!owner) {
        assert(false && "NotMusica JSON constructor received nullptr owner");
        throw std::invalid_argument("NotMusica JSON constructor received nullptr owner.");
    }
}

// Converte l'oggetto NotMusica in un oggetto JSON
QJsonObject NotMusica::toJson() const {
    QJsonObject json;
    json["type"] = "notmusica"; // specifica tipo
    json["ArtistProduct"] = ArtistProduct::toJson();
    json["prezzo"] = prezzo;
    json["disponibile"] = disponibile;
    json["quantita"] = static_cast<int>(quantita);
    return json;
}

//XML
// Converte un oggetto XML in un oggetto NotMusica
NotMusica::NotMusica(Artista* owner, const QDomElement& xml)
: ArtistProduct(owner, xml),
  prezzo(xml.attribute("prezzo").toDouble()),
  disponibile(xml.attribute("disponibile").toInt()),
  quantita(xml.attribute("quantita").toUInt())
{
    if (!owner) {
        assert(false && "NotMusica XML constructor received nullptr owner");
        throw std::invalid_argument("NotMusica XML constructor received nullptr owner.");
    }
}

// Converte l'oggetto NotMusica in un oggetto XML   
QDomElement NotMusica::toXml(QDomDocument& doc) const {
    QDomElement notMusicaEl = doc.createElement("NotMusica");
    notMusicaEl.setAttribute("prezzo", QString::number(prezzo, 'f', 2));
    notMusicaEl.setAttribute("disponibile", disponibile ? 1 : 0);
    notMusicaEl.setAttribute("quantita", static_cast<int>(quantita));

    QDomElement artistProductEl = ArtistProduct::toXml(doc);
    notMusicaEl.appendChild(artistProductEl);

    return notMusicaEl;
}

// DISTRUTTORE
NotMusica::~NotMusica() {}

// GETTER/SETTER
double NotMusica::getPrezzo() const { return prezzo; }
void NotMusica::setPrezzo(double p) { prezzo = p; }

bool NotMusica::getDisponibile() const { return disponibile; }
void NotMusica::setDisponibile(bool d) { disponibile = d; }

unsigned int NotMusica::getQuantita() const { return quantita; }
void NotMusica::setQuantita(unsigned int q) { quantita = q; }

// PRINT INFO
void NotMusica::printInfo() const {
    ArtistProduct::printInfo();
    std::cout << "--NOT MUSICA--" << std::endl;
    std::cout << "Prezzo: " << prezzo << std::endl;
    std::cout << "Disponibile: " << (disponibile ? "Si" : "No") << std::endl;
    std::cout << "Quantita: " << quantita << std::endl;
}

// OVERLOADING OPERATORI
bool operator==(const NotMusica& a, const NotMusica& b) {
    if (!(static_cast<const ArtistProduct&>(a) == static_cast<const ArtistProduct&>(b))) {
        return false;
    }

    constexpr double epsilon = 1e-6;
    if (std::abs(a.prezzo - b.prezzo) > epsilon || a.disponibile != b.disponibile || a.quantita != b.quantita) {
        return false;
    }

    return true;
}

bool operator!=(const NotMusica& a, const NotMusica& b) {
    return !(a == b);
}