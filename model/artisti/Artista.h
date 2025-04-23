#ifndef ARTISTA_H
#define ARTISTA_H

#include <string>
#include <vector>

#include "../core/ArtistProduct.h"
#include "../../include/data_format.h"

class ArtistProduct;
class VisitorGUI;

class Artista {
private:
    std::string nome;
    std::string genere;
    std::string info;
    std::string imagePath;
    std::vector<ArtistProduct*> prodotti;
public:
    //COSTRUTTORE STD
    Artista(const std::string &n);
    Artista(const std::string &n, const std::string &g);
    Artista(const std::string &n, const std::string &g, const std::string &i);
    Artista(const std::string &n, const std::string &g, const std::string &i, const std::string &ip);

    //COSTRUTTORE DA QJSONOBJECT
    Artista(const QJsonObject &json);
    //COSTRUTTORE DA QDOMELEMENT
    Artista(const QDomElement &xml);

    //DISTRUTTORE
    ~Artista();

    std::string getNome() const;
    void setNome(const std::string &n);

    std::string getGenere() const;
    void setGenere(const std::string &g);

    std::string getInfo() const;
    void setInfo(const std::string &i);

    std::string getImagePath() const;
    void setImagePath(const std::string &ip);

    void addProduct(ArtistProduct* p);
    void removeProduct(ArtistProduct* p);
    const std::vector<ArtistProduct*>& getProducts() const;

    QJsonObject toJson() const;
    QDomElement toXml(QDomDocument& doc) const;

    void accept(VisitorGUI *visitor) const;

    //OVERLOADING OPERATORI
    friend bool operator==(const Artista& a, const Artista& b);
    friend bool operator!=(const Artista& a, const Artista& b);
};

#endif