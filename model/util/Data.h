#ifndef DATA_H 
#define DATA_H

#include <string>
#include <QDate>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomElement>
#include <QDomDocument>

class Data {
    private: 
        unsigned int giorno; 
        unsigned int mese; 
        unsigned int anno; 
    public: 
        Data() = default;
        Data(unsigned int g, unsigned int m, unsigned int a);
        Data(const QJsonObject& json);
        Data(const QDomElement& xml);
        virtual ~Data();

        unsigned int getGiorno() const; 
        void setGiorno(unsigned int g);

        unsigned int getMese() const; 
        void setMese(unsigned int m);
        
        unsigned int getAnno() const; 
        void setAnno(unsigned int a);

        std::string toString() const;
        QDate toQDate() const;

        QJsonObject toJson() const;
        QDomElement toXml(QDomDocument& doc) const;

        

        //OVERLOADING OPERATORI
        friend bool operator==(const Data& a, const Data& b);
        friend bool operator!=(const Data& a, const Data& b);
        friend bool operator<(const Data& a, const Data& b);
        friend bool operator>(const Data& a, const Data& b);
};

#endif