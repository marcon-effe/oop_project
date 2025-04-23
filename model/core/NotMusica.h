#ifndef NOTMUSICA_H
#define NOTMUSICA_H

#include <string>

#include "ArtistProduct.h"
#include "../../include/data_format.h"

class NotMusica : public ArtistProduct {
protected:
    double prezzo;
    bool disponibile;
    unsigned int quantita;
public:
    NotMusica(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita);
    NotMusica(ArtistProduct* p, double pr, bool disp, unsigned int q);
    NotMusica(const NotMusica* nm);
    NotMusica(const QJsonObject& json);
    NotMusica(const QDomElement& xml);
    virtual ~NotMusica();

    double getPrezzo() const;
    void setPrezzo(double p);

    bool getDisponibile() const;
    void setDisponibile(bool d);

    unsigned int getQuantita() const;
    void setQuantita(unsigned int q);

    virtual void printInfo() const override;
    virtual QJsonObject toJson() const override;
    virtual QDomElement toXml(QDomDocument& doc) const override;

    // OVERLOADING OPERATORI
    friend bool operator==(const NotMusica& a, const NotMusica& b);
    friend bool operator!=(const NotMusica& a, const NotMusica& b);
};

#endif
