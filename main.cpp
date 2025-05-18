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

#include "data/DataManager.h"
#include "view/MainWindow.h"
#include "view/ErrorManager.h"
#include "cli/ConsoleApp.h"

void runTestCompleto() {
    try {
        std::string suffix = std::to_string(std::time(nullptr));
        std::unordered_map<unsigned int, Artista*> artisti;

        for (int i = 1; i <= 3; ++i) {
            std::string nome = "ArtistaTest_" + std::to_string(i) + "_" + suffix;
            Artista* a = new Artista(nome, "GenereTest_"+nome, "InfoTest_" + nome);
            std::string base = "Prod_" + std::to_string(i) + "_" + suffix;

            // Tracce
            Traccia tr1(base + "_tr1", {nome}, Durata(0, 3, 45), "Testo1", true);
            Traccia tr2(base + "_tr2", {nome}, Durata(0, 4, 10), "Testo2", false);
            std::vector<Traccia> tracce = {tr1, tr2};

            // Album (costruttore con 8 parametri)
            a->addProduct(new Album(a, base + "_album", "Album desc", Data(1, 1, 2022), Durata(0, 8, 55), "Rock", tracce, "LabelX"));

            // Singolo (con main track e info complete)
            a->addProduct(new Singolo(a, base + "_singolo", "Singolo desc", Data(2, 2, 2023), Durata(0, 3, 45), "Pop", tr1, true, 3));

            // TShirt
            a->addProduct(new TShirt(a, base + "_ts1", "TShirt estiva", 25.0, true, 50, "Cotone", "M", "Nero"));
            a->addProduct(new TShirt(a, base + "_ts2", "TShirt invernale", 30.0, false, 20, "Lana", "L", "Blu"));

            // Tour (con costruttore base + vector<DataTour>)
            std::vector<DataTour> tappe = {
                DataTour(1, 3, 2023, 21, 0, 0, "Milano"),
                DataTour(2, 3, 2023, 20, 30, 0, "Roma")
            };

            a->addProduct(new Tour(a, base + "_tour1", "Tour europeo", 45.0, true, 100, tappe));

            // CD (costruttore con 11 parametri)
            a->addProduct(new CD(a, base + "_cd1", "CD desc", 15.0, true, 30,
                                 "cod123", "Sony", "ric123", "Audio", "Standard"));

            // Vinile (costruttore con 12 parametri)
            a->addProduct(new Vinile(a, base + "_vinile1", "Vinile desc", 30.0, false, 20,
                                     "vin321", "Universal", "ric999", "LP", 33, 12));

            artisti[a->getId()] = a;
        }

        std::cout << "\n[Salvataggio JSON]" << std::endl;
        DataManager::saveToFileJson(artisti, "saves/json/test_completo.json");

        std::cout << "[Caricamento JSON]" << std::endl;
        auto loadedJson = DataManager::loadFromFileJson("saves/json/test_completo.json");
        for (const auto& [_, art] : loadedJson) art->printInfo();

        std::cout << "\n[Salvataggio XML]" << std::endl;
        DataManager::saveToFileXml(artisti, "saves/xml/test_completo.xml");

        std::cout << "[Caricamento XML]" << std::endl;
        auto loadedXml = DataManager::loadFromFileXml("saves/xml/test_completo.xml");
        for (const auto& [_, art] : loadedXml) art->printInfo();

        for (auto& [_, a] : artisti) delete a;
        for (auto& [_, a] : loadedJson) delete a;
        for (auto& [_, a] : loadedXml) delete a;

        std::cout << "\nTest completo terminato con successo.\n";

    } catch (const std::exception& ex) {
        ErrorManager::showError(std::string("Errore durante il test completo: ") + ex.what());
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
            runTestCompleto();
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
                qDebug() << "style.qss applicato correttamente!";
            } else {
                qDebug() << "style.qss non trovato!";
            }

            MainWindow w;
            w.show();
            return app.exec();
        }
        case 0:
            return 0;
        default:
            std::cout << "Scelta non valida.\n";
            break;
        }
    }
}