#include <iostream>
#include <unordered_map>
#include <cassert>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QResource>

#include "model/artisti/Artista.h"
#include "model/musica/Album.h"
#include "model/musica/Singolo.h"
#include "model/merch/CD.h"
#include "model/merch/Vinile.h"
#include "model/merch/TShirt.h"
#include "model/tour/Tour.h"
#include "model/musica/Traccia.h"
#include "model/tour/DataTour.h"
#include "include/dataManager.h"

#include "view/MainWindow.h"
#include "view/ErrorManager.h"

void assertDeepEquality(const std::unordered_map<unsigned int, Artista*>& original, const std::unordered_map<unsigned int, Artista*>& loaded) {
    assert(original.size() == loaded.size());

    auto it_orig = original.begin();
    auto it_load = loaded.begin();

    while (it_orig != original.end() && it_load != loaded.end()) {
        const Artista* o = it_orig->second;
        const Artista* l = it_load->second;

        std::cout << "▶ Confronto artista: " << o->getNome() << " vs " << l->getNome() << std::endl;

        if (!(o->getNome() == l->getNome() &&
              o->getGenere() == l->getGenere() &&
              o->getInfo() == l->getInfo() &&
              o->getImagePath() == l->getImagePath())) {
            std::cerr << "❌ Dati artista diversi!" << std::endl;
            assert(false);
        }

        const auto& originalProd = o->getProducts();
        const auto& loadedProd = l->getProducts();

        if (originalProd.size() != loadedProd.size()) {
            std::cerr << "❌ Numero prodotti diverso per artista " << o->getNome() << "\n"
                      << " - Originale: " << originalProd.size()
                      << ", Caricato:  " << loadedProd.size() << std::endl;
            assert(false);
        }

        // Confronta *contenuto* dei prodotti, NON gli ID
        auto it_op = originalProd.begin();
        auto it_lp = loadedProd.begin();
        while (it_op != originalProd.end() && it_lp != loadedProd.end()) {
            const ArtistProduct* oProd = it_op->second;
            const ArtistProduct* lProd = it_lp->second;

            if (!(*oProd == *lProd)) {
                std::cerr << "   ❌ Differenza nei dati prodotto [" << oProd->getTitle() << "]" << std::endl;
                assert(false);
            }

            if (lProd->getArtistId() != l -> getId()) {
                std::cerr << "   ❌ Prodotto non collegato correttamente al nuovo artista!" << std::endl;
                assert(false);
            }

            std::cout << "   ✅ Prodotto [" << oProd->getTitle() << "] OK" << std::endl;

            ++it_op;
            ++it_lp;
        }

        ++it_orig;
        ++it_load;
    }

    std::cout << "✅ Tutti gli artisti e i prodotti corrispondono perfettamente!" << std::endl;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(resources);
    qDebug() << "[DEBUG] Working directory:" << QDir::currentPath();

    std::unordered_map<unsigned int, Artista*> artisti;

    try {
        // Creazione manuale artisti
        {
            Artista* a = new Artista("Artista1");
            Traccia t1("Traccia1", {"Artista1"}, Durata(0, 3, 30), "Testo1", true);
            Traccia t2("Traccia2", {"Artista1"}, Durata(0, 4, 0), "Testo2", false);
            a->addProduct(new Album(a, "Album1", "desc", Data(1,1,2020), Durata(0,42,10), "Rock", {t1, t2}, "Label1"));
            a->addProduct(new Singolo(a, "Sing1", "desc", Data(2,2,2021), Durata(0,3,30), "Pop", t1, false, 1));
            a->addProduct(new CD(a, "CD1", "desc", 15.5, true, 10, "CP1", "StampeX", "CR1", "Standard", "Jewel"));
            a->addProduct(new Vinile(a, "Vin1", "desc", 20.0, true, 5, "VP1", "StampeY", "CR2", "Limited", 33, 12));
            a->addProduct(new TShirt(a, "TS1", "desc", 10.0, true, 100, "TP1", "M", "Nero"));
            Tour* tour = new Tour(a, "Tour1", "desc", 99.99, true, 200);
            tour->addDataTour(DataTour(10,5,2025,20,30,0,"Roma"));
            a->addProduct(tour);

            a -> printInfo();

            artisti[a->getId()] = a;
        }
        
        // JSON
        DataManager::saveToFileJson(artisti, "saves/json/test_artisti.json");
        auto loadedJson = DataManager::loadFromFileJson("saves/json/test_artisti.json");
        for (auto& p : loadedJson) {
            std::cout << "Artista caricato: " << std::endl;
            p.second->printInfo();
        }
        assertDeepEquality(artisti, loadedJson);

        // XML
        DataManager::saveToFileXml(artisti, "saves/xml/test_artisti.xml");
        auto loadedXml = DataManager::loadFromFileXml("saves/xml/test_artisti.xml");
        assertDeepEquality(artisti, loadedXml);

        std::cout << "✅ Tutti i test superati: uguaglianza semantica confermata!" << std::endl;

        // Deallocazione memoria
        for (auto& p : artisti) delete p.second;
        //for (auto& p : loadedJson) delete p.second;
        for (auto& p : loadedXml) delete p.second;

    } catch (const std::exception& ex) {
        ErrorManager::showError(ex.what());
        return -1;
    } catch (...) {
        ErrorManager::showError("Errore sconosciuto durante l'esecuzione.");
        return -1;
    }

    // Carica stile
    QFile styleFile(":/styles/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&styleFile);
        app.setStyleSheet(ts.readAll());
        qDebug() << "✅ style.qss applicato correttamente!";
    } else {
        qDebug() << "❌ style.qss non trovato!";
    }

    // Avvio finestra principale
    MainWindow w;
    w.show();

    return app.exec();
}