#ifndef CD_H
#define CD_H

#include <string>

#include "Disco.h"
#include "../../include/data_format.h"

class CD : public Disco {
private:
    std::string formato; // Es: "CD Audio", "Enhanced CD", etc.
public:
    CD(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice, const std::string &produttoreStampe, const std::string &codiceRiconoscimento, const std::string &tipoProdotto, const std::string &formato);
    CD(Disco* d, std::string f);
    CD(const CD* c);
    CD(const QJsonObject& json);
    CD(const QDomElement& xml);
    virtual ~CD();

    std::string getFormato() const;
    void setFormato(const std::string &f);

    virtual void printInfo() const final;
    QJsonObject toJson() const final;
    QDomElement toXml(QDomDocument& doc) const final;

    virtual void accept(VisitorGUI* v) const final;

    // OVERLOADING OPERATORI
    friend bool operator==(const CD& a, const CD& b);
    friend bool operator!=(const CD& a, const CD& b);
};

#endif
