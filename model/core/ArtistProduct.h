#ifndef ARTISTPRODUCT_H
#define ARTISTPRODUCT_H


#include <string>
#include <mutex>
#include <cassert>
#include <stdexcept>

#include <QJsonObject>
#include <QJsonArray>
#include <QDomElement>
#include <QDomDocument>


class VisitorGUI;
class VisitorConsoleEditor;
class Artista;

class ArtistProduct {
private:
    const unsigned int ID;
    unsigned int id_artist; //volutamente non const, in quanto l'artista potrebbe essere cambiato
    std::string nomeArtista; // nome dell'artista, usato per il path dell'immagine
    static unsigned int nextProductId;
    static std::mutex idMutex;

    std::string title;
    std::string description;
    std::string imagePath;
    std::string imageB64;

    static unsigned int generateId();

public:
    // ho modificato i costruttori perché SIA NECESSARIO passare un puntatore ad Artista per la certezza che id_artist sia sempre valido
    ArtistProduct(Artista* owner, const std::string& t, const std::string& desc);
    ArtistProduct(Artista* owner, const std::string& t, const std::string& d, const std::string& i);
    ArtistProduct(const ArtistProduct* p);
    // Costruttori di caricamento
    ArtistProduct(Artista* owner, const QJsonObject& json);
    ArtistProduct(Artista* owner, const QDomElement& el);
    virtual ~ArtistProduct();

    unsigned int getId() const;
    unsigned int getArtistId() const;
    void setOwner(Artista* owner);
    std::string getNomeArtista() const;
    
    
    std::string getTitle() const;
    void setTitle(const std::string &t);

    std::string getDescription() const;
    void setDescription(const std::string &d);

    std::string getImagePath() const;
    void setImagePath(const std::string &path);
    std::string getImageB64() const;
    void setImageB64(const std::string &b64);

    virtual void printInfo() const;

    virtual QJsonObject toJson(bool reduced=false) const;
    virtual QDomElement toXml(QDomDocument& doc, bool reduced=false) const;
    
    static ArtistProduct* createJson(Artista* owner, const QJsonObject& json);
    static ArtistProduct* createXml(Artista* owner, const QDomElement& xml);

    virtual void accept(VisitorGUI* visitor) const = 0;
    virtual void accept(VisitorConsoleEditor* visitor) = 0;

    //OVERLOADING OPERATORI
    friend bool operator==(const ArtistProduct& a, const ArtistProduct& b);
    friend bool operator!=(const ArtistProduct& a, const ArtistProduct& b);
};

#endif
