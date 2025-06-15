#ifndef NOTMUSICA_H
#define NOTMUSICA_H

#include "ArtistProduct.h"

class NotMusica : public ArtistProduct {
protected:
    double prezzo;
    bool disponibile;
    unsigned int quantita;
public:
    NotMusica(Artista* owner, const std::string& title, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita);

    NotMusica(Artista* owner, const std::string& title, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& img);

    NotMusica(ArtistProduct* base, double prezzo, bool disponibile, unsigned int quantita);

    NotMusica(const NotMusica* nm);

    NotMusica(Artista* owner, const QJsonObject& json);

    NotMusica(Artista* owner, const QDomElement& xml);

    virtual ~NotMusica();

    double getPrezzo() const;
    void setPrezzo(double p);

    bool getDisponibile() const;
    void setDisponibile(bool d);

    unsigned int getQuantita() const;
    void setQuantita(unsigned int q);

    virtual void printInfo() const override;
    virtual QJsonObject toJson(bool reduced=false) const override;
    virtual QDomElement toXml(QDomDocument& doc, bool reduced=false) const override;

    friend bool operator==(const NotMusica& a, const NotMusica& b);
    friend bool operator!=(const NotMusica& a, const NotMusica& b);
};

#endif // NOTMUSICA_H