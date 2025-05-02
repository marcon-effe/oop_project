#ifndef SINGOLO_H
#define SINGOLO_H

#include <string>

#include "../core/Musica.h" 
#include "Traccia.h"
#include "../../include/data_format.h"

class Singolo : public Musica {
private:
    Traccia mainTrack;
    bool isRemix;
    int chartPosition;
public:
    // Costruttore standard
    Singolo(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const Traccia& track, bool remix, int chartPos);

    // Costruttore di trasformazione da Musica*
    Singolo(Musica* base, const Traccia& mainTrack, bool isRemix, int chartPosition);

    // Costruttore di copia
    Singolo(const Singolo* s);

    // Costruttore da JSON
    Singolo(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    Singolo(Artista* owner, const QDomElement& xml);

    // Distruttore
    virtual ~Singolo();

    // Getter / Setter
    Traccia getMainTrack() const;
    void setMainTrack(const Traccia& t);

    bool getIsRemix() const;
    void setIsRemix(bool r);

    int getChartPosition() const;
    void setChartPosition(int pos);

    void updateDurata() override;

    Traccia& getTracciaModificabile();

    // Serializzazione
    virtual QJsonObject toJson() const final;
    virtual QDomElement toXml(QDomDocument& doc) const final;

    // Info print
    virtual void printInfo() const final;

    // Visitor
    virtual void accept(VisitorGUI* visitor) const final;
    virtual void accept(VisitorConsoleEditor* visitor) final;

    // Overloading operatori
    friend bool operator==(const Singolo& a, const Singolo& b);
    friend bool operator!=(const Singolo& a, const Singolo& b);
};

#endif // SINGOLO_H