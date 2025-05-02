#ifndef DATATOUR_H
#define DATATOUR_H

#include <string>

#include "../util/Data.h"
#include "../util/Orario.h"
#include "../../include/data_format.h"

class VisitorConsoleEditor;

class DataTour : public Data, public Orario {
private:
    std::string luogo;
    
public:
    DataTour(unsigned int g, unsigned int m, unsigned int a, unsigned int o, unsigned int min, unsigned int s, const std::string &luogo);
    DataTour(const QJsonObject& json);
    DataTour(const QDomElement& xml);
    virtual ~DataTour();

    std::string getLuogo() const;
    void setLuogo(const std::string &l);

    Data getData() const;
    void setData(const Data &d);

    Orario getOrario() const;
    void setOrario(const Orario &o);

    std::string toString() const;

    QJsonObject toJson() const;
    QDomElement toXml(QDomDocument& doc) const;

    void accept(VisitorConsoleEditor* visitor);

    // OVERLOADING OPERATORI
    friend bool operator==(const DataTour& a, const DataTour& b);
    friend bool operator!=(const DataTour& a, const DataTour& b);
};

#endif