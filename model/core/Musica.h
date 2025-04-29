#ifndef MUSICA_H
#define MUSICA_H

#include <string>
#include "ArtistProduct.h"
#include "../util/Data.h"
#include "../util/Durata.h"
#include "../../include/data_format.h"

class Musica : public ArtistProduct {
private:
    Data dataUscita;
    Durata durata;
    std::string genere;
public:
    // Costruttore standard
    Musica(Artista* owner, const std::string& title, const std::string& desc, const Data& du, const Durata& dur, const std::string& g);

    // Costruttore di "trasformazione" da ArtistProduct base
    Musica(ArtistProduct* base, const Data& du, const Durata& dur, const std::string& g);

    // Costruttore di copia da Musica*
    Musica(const Musica* m);

    // Costruttore da JSON
    Musica(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    Musica(Artista* owner, const QDomElement& xml);

    // Distruttore
    virtual ~Musica();

    // Getter/Setter
    Data getDataUscita() const;
    void setDataUscita(const Data& d);

    Durata getDurata() const;
    void setDurata(const Durata& d);

    std::string getGenere() const;
    void setGenere(const std::string& g);

    // Metodi di stampa/salvataggio
    virtual void printInfo() const override;
    QJsonObject toJson() const override;
    QDomElement toXml(QDomDocument& doc) const override;

    // Overloading operatori
    friend bool operator==(const Musica& a, const Musica& b);
    friend bool operator!=(const Musica& a, const Musica& b);
};

#endif // MUSICA_H