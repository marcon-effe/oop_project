#ifndef TRACCIA_H
#define TRACCIA_H

#include <string>
#include <vector>

#include <QJsonObject>
#include <QJsonArray>
#include <QDomElement>
#include <QDomDocument>

#include "../util/Durata.h"

class VisitorConsoleEditor;

class Traccia {
private:
    std::string nome;
    std::vector<std::string> partecipanti;  // Ora contiene solo i nomi
    Durata durata;
    std::string testo;
    bool hasTesto;
public:
    Traccia(const std::string &n,const std::vector<std::string> &parts, const Durata &d, const std::string &t, bool ht);
    Traccia(const QJsonObject& json);
    Traccia(const QDomElement& xml);
    ~Traccia();

    std::string getNome() const;
    void setNome(const std::string &n);

    Durata getDurata() const;
    void setDurata(const Durata &d);

    std::string getTesto() const;
    void setTesto(const std::string &t);

    bool hasTestoPresent() const;
    void setHasTesto(bool ht);

    const std::vector<std::string>& getPartecipanti() const;
    void addPartecipante(const std::string &a);
    void removePartecipante(const std::string &a);
    void removePartecipante(unsigned int index);
    void editPartecipante(unsigned int index, const std::string& nuovoNome);

    QJsonObject toJson() const;
    QDomElement toXml(QDomDocument& doc) const;

    //Visitor
    void accept(VisitorConsoleEditor* visitor);

    // OVERLOADING OPERATORI
    friend bool operator==(const Traccia& a, const Traccia& b);
    friend bool operator!=(const Traccia& a, const Traccia& b);
};

#endif 
