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
    // Costruttore standard
    NotMusica(Artista* owner, const std::string& title, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita);

    // Costruttore con immagine
    NotMusica(Artista* owner, const std::string& title, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& img);

    // Costruttore di trasformazione da ArtistProduct*
    NotMusica(ArtistProduct* base, double prezzo, bool disponibile, unsigned int quantita);

    // Costruttore di copia
    NotMusica(const NotMusica* nm);

    // Costruttore da JSON
    NotMusica(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    NotMusica(Artista* owner, const QDomElement& xml);

    // Distruttore
    virtual ~NotMusica();

    // Getter / Setter
    double getPrezzo() const;
    void setPrezzo(double p);

    bool getDisponibile() const;
    void setDisponibile(bool d);

    unsigned int getQuantita() const;
    void setQuantita(unsigned int q);

    // Altri metodi
    virtual void printInfo() const override;
    virtual QJsonObject toJson() const override;
    virtual QDomElement toXml(QDomDocument& doc) const override;

    // Overloading operatori
    friend bool operator==(const NotMusica& a, const NotMusica& b);
    friend bool operator!=(const NotMusica& a, const NotMusica& b);
};

#endif // NOTMUSICA_H