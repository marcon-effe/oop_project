#ifndef TSHIRT_H
#define TSHIRT_H

#include "Merch.h"

class TShirt : public Merch {
private:
    std::string taglia;
    std::string colore;
public:
    TShirt(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& taglia, const std::string& colore);

    TShirt(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& taglia, const std::string& colore, const std::string& img);

    TShirt(Merch* base, const std::string& taglia, const std::string& colore);

    TShirt(const TShirt* t);

    // Costruttore da JSON
    TShirt(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    TShirt(Artista* owner, const QDomElement& xml);

    virtual ~TShirt();

    std::string getTaglia() const;
    void setTaglia(const std::string& taglia);

    std::string getColore() const;
    void setColore(const std::string& colore);

    virtual void printInfo() const override;
    virtual QJsonObject toJson(bool reduced=false) const override;
    virtual QDomElement toXml(QDomDocument& doc, bool reduced=false) const override;

    virtual void accept(VisitorInterfaceConst* v) const final;
    virtual void accept(VisitorInterfaceNotConst* visitor) final override;

    friend bool operator==(const TShirt& a, const TShirt& b);
    friend bool operator!=(const TShirt& a, const TShirt& b);
};

#endif // TSHIRT_H