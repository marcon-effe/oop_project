#ifndef ALBUM_H
#define ALBUM_H

#include <vector>

#include "../core/Musica.h"
#include "Traccia.h"

class Album : public Musica {
private:
    std::vector<Traccia> tracce;
    std::string label; // Etichetta discografica
public:
    // Costruttore standard senza tracce
    Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::string& lbl);

    // Costruttore standard con tracce
    Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::vector<Traccia>& tracce, const std::string& lbl);

    // Costruttore con immagine (senza tracce)
    Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::string& lbl, const std::string& img);

    // Costruttore con immagine (con tracce)
    Album(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const std::vector<Traccia>& tracce, const std::string& lbl, const std::string& img);

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
    void updateDurata() override;

    Traccia& getTracciaModificabile(unsigned int index);

    void setTracce(const std::vector<Traccia>& t);

    // Serializzazione
    virtual QJsonObject toJson(bool reduced=false) const final;
    virtual QDomElement toXml(QDomDocument& doc, bool reduced=false) const final;

    // Stampa info
    virtual void printInfo() const final;

    // Visitor
    virtual void accept(VisitorGUI* visitor) const final;
    virtual void accept(VisitorConsoleEditor* visitor) final;

    // Overloading operatori
    friend bool operator==(const Album& a, const Album& b);
    friend bool operator!=(const Album& a, const Album& b);
};

#endif // ALBUM_H