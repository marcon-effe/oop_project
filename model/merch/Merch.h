#ifndef MERCH_H
#define MERCH_H

#include <string>

#include "../core/NotMusica.h"
#include "../../include/data_format.h"

class Merch : public NotMusica {
protected:
    std::string codiceProdotto;
public:
    Merch(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice);
    Merch(NotMusica* nm, std::string cp);
    Merch(const Merch* m);
    Merch(const QJsonObject& obj);
    Merch(const QDomElement& xml);
    virtual ~Merch();

    std::string getCodiceProdotto() const;
    void setCodiceProdotto(const std::string &codice);

    virtual void printInfo() const override;
    QJsonObject toJson() const override;
    QDomElement toXml(QDomDocument& doc) const override;

    // OVERLOADING OPERATORI
    friend bool operator==(const Merch& a, const Merch& b);
    friend bool operator!=(const Merch& a, const Merch& b);
};

#endif