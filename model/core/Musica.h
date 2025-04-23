#ifndef MUSICA_H
#define MUSICA_H

#include <string>

#include "ArtistProduct.h" // Include the full definition of ArtistProduct
#include "../util/Data.h"
#include "../util/Durata.h"
#include "../../include/data_format.h"

class Musica : public ArtistProduct {
private:
    Data dataUscita;
    Durata durata;
    std::string genere;
public:
    Musica(const std::string &t, const std::string &desc, const Data &du, const Durata &dur, const std::string &g);
    Musica(ArtistProduct* ap, const Data& du, const Durata& d, const std::string& g);
    Musica(const Musica* m);
    Musica(const QJsonObject &json);
    Musica(const QDomElement &xml);
    virtual ~Musica();

    Data getDataUscita() const;
    void setDataUscita(const Data &d);

    Durata getDurata() const;
    void setDurata(const Durata &d);

    std::string getGenere() const;
    void setGenere(const std::string &g);

    virtual void printInfo() const override;
    
    QJsonObject toJson() const override;
    QDomElement toXml(QDomDocument& doc) const override;

    // OVERLOADING OPERATORI
    friend bool operator==(const Musica& a, const Musica& b);
    friend bool operator!=(const Musica& a, const Musica& b);
};

#endif
