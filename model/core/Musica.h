#ifndef MUSICA_H
#define MUSICA_H

#include "ArtistProduct.h"
#include "../util/Data.h"
#include "../util/Durata.h"


class Musica : public ArtistProduct {
private:
    Data dataUscita;
    Durata durata;
    std::string genere;
public:
    Musica(Artista* owner, const std::string& title, const std::string& desc, const Data& du, const Durata& dur, const std::string& g);

    Musica(Artista* owner, const std::string& title, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::string& img);

    Musica(ArtistProduct* base, const Data& du, const Durata& dur, const std::string& g);

    Musica(const Musica* m);

    Musica(Artista* owner, const QJsonObject& json);

    Musica(Artista* owner, const QDomElement& xml);

    virtual ~Musica();

    Data getDataUscita() const;
    void setDataUscita(const Data& d);

    Durata getDurata() const;
    void setDurata(const Durata& d);

    std::string getGenere() const;
    void setGenere(const std::string& g);

    virtual void updateDurata() = 0;

    virtual void printInfo() const override;
    QJsonObject toJson(bool reduced=false) const override;
    QDomElement toXml(QDomDocument& doc, bool reduced=false) const override;

    friend bool operator==(const Musica& a, const Musica& b);
    friend bool operator!=(const Musica& a, const Musica& b);
};

#endif // MUSICA_H