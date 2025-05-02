#ifndef TOUR_H
#define TOUR_H

#include <vector>

#include "../core/NotMusica.h"
#include "DataTour.h"
#include "../../include/data_format.h"

class Tour : public NotMusica {
private:
    std::vector<DataTour> dateTour;
public:
    Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita);
    Tour(NotMusica* n, const std::vector<DataTour>& dateTour);
    Tour(const Tour* t);
    Tour(Artista* owner, const QJsonObject& json);
    Tour(Artista* owner, const QDomElement& xml);
    virtual ~Tour();

    void addDataTour(const DataTour& dt);
    void removeDataTour(unsigned int index);

    const std::vector<DataTour>& getDateTour() const;
    std::vector<DataTour>& getDateTourModificabile();

    virtual void printInfo() const override;
    QJsonObject toJson() const override;
    QDomElement toXml(QDomDocument& doc) const override;

    virtual void accept(VisitorGUI* v) const final;
    virtual void accept(VisitorConsoleEditor* visitor) final;

    // OVERLOADING OPERATORI
    friend bool operator==(const Tour& a, const Tour& b);
    friend bool operator!=(const Tour& a, const Tour& b);
};

#endif