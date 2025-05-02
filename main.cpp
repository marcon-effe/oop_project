#include <iostream>
#include <unordered_map>
#include <cassert>
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QResource>
#include <QFile>
#include <QTextStream>

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
#include "cli/ConsoleApp.h"

void assertDeepEquality(const std::unordered_map<unsigned int, Artista*>& original, const std::unordered_map<unsigned int, Artista*>& loaded) {
    assert(original.size() == loaded.size());

    auto itO = original.begin();
    auto itL = loaded.begin();
    for (; itO != original.end() && itL != loaded.end(); ++itO, ++itL) {
        const Artista* o = itO->second;
        const Artista* l = itL->second;

        std::cout << "▶ Confronto artista: " << o->getNome() << " vs " << l->getNome() << std::endl;

        assert(o->getNome() == l->getNome());
        assert(o->getGenere() == l->getGenere());
        assert(o->getInfo() == l->getInfo());
        assert(o->getImagePath() == l->getImagePath());

        const auto& originalProd = o->getProducts();
        const auto& loadedProd = l->getProducts();

        if (originalProd.size() != loadedProd.size()) {
            std::cerr << "❌ Numero prodotti diverso per artista " << o->getNome() << "\n"
                      << " - Originale: " << originalProd.size()
                      << ", Caricato:  " << loadedProd.size() << std::endl;
            assert(false);
        }

        for (const auto& [_, oProd] : originalProd) {
            bool trovato = false;
            for (const auto& [__, lProd] : loadedProd) {
                if (*oProd == *lProd) {
                    trovato = true;
                    std::cout << "   ✅ Prodotto [" << oProd->getTitle() << "] OK" << std::endl;
                    break;
                }
            }
            if (!trovato) {
                std::cerr << "   ❌ Nessun prodotto caricato corrisponde a [" << oProd->getTitle() << "]" << std::endl;
                assert(false);
            }
        }
    }

    std::cout << "✅ Tutti gli artisti e i prodotti corrispondono perfettamente!" << std::endl;
}

void runTests() {
    std::unordered_map<unsigned int, Artista*> artisti;
    try {
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
        artisti[a->getId()] = a;

        std::cout << "------- Salvataggio in JSON -------" << std::endl;
        DataManager::saveToFileJson(artisti, "saves/json/test_artisti.json");
        auto loadedJson = DataManager::loadFromFileJson("saves/json/test_artisti.json");
        assertDeepEquality(artisti, loadedJson);

        std::cout << "------- Salvataggio in XML -------" << std::endl;
        DataManager::saveToFileXml(artisti, "saves/xml/test_artisti.xml");
        auto loadedXml = DataManager::loadFromFileXml("saves/xml/test_artisti.xml");
        assertDeepEquality(artisti, loadedXml);

        for (auto& p : artisti) delete p.second;
        for (auto& p : loadedJson) delete p.second;
        for (auto& p : loadedXml) delete p.second;
    } catch (const std::exception& ex) {
        ErrorManager::showError(ex.what());
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);
    qDebug() << "[DEBUG] Working directory:" << QDir::currentPath();

    while (true) {
        std::cout << "\n==== Programma Gestione Artisti ====\n"
                  << "1. Esegui test di salvataggio/caricamento\n"
                  << "2. Avvia programma in modalità CLI\n"
                  << "3. Avvia programma in modalità GUI\n"
                  << "0. Esci\n"
                  << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (scelta) {
        case 1:
            runTests();  // esegue i test e torna al menu
            break;
        case 2: {
            ConsoleApp console;
            console.run();
            break;
        }
        case 3: {
            QFile styleFile(":/styles/style.qss");
            if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
                QTextStream ts(&styleFile);
                app.setStyleSheet(ts.readAll());
                qDebug() << "✅ style.qss applicato correttamente!";
            } else {
                qDebug() << "❌ style.qss non trovato!";
            }

            MainWindow w;
            w.show();
            return app.exec();  // esce dal ciclo principale
        }
        case 0:
            return 0;
        default:
            std::cout << "Scelta non valida.\n";
            break;
        }
    }
}