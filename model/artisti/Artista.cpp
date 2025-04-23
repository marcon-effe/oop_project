#include <iostream>
#include "Artista.h"
#include "../../view/VisitorGUI.h"

//COSTRUTTORE STD
Artista::Artista(const std::string &n) : nome(n) {}
Artista::Artista(const std::string &n, const std::string &g) : nome(n), genere(g) {}
Artista::Artista(const std::string &n, const std::string &g, const std::string &i) : nome(n), genere(g), info(i) {}

//DISTRUTTORE
Artista::~Artista() {}

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
    prodotti.push_back(p);
}
void Artista::removeProduct(ArtistProduct* p) {
    for (size_t i = 0; i < prodotti.size(); ++i) {
        if (prodotti[i] == p) {
            prodotti.erase(prodotti.begin() + i);
            break;
        }
    }
}
const std::vector<ArtistProduct*>& Artista::getProducts() const {
    return prodotti;
}


//JSON
// Converte un oggetto JSON in un oggetto Artista
Artista::Artista(const QJsonObject &json) {
    nome = json["nome"].toString().toStdString();
    genere = json["genere"].toString().toStdString();
    info = json["info"].toString().toStdString();
    imagePath = json["imagePath"].toString().toStdString();
    QJsonArray array_prodotti = json["prodotti"].toArray();
    for (const QJsonValue &v : array_prodotti) {
        ArtistProduct *p = ArtistProduct::createJson(v.toObject());
        if (p) prodotti.push_back(p);
    }
}
// Converte l'oggetto Artista in un oggetto JSON
QJsonObject Artista::toJson() const{
    QJsonObject json;
    json["type"] = "artista";
    json["nome"] = QString::fromStdString(nome);
    json["genere"] = QString::fromStdString(genere);
    json["info"] = QString::fromStdString(info);
    json["imagePath"] = QString::fromStdString(imagePath);
    QJsonArray array_prodotti;
    for(ArtistProduct *p : prodotti)
        array_prodotti.append(p -> toJson());
    json["prodotti"] = array_prodotti;
    return json;
}

//XML
// Converte un oggetto XML in un oggetto Artista
Artista::Artista(const QDomElement& el) {
    nome = el.attribute("nome").toStdString();
    genere = el.attribute("genere").toStdString();
    imagePath = el.attribute("imagePath").toStdString();
    
    info = el.firstChildElement("info").text().toStdString();

    QDomElement prodottiElem = el.firstChildElement("prodotti");
    if (!prodottiElem.isNull()) {
        QDomNodeList nodiProdotti = prodottiElem.childNodes();
        for (int i = 0; i < nodiProdotti.count(); ++i) {
            QDomNode nodo = nodiProdotti.at(i);
            if (!nodo.isElement()) continue;

            QDomElement prodottoEl = nodo.toElement();
            ArtistProduct* prodotto = ArtistProduct::createXml(prodottoEl);

            if (prodotto)
                prodotti.push_back(prodotto);
            else
                qWarning() << "[Artista XML] Tag non riconosciuto o parsing fallito:" << prodottoEl.tagName();
        }
    }
}
// Converte l'oggetto Artista in un oggetto XML
QDomElement Artista::toXml(QDomDocument& doc) const {
    QDomElement artistaElem = doc.createElement("Artista");
    artistaElem.setAttribute("nome", QString::fromStdString(nome));
    artistaElem.setAttribute("genere", QString::fromStdString(genere));
    artistaElem.setAttribute("imagePath", QString::fromStdString(imagePath));

    QDomElement infoElem = doc.createElement("info");
    infoElem.appendChild(doc.createTextNode(QString::fromStdString(info)));
    artistaElem.appendChild(infoElem);


    QDomElement prodottiElem = doc.createElement("prodotti");
    for (const auto& p : prodotti) {
        QDomElement prodottoElem = p->toXml(doc);
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
    if (a.nome != b.nome || a.genere != b.genere || a.info != b.info || a.imagePath == b.imagePath) return false;
    if (a.prodotti.size() != b.prodotti.size()) return false;

    for (size_t i = 0; i < a.prodotti.size(); ++i) {
        const ArtistProduct* pa = a.prodotti[i];
        const ArtistProduct* pb = b.prodotti[i];

        if (pa == nullptr || pb == nullptr) {
            if (pa != pb) return false; // uno è null, l'altro no
        } else if (*pa != *pb) {
            return false;
        }
    }

    return true;
}

bool operator!=(const Artista& a, const Artista& b) {
    return !(a == b);
}