#ifndef MERCH_H
#define MERCH_H

#include <string>

#include <QJsonObject>
#include <QJsonArray>
#include <QDomElement>
#include <QDomDocument>

#include "../core/NotMusica.h"

class Merch : public NotMusica {
protected:
    std::string codiceProdotto;
public:
    // Costruttore standard
    Merch(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice);

    // Costruttore con immagine
    Merch(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& img);

    // Costruttore di trasformazione da NotMusica*
    Merch(NotMusica* base, const std::string& codice);

    // Costruttore di copia
    Merch(const Merch* m);

    // Costruttore da JSON
    Merch(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    Merch(Artista* owner, const QDomElement& xml);

    // Distruttore
    virtual ~Merch();

    // Getter / Setter
    std::string getCodiceProdotto() const;
    void setCodiceProdotto(const std::string& codice);

    // Info print
    virtual void printInfo() const override;

    // Serializzazione
    virtual QJsonObject toJson(bool reduced=false) const override;
    virtual QDomElement toXml(QDomDocument& doc, bool reduced=false) const override;

    // Overloading operatori
    friend bool operator==(const Merch& a, const Merch& b);
    friend bool operator!=(const Merch& a, const Merch& b);
};

#endif // MERCH_H