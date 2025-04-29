#include <iostream>
#include "Artista.h"
#include "../../view/VisitorGUI.h"

unsigned int Artista::nextArtistId = 0;
std::mutex Artista::idMutex;

//COSTRUTTORE STD
Artista::Artista(const std::string &n) : ID(generateId()), nome(n) {}
Artista::Artista(const std::string &n, const std::string &g) : ID(generateId()), nome(n), genere(g) {}
Artista::Artista(const std::string &n, const std::string &g, const std::string &i) : ID(generateId()), nome(n), genere(g), info(i) {}
Artista::Artista(const std::string &n, const std::string &g, const std::string &i, const std::string &ip) : ID(generateId()), nome(n), genere(g), info(i), imagePath(ip) {}


//DISTRUTTORE
Artista::~Artista() {}

unsigned int Artista::generateId() {
    std::lock_guard<std::mutex> lock(idMutex);
    return nextArtistId++;
}

unsigned int Artista::getId() const {
    return ID;
}

std::string Artista::getNome() const { 
    return nome; 
}
void Artista::setNome(const std::string &n) { 
    nome = n; 
}

std::string Artista::getGenere() const { 
    return genere; 
}
void Artista::setGenere(const std::string &g) { 
    genere = g; 
}

std::string Artista::getInfo() const { 
    return info; 
}

void Artista::setInfo(const std::string &i) { 
    info = i; 
}

std::string Artista::getImagePath() const {
    return imagePath;
}

void Artista::setImagePath(const std::string &path) {
    imagePath = path;
}

void Artista::addProduct(ArtistProduct* p) {
    if (!p) {
        assert(false && "ArtistProduct pointer must not be nullptr");
        throw std::invalid_argument("Artista addProduct() received nullptr ArtistProduct pointer.");
    }
    products[p->getId()] = p;
}

void Artista::removeProduct(unsigned int id_product) {
    products.erase(id_product);
}

const std::unordered_map<unsigned int, ArtistProduct*>& Artista::getProducts() const {
    return products;
}

void Artista::printInfo() const {
    std::cout << "Artista ID: " << ID << std::endl;
    std::cout << "Nome: " << nome << std::endl;
    std::cout << "Genere: " << genere << std::endl;
    std::cout << "Info: " << info << std::endl;
    std::cout << "Image Path: " << imagePath << std::endl;

    std::cout << "Prodotti: " << std::endl;
    for(auto& pair : products) {
        std::cout << "------------------------" << std::endl;
        std::cout << "Prodotto ID: " << pair.first << std::endl;
        pair.second->printInfo();
        std::cout << "------------------------" << std::endl;
    }
}


// JSON
// Costruttore: carica SOLO le info base
Artista::Artista(const QJsonObject& json)
: ID(generateId()),
  nome(json["nome"].toString().toStdString()),
  genere(json["genere"].toString().toStdString()),
  info(json["info"].toString().toStdString()),
  imagePath(json["imagePath"].toString().toStdString())
{}

// Metodo statico di Factory
Artista* Artista::createFromJson(const QJsonObject& json) {
    Artista* artista = new Artista(json);
    artista->loadProductsFromJson(json["prodotti"].toArray());
    return artista;
}

// Carica i prodotti separatamente
void Artista::loadProductsFromJson(const QJsonArray& prodottiArray) {
    for (const QJsonValue& v : prodottiArray) {
        ArtistProduct* prodotto = ArtistProduct::createJson(this, v.toObject());
        if (prodotto) {
            addProduct(prodotto);
        } else {
            assert(false && "[Artista JSON] Creazione prodotto fallita");
            throw std::runtime_error("[Artista JSON] Creazione prodotto fallita.");
        }
    }
}

// Metodo di salvataggio JSON
QJsonObject Artista::toJson() const {
    QJsonObject json;
    json["type"] = "artista";
    json["nome"] = QString::fromStdString(nome);
    json["genere"] = QString::fromStdString(genere);
    json["info"] = QString::fromStdString(info);
    json["imagePath"] = QString::fromStdString(imagePath);
    
    QJsonArray array_prodotti;
    for (const auto& pair : products) {
        array_prodotti.append(pair.second->toJson());
    }
    json["prodotti"] = array_prodotti;
    return json;
}


// XML
// Costruttore: carica SOLO le info base
Artista::Artista(const QDomElement& el)
: ID(generateId()),
  nome(el.attribute("nome").toStdString()),
  genere(el.attribute("genere").toStdString()),
  imagePath(el.attribute("imagePath").toStdString())
{
    info = el.firstChildElement("info").text().toStdString();
}

// Metodo statico di Factory
Artista* Artista::createFromXml(const QDomElement& el) {
    Artista* artista = new Artista(el);
    QDomElement prodottiElem = el.firstChildElement("prodotti");
    if (!prodottiElem.isNull()) {
        artista->loadProductsFromXml(prodottiElem);
    }
    return artista;
}

// Carica i prodotti separatamente
void Artista::loadProductsFromXml(const QDomElement& prodottiElem) {
    QDomNodeList nodiProdotti = prodottiElem.childNodes();
    for (int i = 0; i < nodiProdotti.count(); ++i) {
        QDomNode nodo = nodiProdotti.at(i);
        if (!nodo.isElement()) continue;

        QDomElement prodottoEl = nodo.toElement();
        ArtistProduct* prodotto = ArtistProduct::createXml(this, prodottoEl);

        if (prodotto) {
            addProduct(prodotto);
        } else {
            assert(false && "[Artista XML] Tag non riconosciuto o parsing fallito");
            throw std::runtime_error("[Artista XML] Tag non riconosciuto o parsing fallito: " + prodottoEl.tagName().toStdString());
        }
    }
}

// Metodo di salvataggio XML
QDomElement Artista::toXml(QDomDocument& doc) const {
    QDomElement artistaElem = doc.createElement("Artista");
    artistaElem.setAttribute("nome", QString::fromStdString(nome));
    artistaElem.setAttribute("genere", QString::fromStdString(genere));
    artistaElem.setAttribute("imagePath", QString::fromStdString(imagePath));

    QDomElement infoElem = doc.createElement("info");
    infoElem.appendChild(doc.createTextNode(QString::fromStdString(info)));
    artistaElem.appendChild(infoElem);

    QDomElement prodottiElem = doc.createElement("prodotti");
    for (const auto& pair : products) { // attenzione: stiamo iterando la mappa products
        QDomElement prodottoElem = pair.second->toXml(doc);
        prodottiElem.appendChild(prodottoElem);
    }
    artistaElem.appendChild(prodottiElem);

    return artistaElem;
}


void Artista::accept(VisitorGUI* visitor) const{
    visitor -> visit(this);
}


// OVERLOADING OPERATORI
bool operator==(const Artista& a, const Artista& b) {
    if (a.nome != b.nome || a.genere != b.genere || a.info != b.info || a.imagePath != b.imagePath) return false;
    if (a.products.size() != b.products.size()) return false;

    for (const auto& pair : a.products) {
        auto it = b.products.find(pair.first);
        if (it == b.products.end()) return false;
        if (*(pair.second) != *(it->second)) return false;
    }

    return true;
}

bool operator!=(const Artista& a, const Artista& b) {
    return !(a == b);
}