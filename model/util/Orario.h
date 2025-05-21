#ifndef ORARIO_H
#define ORARIO_H

#include <QJsonObject>
#include <QJsonArray>
#include <QDomElement>
#include <QDomDocument>
#include <QTime>

class Orario {
private:
    unsigned int ore;
    unsigned int minuti;
    unsigned int secondi;
public:
    Orario(unsigned int o = 0, unsigned int m = 0, unsigned int s = 0);
    Orario(const QJsonObject& json);
    Orario(const QDomElement& xml);
    ~Orario();

    unsigned int getOre() const;
    void setOre(unsigned int o);

    unsigned int getMinuti() const;
    void setMinuti(unsigned int m);

    unsigned int getSecondi() const;
    void setSecondi(unsigned int s);

    std::string toString() const;
    QTime toQTime() const;

    Orario getOrario() const;
    QJsonObject toJson() const;
    QDomElement toXml(QDomDocument& doc) const;

    friend bool operator==(const Orario& a, const Orario& b);
    friend bool operator!=(const Orario& a, const Orario& b);
    friend bool operator<(const Orario& a, const Orario& b);
    friend bool operator>(const Orario& a, const Orario& b);
};

#endif
