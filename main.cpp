#include <iostream>
#include <unordered_map>
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

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);
    qDebug() << "[DEBUG] Working directory:" << QDir::currentPath();

    QString fullStyle;

    QFile general(":/styles/style.qss");
    if (general.open(QFile::ReadOnly | QFile::Text)) {
        fullStyle += QLatin1String(general.readAll());
    } else {
        qDebug() << "style.qss non trovato!";
    }

    QFile dialogs(":/styles/dialog_styles.qss");
    if (dialogs.open(QFile::ReadOnly | QFile::Text)) {
        fullStyle += QLatin1String(dialogs.readAll());
    } else {
        qDebug() << "dialog_styles.qss non trovato!";
    }

    app.setStyleSheet(fullStyle);
    qDebug() << "Tutti i fogli di stile applicati correttamente!";

    while (true) {
        std::cout << "\n==== Programma Gestione Artisti ====\n"
                  << "1. Avvia programma in modalità CLI\n"
                  << "2. Avvia programma in modalità GUI\n"
                  << "0. Esci\n"
                  << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (scelta) {
        case 1: {
            ConsoleApp console;
            console.run();
            break;
        }
        case 2: {
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