#ifndef VINILE_H
#define VINILE_H

#include "Disco.h"
#include "../../include/data_format.h"

class Vinile : public Disco {
private:
    unsigned int rpm;
    unsigned int diametro;
public:
    Vinile(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice, const std::string &produttoreStampe, const std::string &codiceRiconoscimento, const std::string &tipoProdotto, unsigned int rpm, unsigned int diametro);
    Vinile(Disco* d, unsigned int rpm, unsigned int diametro);
    Vinile(const Vinile* v);
    Vinile(const QJsonObject& json);
    Vinile(const QDomElement& xml);
    virtual ~Vinile();

    unsigned int getRpm() const;
    void setRpm(unsigned int r);

    unsigned int getDiametro() const;
    void setDiametro(unsigned int d);

    virtual void printInfo() const final;
    QJsonObject toJson() const final;
    QDomElement toXml(QDomDocument& doc) const final;

    virtual void accept(VisitorGUI* v) const final;

    // OVERLOADING OPERATORI
    friend bool operator==(const Vinile& a, const Vinile& b);
    friend bool operator!=(const Vinile& a, const Vinile& b);
};

#endif
