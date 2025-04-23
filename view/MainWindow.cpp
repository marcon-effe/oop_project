#include "MainWindow.h"
#include "VisitorGUI.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QSpacerItem>
#include <QPushButton>



#include "../model/artisti/Artista.h"
#include "../model/core/ArtistProduct.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/musica/Traccia.h"
#include "../include/dataManager.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    loadDataFromSaves();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (centralWidget()) {
        auto splitter = centralWidget()->findChild<QSplitter *>();
        if (splitter) {
            QWidget* leftPanel = splitter->widget(0);
            if (leftPanel) {
                int newMax = width() / 3;
                leftPanel->setMaximumWidth(newMax);
            }
        }
    }
}

void MainWindow::setupUI() {
    // Menu
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Importa");
    fileMenu->addAction("Esporta");
    fileMenu->addSeparator();
    fileMenu->addAction("Esci", this, SLOT(close()));

    QMenu *inserisciMenu = menuBar()->addMenu("Inserisci");
    inserisciMenu->addAction("Artista");
    inserisciMenu->addAction("Prodotto");

    // Centrale
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Splitter principale (orizzontale)
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(8);
    mainSplitter->setStyleSheet("QSplitter::handle { background-color: white; }");

    // LEFT PANEL
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    // Splitter verticale interno sinistra (artisti sopra, prodotti sotto)
    QSplitter *leftSplitter = new QSplitter(Qt::Vertical, leftPanel);
    leftSplitter->setHandleWidth(5);

    // Artisti section
    QWidget *artistiSection = new QWidget();
    QVBoxLayout *artistiLayout = new QVBoxLayout(artistiSection);
    artistiLayout->setContentsMargins(4, 4, 4, 4);

    QLabel *labelArtisti = new QLabel("Artisti");
    QHBoxLayout *searchLayoutArtisti = new QHBoxLayout();
    QLineEdit *searchArtisti = new QLineEdit();
    searchArtisti->setPlaceholderText("Cerca artisti...");
    QPushButton *filtroArtisti = new QPushButton("Filtri");
    searchLayoutArtisti->addWidget(searchArtisti);
    searchLayoutArtisti->addWidget(filtroArtisti);

    artistListWidget = new QListWidget();

    artistiLayout->addWidget(labelArtisti);
    artistiLayout->addLayout(searchLayoutArtisti);
    artistiLayout->addWidget(artistListWidget);

    // Prodotti section
    QWidget *prodottiSection = new QWidget();
    QVBoxLayout *prodottiLayout = new QVBoxLayout(prodottiSection);
    prodottiLayout->setContentsMargins(4, 4, 4, 4);

    QLabel *labelProdotti = new QLabel("Prodotti");
    QHBoxLayout *searchLayoutProdotti = new QHBoxLayout();
    QLineEdit *searchProdotti = new QLineEdit();
    searchProdotti->setPlaceholderText("Cerca prodotti...");
    QPushButton *filtroProdotti = new QPushButton("Filtri");
    searchLayoutProdotti->addWidget(searchProdotti);
    searchLayoutProdotti->addWidget(filtroProdotti);

    productListFullWidget = new QListWidget();

    prodottiLayout->addWidget(labelProdotti);
    prodottiLayout->addLayout(searchLayoutProdotti);
    prodottiLayout->addWidget(productListFullWidget);

    // Inserimento sezioni nello splitter interno
    leftSplitter->addWidget(artistiSection);
    leftSplitter->addWidget(prodottiSection);
    leftSplitter->setStretchFactor(0, 1);
    leftSplitter->setStretchFactor(1, 1);
    leftSplitter->setStyleSheet("QSplitter::handle { background-color: white; }");

    leftLayout->addWidget(leftSplitter);

    // RIGHT PANEL
    QWidget *rightPanel = new QWidget();
    rightPanel->setObjectName("rightPanel");
    QVBoxLayout *rightOuterLayout = new QVBoxLayout(rightPanel);
    rightOuterLayout->setContentsMargins(40, 40, 40, 40);  // Margini esterni controllati

    // Contenitore centrale per i dettagli
    QWidget* detailsContainer = new QWidget();
    detailsContainer->setObjectName("detailsContainer");
    detailsContainer->setStyleSheet(R"(
        #detailsContainer {
            padding: 10px 20px;
            border: 2px dashed red;
            background-color: rgb(255, 255, 255);
        }
    )");

    rightLayout = new QVBoxLayout(detailsContainer);
    rightLayout->setSpacing(10);  // spazio tra elementi interni
    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->setContentsMargins(0, 0, 0, 0); // Nessun margine extra interno

    // Aggiunta diretta al layout destro
    rightOuterLayout->addWidget(detailsContainer);

    // Splitter principale
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);

    // Imposta proporzione 1/3 - 2/3
    QList<int> sizes;
    sizes << width() / 3 << (2 * width() / 3);
    mainSplitter->setSizes(sizes);

    // Imposta limite massimo della parte sinistra
    leftPanel->setMaximumWidth(width() / 3);

    // Layout finale
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mainSplitter);

    // Collegamenti
    connect(artistListWidget, &QListWidget::itemClicked, this, &MainWindow::handleArtistSelection);
    connect(productListFullWidget, &QListWidget::itemClicked, this, &MainWindow::handleProductSelection);
}

void MainWindow::loadDataFromSaves(const std::string& path) {
    std::vector<Artista*> loaded = DataManager::loadFromFileJson(path);

    // Inserisci gli artisti uno a uno nella QList
    for (Artista* a : loaded) {
        artists.append(a); // da std::vector a QList
        artistListWidget->addItem(QString::fromStdString(a->getNome()));

        for (ArtistProduct* p : a->getProducts()) {
            prodotti.append(p);
            productListFullWidget->addItem(QString::fromStdString(p->getTitle()));
        }
    }
}

void MainWindow::handleArtistSelection(QListWidgetItem* item) {
    clearRightPanel();
    for (Artista* a : artists) {
        if (QString::fromStdString(a->getNome()) == item->text()) {
            VisitorGUI* visitor = new VisitorGUI();
            a->accept(visitor);
            rightLayout->addWidget(visitor->getWidget());
            return;
        }
    }
}

void MainWindow::handleProductSelection(QListWidgetItem* item) {
    clearRightPanel();
    for (ArtistProduct* p : prodotti) {
        if (QString::fromStdString(p->getTitle()) == item->text()) {
            VisitorGUI* visitor = new VisitorGUI();
            p->accept(visitor);
            rightLayout->addWidget(visitor->getWidget());
            return;
        }
    }
}

void MainWindow::clearRightPanel() {
    QLayoutItem* child;
    while ((child = rightLayout->takeAt(0)) != nullptr) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
}