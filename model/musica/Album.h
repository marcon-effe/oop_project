#ifndef ALBUM_H
#define ALBUM_H

#include <vector>
#include <string>

#include "../core/Musica.h"
#include "Traccia.h"
#include "../../include/data_format.h"

class Album : public Musica {
private:
    std::vector<Traccia> tracce;
    std::string label; // Etichetta discografica
public:
    // Costruttore standard senza tracce
    Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::string& lbl);

    // Costruttore standard con tracce
    Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::vector<Traccia>& tracce, const std::string& lbl);

    // Costruttore di trasformazione da Musica*
    Album(Musica* base, const std::vector<Traccia>& tracce, const std::string& lbl);

    // Costruttore di copia
    Album(const Album* a);

    // Costruttore da JSON
    Album(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    Album(Artista* owner, const QDomElement& xml);

    // Distruttore
    virtual ~Album();

    // Getter / Setter
    std::string getLabel() const;
    void setLabel(const std::string& l);

    void addTraccia(const Traccia& tr);
    void removeTraccia(const std::string& nomeTraccia);
    const std::vector<Traccia>& getTracce() const;
    Durata getDurataTotale() const;

    // Serializzazione
    virtual QJsonObject toJson() const final;
    virtual QDomElement toXml(QDomDocument& doc) const final;

    // Stampa info
    virtual void printInfo() const final;

    // Visitor
    virtual void accept(VisitorGUI* visitor) const final;

    // Overloading operatori
    friend bool operator==(const Album& a, const Album& b);
    friend bool operator!=(const Album& a, const Album& b);
};

#endif // ALBUM_H