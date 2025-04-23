#ifndef ALBUM_H
#define ALBUM_H


#include <vector>
#include <string>

#include "../core/Musica.h"
#include "Traccia.h"
#include "../../include/data_format.h"

class Album : public Musica {
private:
    std::vector<Traccia> tracce;
    std::string label; // etichetta discografica
public:
    Album(const std::string &t, const std::string &desc, const Data &du, const Durata &dur, const std::string &g, const std::string &lbl);
    Album(const std::string &t, const std::string &desc, const Data &du, const Durata &dur, const std::string &g, const std::vector<Traccia>& traccePtr, const std::string &lbl);
    Album(Musica* m, const std::vector<Traccia>& t, const std::string& l);
    Album(const Album* a);
    Album(const QJsonObject& json);
    Album(const QDomElement& xml);
    virtual ~Album();

    std::string getLabel() const;
    void setLabel(const std::string &l);

    void addTraccia(const Traccia &tr);
    void removeTraccia(const std::string &nomeTraccia);
    const std::vector<Traccia>& getTracce() const;
    Durata getDurataTotale() const;

    virtual void printInfo() const final;

    QJsonObject toJson() const final;
    QDomElement toXml(QDomDocument& doc) const final;

    virtual void accept(VisitorGUI* v) const final;

    // OVERLOADING OPERATORI
    friend bool operator==(const Album& a, const Album& b);
    friend bool operator!=(const Album& a, const Album& b);
};

#endif
