#ifndef TOUR_H
#define TOUR_H

#include <vector>

#include "../core/NotMusica.h"
#include "DataTour.h"


#include "../../include/data_format.h"
#include "../../view/VisitorGUI.h"

class Tour : public NotMusica {
private:
    std::vector<DataTour> dateTour;
public:
    Tour(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita);
    Tour(NotMusica* n, const std::vector<DataTour>& dateTour);
    Tour(const Tour* t);
    Tour(const QJsonObject& json);
    Tour(const QDomElement& xml);
    virtual ~Tour();

    void addDataTour(const DataTour &dt);
    void removeDataTour(unsigned int index);
    const std::vector<DataTour>& getDateTour() const;

    virtual void printInfo() const final;
    QJsonObject toJson() const final;
    QDomElement toXml(QDomDocument& doc) const final;

    virtual void accept(VisitorGUI* v) const final;

    // OVERLOADING OPERATORI
    friend bool operator==(const Tour& a, const Tour& b);
    friend bool operator!=(const Tour& a, const Tour& b);
};

#endif