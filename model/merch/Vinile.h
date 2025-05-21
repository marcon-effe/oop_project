#ifndef VINILE_H
#define VINILE_H

#include "Disco.h"

class Vinile : public Disco {
private:
    unsigned int rpm;
    unsigned int diametro;
public:
    Vinile(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile,
           unsigned int quantita, const std::string& codice, const std::string& produttoreStampe,
           const std::string& codiceRiconoscimento, const std::string& tipoProdotto, unsigned int rpm, unsigned int diametro);

    Vinile(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile,
           unsigned int quantita, const std::string& codice, const std::string& produttoreStampe,
           const std::string& codiceRiconoscimento, const std::string& tipoProdotto, unsigned int rpm, unsigned int diametro,
           const std::string& img);

    Vinile(Disco* d, unsigned int rpm, unsigned int diametro);
    Vinile(const Vinile* v);
    Vinile(Artista* owner, const QJsonObject& json);
    Vinile(Artista* owner, const QDomElement& xml);
    virtual ~Vinile();

    unsigned int getRpm() const;
    void setRpm(unsigned int r);

    unsigned int getDiametro() const;
    void setDiametro(unsigned int d);

    virtual void printInfo() const final;
    QJsonObject toJson(bool reduced=false) const final;
    QDomElement toXml(QDomDocument& doc, bool reduced=false) const final;

    virtual void accept(VisitorGUI* v) const final;
    virtual void accept(VisitorConsoleEditor* v) final;

    // OVERLOADING OPERATORI
    friend bool operator==(const Vinile& a, const Vinile& b);
    friend bool operator!=(const Vinile& a, const Vinile& b);
};

#endif // VINILE_H