// CONSIDERANE LA RIMOZIONE PERCHE' GIA' PRESENTE ORARIO
// L'implementazione e' praticamente la stessa

#ifndef DURATA_H
#define DURATA_H

#include <string>

#include "../../include/data_format.h"

class Durata {
public:
    Durata(unsigned int h, unsigned int m, unsigned int s);
    Durata(unsigned int s);
    Durata(const QJsonObject& json);
    Durata(const QDomElement& xml);
    ~Durata();

    unsigned int getOre() const;
    unsigned int getMinuti() const;
    unsigned int getSecondi() const;

    void setOre(unsigned int h);
    void setMinuti(unsigned int m);
    void setSecondi(unsigned int s);

    std::string toString() const;
    QJsonObject toJson() const;
    QDomElement toXml(QDomDocument& doc) const;

    //OVERLOADING OPERATORI
    friend bool operator==(const Durata& a, const Durata& b);
    friend bool operator!=(const Durata& a, const Durata& b);
    friend bool operator<(const Durata& a, const Durata& b);
    friend bool operator>(const Durata& a, const Durata& b);

private:
    unsigned int ore;
    unsigned int minuti;
    unsigned int secondi;

    void normalize();
};

#endif // DURATA_H
