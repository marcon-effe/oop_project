#ifndef TOUR_H
#define TOUR_H

#include <vector>

#include "../core/NotMusica.h"
#include "DataTour.h"

class Tour : public NotMusica {
private:
    std::vector<DataTour> dateTour;
public:
    Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita);
    Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::vector<DataTour>& dateTour);
    Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& img);
    Tour(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& img, const std::vector<DataTour>& dateTour);

    Tour(NotMusica* n, const std::vector<DataTour>& dateTour);
    Tour(const Tour* t);
    Tour(Artista* owner, const QJsonObject& json);
    Tour(Artista* owner, const QDomElement& xml);
    virtual ~Tour();

    void addDataTour(const DataTour& dt);
    void removeDataTour(unsigned int index);

    const std::vector<DataTour>& getDateTour() const;
    std::vector<DataTour>& getDateTourModificabile();
    void clearDateTour();
    void setDateTour(const std::vector<DataTour>& dt);

    virtual void printInfo() const override;
    QJsonObject toJson(bool reduced=false) const override;
    QDomElement toXml(QDomDocument& doc, bool reduced=false) const override;

    virtual void accept(VisitorInterfaceConst* v) const final;
    virtual void accept(VisitorInterfaceNotConst* visitor) final;

    friend bool operator==(const Tour& a, const Tour& b);
    friend bool operator!=(const Tour& a, const Tour& b);
};

#endif