#include <iostream>
#include "Artista.h"
#include "../../view/VisitorGUI.h"
#include "../../cli/VisitorConsoleEditor.h"
#include "../../view/ErrorManager.h"
#include "../../data/DataManager.h"

unsigned int Artista::nextArtistId = 1;
std::mutex Artista::idMutex;

//COSTRUTTORE STD
Artista::Artista(const std::string &n)
    : ID(generateId()), nome(n) {}

Artista::Artista(const std::string &n, const std::string &g)
    : ID(generateId()), nome(n), genere(g) {}

Artista::Artista(const std::string &n, const std::string &g, const std::string &i)
    : ID(generateId()), nome(n), genere(g), info(i) {}

Artista::Artista(const std::string &n, const std::string &g, const std::string &i, const std::string &ip)
    : ID(generateId()), nome(n), genere(g), info(i)
{
    setImagePath(ip);
}

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

void Artista::setNome(const std::string& n) {
    if (n == nome) return;

    // Path sanitizzati
    QString oldSanitized = QString::fromStdString(DataManager::sanitizeForPath(nome));
    QString newSanitized = QString::fromStdString(DataManager::sanitizeForPath(n));

    QString oldDir = "view/icons/" + oldSanitized;
    QString newDir = "view/icons/" + newSanitized;

    // Controlla conflitto di directory
    if (QDir(newDir).exists()) {
        std::string errorMessage = "Impossibile rinominare artista: esiste già una directory con il nome \"" + n + "\" (o con lo stesso path dopo sanitizzazione).";
        ErrorManager::showError(errorMessage);
        ErrorManager::logError(errorMessage);
        return;
    }

    // Rinomina fisicamente la directory se esiste
    if (QDir(oldDir).exists()) {
        QDir().mkpath(QFileInfo(newDir).path());
        if (!QDir().rename(oldDir, newDir)) {
            std::string errorMessage = "Rinomina cartella fallita: impossibile spostare \"icons/" + oldSanitized.toStdString() + "\" in \"icons/" + newSanitized.toStdString() + "\".";
            ErrorManager::showError(errorMessage);
            ErrorManager::logError(errorMessage);
            return;
        }
    }

    // Aggiorna imagePath se presente
    if (!imagePath.empty()) {
        QFileInfo info(QString::fromStdString(imagePath));
        QString ext = info.suffix();
        QString newImagePath = "view/icons/" + newSanitized + "/profilo." + ext;
        imagePath = newImagePath.toStdString();
    }

    // Aggiorna il nome reale (non sanitizzato)
    nome = n;

    for (auto& p : products) {
        if (p.second) {
            p.second->setOwner(this);
        }
    }
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

void Artista::setImagePath(const std::string& userSelectedPath) {
    if(imagePath == userSelectedPath) {
        return;
    }
    
    if(userSelectedPath.empty()) {
        imagePath = "";
        imageB64 = "";
        return;
    }
    
    QFile file(QString::fromStdString(userSelectedPath));
    if (!file.exists()) {
        imagePath = "";
        imageB64 = "";
        return;
    }

    QString extension = QFileInfo(file).suffix().toLower();
    QString sanitizedName = QString::fromStdString(DataManager::sanitizeForPath(nome));
    QString folderPath = "view/icons/" + sanitizedName;
    QString fileName = "profilo." + extension;
    QString localPath = folderPath + "/" + fileName;

    // Crea la cartella se non esiste
    QDir().mkpath(folderPath);

    // Sovrascrive file locale
    QFile::remove(localPath);
    if (!file.copy(localPath)) {
        ErrorManager::showError("Errore durante la copia dell'immagine in " + localPath.toStdString());
        imagePath = "";
        imageB64 = "";
        return;
    }

    QFile saved(localPath);
    if (saved.open(QIODevice::ReadOnly)) {
        QByteArray data = saved.readAll();
        saved.close();
        imageB64 = data.toBase64().toStdString();
        imagePath = localPath.toStdString();
    } else {
        ErrorManager::showError("Errore durante la lettura dell'immagine copiata.");
        imagePath = "";
        imageB64 = "";
    }
}

std::string Artista::getImageB64() const {
    return imageB64;
}

void Artista::setImageB64(const std::string& b64) {
    imageB64 = b64;
}

void Artista::addProduct(ArtistProduct* p) {
    if (!p) {
        assert(false && "ArtistProduct pointer must not be nullptr");
        throw std::invalid_argument("Artista addProduct() received nullptr ArtistProduct pointer.");
    }

    // Spostamento immagine se necessario
    std::string imagePath = p->getImagePath();
    if (!imagePath.empty()) {
        QString currentPath = QString::fromStdString(imagePath);
        QFileInfo info(currentPath);

        if (info.exists()) {
            QString ext = info.suffix().toLower();
            QString sanitizedArtist = QString::fromStdString(DataManager::sanitizeForPath(nome));
            QString sanitizedTitle = QString::fromStdString(DataManager::sanitizeForPath(p->getTitle()));

            QString targetDir = "view/icons/" + sanitizedArtist;
            QString newLocalPath = targetDir + "/" + sanitizedTitle + "." + ext;

            if (currentPath != newLocalPath) {
                QDir().mkpath(targetDir);
                QFile::remove(newLocalPath);
                QFile::rename(currentPath, newLocalPath);

                // Aggiorna percorso e base64
                QFile copied(newLocalPath);
                if (copied.open(QIODevice::ReadOnly)) {
                    QByteArray data = copied.readAll();
                    copied.close();
                    p->setImageB64(data.toBase64().toStdString());
                    p->setImagePath(newLocalPath.toStdString());
                } else {
                    ErrorManager::showError("Errore durante la lettura dell'immagine spostata.");
                    p->setImagePath("");
                    p->setImageB64("");
                }
            }
        }
    }

    products[p->getId()] = p;
}

void Artista::removeProduct(unsigned int id_product) {
    auto it = products.find(id_product);
    if (it != products.end()) {
        ArtistProduct* prodotto = it->second;
        if (prodotto) {
            // Elimina immagine associata
            std::string path = prodotto->getImagePath();
            if (!path.empty() && !QString::fromStdString(path).startsWith(":/")) {
                QFile file(QString::fromStdString(path));
                if (file.exists()) {
                    file.remove();
                }
            }

            delete prodotto;
        }
        products.erase(it);
    }
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


static std::string restoreImageFromB64(const std::string& artistName, const std::string& imageB64, const std::string& originalExt) {
    if (imageB64.empty()) return "";

    std::string sanitized = DataManager::sanitizeForPath(artistName);
    QString folder = "view/icons/" + QString::fromStdString(sanitized);
    QString ext = QString::fromStdString(originalExt).toLower();
    QString fileName = "profilo." + ext;
    QString localPath = folder + "/" + fileName;

    // Assicura che la directory esista
    QDir().mkpath(folder);

    // Rimuove eventuali file "profilo.*" preesistenti
    QDir dir(folder);
    QStringList existing = dir.entryList(QStringList() << "profilo.*", QDir::Files);
    for (const QString& f : existing) {
        dir.remove(f);
    }

    // Scrive il nuovo file decodificato
    QFile out(localPath);
    if (out.open(QIODevice::WriteOnly)) {
        out.write(QByteArray::fromBase64(QString::fromStdString(imageB64).toUtf8()));
        out.close();
        return localPath.toStdString();  // restituisce path REALE su disco
    }

    ErrorManager::logError("Impossibile scrivere immagine base64 per artista: " + artistName);
    return "";
}

// JSON
// Costruttore: carica SOLO le info base
Artista::Artista(const QJsonObject& json)
    : ID(generateId()),
      nome(json["nome"].toString().toStdString()),
      genere(json["genere"].toString().toStdString()),
      info(json["info"].toString().toStdString())
{
    imageB64 = json["imageB64"].toString().toStdString();
    imagePath = json["imagePath"].toString().toStdString();

    // Estensione dal path
    QString ext = QFileInfo(QString::fromStdString(imagePath)).suffix();
    imagePath = restoreImageFromB64(nome, imageB64, ext.toStdString());
}

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
QJsonObject Artista::toJson(bool reduced) const {
    QJsonObject json;
    json["type"] = "artista";
    json["nome"] = QString::fromStdString(nome);
    json["genere"] = QString::fromStdString(genere);
    json["info"] = QString::fromStdString(info);
    json["imagePath"] = QString::fromStdString(imagePath);
    if(!reduced)
        json["imageB64"] = QString::fromStdString(imageB64);

    QJsonArray array_prodotti;
    for (const auto& pair : products)
        array_prodotti.append(pair.second->toJson(reduced));

    json["prodotti"] = array_prodotti;
    return json;
}


// XML
// Costruttore: carica SOLO le info base
Artista::Artista(const QDomElement& el)
    : ID(generateId()),
      nome(el.attribute("nome").toStdString()),
      genere(el.attribute("genere").toStdString())
{
    info = el.firstChildElement("info").text().toStdString();
    imagePath = el.attribute("imagePath").toStdString();

    QDomElement imageElem = el.firstChildElement("imageB64");
    imageB64 = imageElem.isNull() ? "" : imageElem.text().toStdString();

    QString ext = QFileInfo(QString::fromStdString(imagePath)).suffix();
    imagePath = restoreImageFromB64(nome, imageB64, ext.toStdString());
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
QDomElement Artista::toXml(QDomDocument& doc, bool reduced) const {
    QDomElement artistaElem = doc.createElement("Artista");
    artistaElem.setAttribute("nome", QString::fromStdString(nome));
    artistaElem.setAttribute("genere", QString::fromStdString(genere));
    artistaElem.setAttribute("imagePath", QString::fromStdString(imagePath));

    QDomElement infoElem = doc.createElement("info");
    infoElem.appendChild(doc.createTextNode(QString::fromStdString(info)));
    artistaElem.appendChild(infoElem);

    if (!imageB64.empty() && !reduced) {
        QDomElement imageElem = doc.createElement("imageB64");
        imageElem.appendChild(doc.createTextNode(QString::fromStdString(imageB64)));
        artistaElem.appendChild(imageElem);
    }

    QDomElement prodottiElem = doc.createElement("prodotti");
    for (const auto& pair : products)
        prodottiElem.appendChild(pair.second->toXml(doc, reduced));

    artistaElem.appendChild(prodottiElem);
    return artistaElem;
}

// VISITOR
void Artista::accept(VisitorGUI* visitor) const{
    visitor -> visit(this);
}

void Artista::accept(VisitorConsoleEditor* visitor) {
    visitor->visit(this);
}


// OVERLOADING OPERATORI
bool operator==(const Artista& a, const Artista& b) {
    if (a.nome != b.nome ||
        a.genere != b.genere ||
        a.info != b.info ||
        a.imageB64 != b.imageB64)
        return false;

    if (a.products.size() != b.products.size())
        return false;

    for (const auto& pair : a.products) {
        auto it = b.products.find(pair.first);
        if (it == b.products.end())
            return false;
        if (*(pair.second) != *(it->second))
            return false;
    }

    return true;
}

bool operator!=(const Artista& a, const Artista& b) {
    return !(a == b);
}