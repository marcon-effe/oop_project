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
    Singolo(const std::string &t, const std::string &desc, const Data &du, const Durata &dur, const std::string &g, const Traccia &track, bool remix, int chartPos);
    Singolo(Musica* m, Traccia t, bool r, int c);
    Singolo(const Singolo* s);
    Singolo(const QJsonObject& json);
    Singolo(const QDomElement& xml);
    virtual ~Singolo();

    Traccia getMainTrack() const;
    void setMainTrack(const Traccia &t);

    bool getIsRemix() const;
    void setIsRemix(bool r);

    int getChartPosition() const;
    void setChartPosition(int pos);

    virtual void printInfo() const final;
    QJsonObject toJson() const final;
    QDomElement toXml(QDomDocument& doc) const final;

    virtual void accept(VisitorGUI* v) const final;

    // OVERLOADING OPERATORI
    friend bool operator==(const Singolo& a, const Singolo& b);
    friend bool operator!=(const Singolo& a, const Singolo& b);
};

#endif
