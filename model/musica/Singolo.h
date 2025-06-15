#ifndef SINGOLO_H
#define SINGOLO_H

#include "../core/Musica.h" 
#include "Traccia.h"

class Singolo : public Musica {
private:
    Traccia mainTrack;
    bool isRemix;
    int chartPosition;
public:
    Singolo(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const Traccia& track, bool remix, int chartPos);

    Singolo(Artista* owner, const std::string& t, const std::string& desc, const Data& du, const Durata& dur, const std::string& g, const Traccia& track, bool remix, int chartPos, const std::string& img);

    Singolo(Musica* base, const Traccia& mainTrack, bool isRemix, int chartPosition);

    Singolo(const Singolo* s);

    // Costruttore da JSON
    Singolo(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    Singolo(Artista* owner, const QDomElement& xml);

    virtual ~Singolo();

    Traccia getMainTrack() const;
    void setMainTrack(const Traccia& t);

    bool getIsRemix() const;
    void setIsRemix(bool r);

    int getChartPosition() const;
    void setChartPosition(int pos);

    void updateDurata() override;

    Traccia& getTracciaModificabile();

    virtual QJsonObject toJson(bool reduced=false) const final;
    virtual QDomElement toXml(QDomDocument& doc, bool reduced=false) const final;

    virtual void printInfo() const final;

    virtual void accept(VisitorInterfaceConst* visitor) const final;
    virtual void accept(VisitorInterfaceNotConst* visitor) final;

    friend bool operator==(const Singolo& a, const Singolo& b);
    friend bool operator!=(const Singolo& a, const Singolo& b);
};

#endif // SINGOLO_H