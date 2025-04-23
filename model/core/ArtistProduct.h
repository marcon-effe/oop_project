#ifndef ARTISTPRODUCT_H
#define ARTISTPRODUCT_H


#include <string>

#include "../../include/data_format.h"

class VisitorGUI;


class ArtistProduct {
protected:
    std::string title;
    std::string description;
    std::string imagePath;
public:
    ArtistProduct(const std::string &t, const std::string &desc);
    ArtistProduct(const std::string &t, const std::string &d, const std::string &i);
    ArtistProduct(const ArtistProduct* p);
    ArtistProduct(const QJsonObject &json);
    ArtistProduct(const QDomElement& xml);
    virtual ~ArtistProduct();

    std::string getTitle() const;
    void setTitle(const std::string &t);

    std::string getDescription() const;
    void setDescription(const std::string &d);

    std::string getImagePath() const;
    void setImagePath(const std::string &path);

    virtual void printInfo() const;

    virtual QJsonObject toJson() const;
    virtual QDomElement toXml(QDomDocument& doc) const;
    
    static ArtistProduct* createJson(const QJsonObject& json);
    static ArtistProduct* createXml(const QDomElement& xml);

    virtual void accept(VisitorGUI* visitor) const = 0;

    //OVERLOADING OPERATORI
    friend bool operator==(const ArtistProduct& a, const ArtistProduct& b);
    friend bool operator!=(const ArtistProduct& a, const ArtistProduct& b);
};

#endif
