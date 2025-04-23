#include <iostream>
#include <vector>
#include <cassert>
#include <QApplication>
#include <QDir>
#include <QDebug>

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

#include <iostream>
#include <cassert>
#include <typeinfo>

#include <QResource>


void assertDeepEquality(const std::vector<Artista*>& original, const std::vector<Artista*>& loaded) {
    assert(original.size() == loaded.size());

    for (size_t i = 0; i < original.size(); ++i) {
        const Artista* o = original[i];
        const Artista* l = loaded[i];

        std::cout << "▶ Confronto artista " << i << ": " << o->getNome() << std::endl;

        if (!(o->getNome() == l->getNome())) {
            std::cerr << "❌ Nomi diversi:\n"
                      << " - Originale: " << o->getNome() << "\n"
                      << " - Caricato:  " << l->getNome() << std::endl;
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

        std::cout << " - Prodotti originali: " << originalProd.size() << ", caricati: " << loadedProd.size() << std::endl;

        for (size_t j = 0; j < originalProd.size(); ++j) {
            const ArtistProduct* oProd = originalProd[j];
            const ArtistProduct* lProd = loadedProd[j];

            std::string oType = typeid(*oProd).name();
            std::string lType = typeid(*lProd).name();

            std::cout << "   ▶ Confronto prodotto " << j << ": "
                      << oType << " [" << oProd->getTitle() << "]"
                      << " vs "
                      << lType << " [" << lProd->getTitle() << "]" << std::endl;

            if (oType != lType) {
                std::cerr << "   ❌ Tipo diverso nel prodotto " << j << "!\n"
                          << "    - Originale: " << oType << "\n"
                          << "    - Caricato:  " << lType << std::endl;
                assert(false);
            }

            if (!(*oProd == *lProd)) {
                std::cerr << "   ❌ Differenza nel contenuto del prodotto " << j << " (type: " << oType << ")\n"
                          << "    - Originale title: " << oProd->getTitle() << "\n"
                          << "    - Caricato  title: " << lProd->getTitle() << std::endl;
                assert(false);
            } else {
                std::cout << "   ✅ Prodotto " << j << " [" << oProd->getTitle() << "] OK" << std::endl;
            }
        }
    }

    std::cout << "✅ Tutti gli artisti e i prodotti corrispondono perfettamente!" << std::endl;
}

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(resources);
    qDebug() << "[DEBUG] Working directory:" << QDir::currentPath();
    
    std::vector<Artista*> artisti;

    {
        Artista* a = new Artista("Artista1");
        Traccia t1("Traccia1", {"Artista1"}, Durata(0, 3, 30), "Testo1", true);
        Traccia t2("Traccia2", {"Artista1"}, Durata(0, 4, 0), "Testo2", false);
        a->addProduct(new Album("Album1", "desc", Data(1,1,2020), Durata(0,42,10), "Rock", {t1, t2}, "Label1"));
        a->addProduct(new Singolo("Sing1", "desc", Data(2,2,2021), Durata(0,3,30), "Pop", t1, false, 1));
        a->addProduct(new CD("CD1", "desc", 15.5, true, 10, "CP1", "StampeX", "CR1", "Standard", "Jewel"));
        a->addProduct(new Vinile("Vin1", "desc", 20.0, true, 5, "VP1", "StampeY", "CR2", "Limited", 33, 12));
        a->addProduct(new TShirt("TS1", "desc", 10.0, true, 100, "TP1", "M", "Nero"));
        Tour* tour = new Tour("Tour1", "desc", 99.99, true, 200);
        tour->addDataTour(DataTour(10,5,2025,20,30,0,"Roma"));
        a->addProduct(tour);
        artisti.push_back(a);
    }

    // JSON
    //assert(DataManager::saveToFileJson(artisti, "saves/json/test_artisti.json") && "Salvataggio JSON fallito");
    //auto loadedJson = DataManager::loadFromFileJson("saves/json/test_artisti.json");
    //assertDeepEquality(artisti, loadedJson);

    // XML
    //assert(DataManager::saveToFileXml(artisti, "saves/xml/test_artisti.xml") && "Salvataggio XML fallito");
    auto loadedXml = DataManager::loadFromFileXml("saves/xml/test_artisti.xml");
    assertDeepEquality(artisti, loadedXml);

    std::cout << "✅ Tutti i test superati: uguaglianza semantica confermata!" << std::endl;

    for (auto* a : artisti) delete a;
    //for (auto* a : loadedJson) delete a;
    for (auto* a : loadedXml) delete a;


    QApplication app(argc, argv);   // 👈 Deve essere il PRIMO oggetto Qt

    QFile styleFile(":/styles/style.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&styleFile);
        app.setStyleSheet(ts.readAll());
        qDebug() << "✅ style.qss applicato correttamente!";
    } else {
        qDebug() << "❌ style.qss non trovato!";
    }


    MainWindow w;                   // 👈 Dopo QApplication
    w.show();

    return app.exec();     
}