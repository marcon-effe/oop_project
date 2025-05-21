#ifndef CD_H
#define CD_H

#include "Disco.h"

class CD : public Disco {
private:
    std::string formato; // Es: "CD Audio", "Enhanced CD", etc.

public:
    CD(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita,
       const std::string& codice, const std::string& produttoreStampe, const std::string& codiceRiconoscimento,
       const std::string& tipoProdotto, const std::string& formato);

    CD(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita,
       const std::string& codice, const std::string& produttoreStampe, const std::string& codiceRiconoscimento,
       const std::string& tipoProdotto, const std::string& formato, const std::string& img);

    CD(Disco* base, const std::string& formato);

    CD(const CD* other);

    // Costruttore da JSON
    CD(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    CD(Artista* owner, const QDomElement& xml);

    virtual ~CD();

    std::string getFormato() const;
    void setFormato(const std::string& formato);

    // Metodi di utility
    void printInfo() const final override;
    QJsonObject toJson(bool reduced=false) const final override;
    QDomElement toXml(QDomDocument& doc, bool reduced=false) const final override;

    // Visitor
    void accept(VisitorGUI* visitor) const final override;
    void accept(VisitorConsoleEditor* visitor) final override;

    friend bool operator==(const CD& a, const CD& b);
    friend bool operator!=(const CD& a, const CD& b);
};

#endif // CD_H