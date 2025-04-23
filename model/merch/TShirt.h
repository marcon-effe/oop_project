#ifndef TSHIRT_H
#define TSHIRT_H

#include <string>

#include "Merch.h"
#include "../../include/data_format.h"

class TShirt : public Merch {
private:
    std::string taglia;
    std::string colore;
public:
    TShirt(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice, const std::string &taglia, const std::string &colore);
    TShirt(Merch* m, std::string t, std::string c);
    TShirt(const TShirt* t);
    TShirt(const QJsonObject& json);
    TShirt(const QDomElement& xml);
    virtual ~TShirt();

    std::string getTaglia() const;
    void setTaglia(const std::string &taglia);

    std::string getColore() const;
    void setColore(const std::string &colore);

    virtual void printInfo() const override;
    QJsonObject toJson() const override;
    QDomElement toXml(QDomDocument& doc) const override;

    virtual void accept(VisitorGUI* v) const final;

    // OVERLOADING OPERATORI
    friend bool operator==(const TShirt& a, const TShirt& b);
    friend bool operator!=(const TShirt& a, const TShirt& b);
};

#endif