#ifndef ARTISTA_H
#define ARTISTA_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <cassert>
#include <stdexcept>

#include <QJsonObject>
#include <QJsonArray>
#include <QDomElement>
#include <QDomDocument>

#include "../core/ArtistProduct.h"

class ArtistProduct;
class VisitorInterfaceConst;
class VisitorInterfaceNotConst;

class Artista {
private:
    const unsigned int ID;
    static unsigned int nextArtistId;
    static std::mutex idMutex;

    std::string nome;
    std::string genere;
    std::string info;
    std::string imagePath;
    std::string imageB64;

    std::unordered_map<unsigned int, ArtistProduct*> products;

    static unsigned int generateId();
    std::string getImageB64() const;
    void setImageB64(const std::string& b64);

public:
    //COSTRUTTORE STD
    Artista(const std::string &n);
    Artista(const std::string &n, const std::string &g);
    Artista(const std::string &n, const std::string &g, const std::string &i);
    Artista(const std::string &n, const std::string &g, const std::string &i, const std::string &ip);

    //DISTRUTTORE
    ~Artista();

    unsigned int getId() const;

    std::string getNome() const;
    void setNome(const std::string &n);

    std::string getGenere() const;
    void setGenere(const std::string &g);

    std::string getInfo() const;
    void setInfo(const std::string &i);

    std::string getImagePath() const;
    void setImagePath(const std::string &ip);

    void addProduct(ArtistProduct* p);
    void removeProduct(unsigned int id_product);
    const std::unordered_map<unsigned int, ArtistProduct*>& getProducts() const;

    void printInfo() const;

    // JSON
    Artista(const QJsonObject &json); // Costruttore: carica SOLO le info base
    // Metodo statico di Factory
    static Artista* createFromJson(const QJsonObject& json);
    void loadProductsFromJson(const QJsonArray& prodottiArray);
    QJsonObject toJson(bool reduced=false) const;

    //XML
    Artista(const QDomElement &xml); // Costruttore: carica SOLO le info base
    // Metodo statico di Factory
    static Artista* createFromXml(const QDomElement& xml);
    void loadProductsFromXml(const QDomElement& prodottiElement);
    QDomElement toXml(QDomDocument& doc, bool reduced=false) const;

    //Visitor
    void accept(VisitorInterfaceConst* visitor) const;
    void accept(VisitorInterfaceNotConst* visitor);

    //OVERLOADING OPERATORI
    friend bool operator==(const Artista& a, const Artista& b);
    friend bool operator!=(const Artista& a, const Artista& b);
};

#endif