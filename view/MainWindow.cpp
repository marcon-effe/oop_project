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

#include <unordered_map> // aggiunto

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
    // === MENÙ SUPERIORE ===
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Importa");
    fileMenu->addAction("Esporta");
    fileMenu->addSeparator();
    fileMenu->addAction("Esci", this, SLOT(close()));

    QMenu *inserisciMenu = menuBar()->addMenu("Inserisci");
    inserisciMenu->addAction("Artista");
    inserisciMenu->addAction("Prodotto");

    // === CENTRO ===
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    mainSplitter->setHandleWidth(8);
    mainSplitter->setStyleSheet("QSplitter::handle { background-color: #e0e0e0; }");

    // === LEFT PANEL ===
    QWidget *leftPanel = new QWidget();
    leftPanel->setObjectName("leftPanel");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter *leftSplitter = new QSplitter(Qt::Vertical, leftPanel);
    leftSplitter->setHandleWidth(5);

    // --- Artisti Section ---
    QWidget *artistiSection = new QWidget();
    QVBoxLayout *artistiLayout = new QVBoxLayout(artistiSection);
    artistiLayout->setContentsMargins(12, 12, 12, 12);
    artistiLayout->setSpacing(10);

    QLabel *labelArtisti = new QLabel("Artisti");
    labelArtisti->setObjectName("labelArtisti");

    QHBoxLayout *searchLayoutArtisti = new QHBoxLayout();
    QLineEdit *searchArtisti = new QLineEdit();
    searchArtisti->setPlaceholderText("Cerca artisti...");
    searchArtisti->setObjectName("searchArtisti");

    QPushButton *filtroArtisti = new QPushButton("Filtri");
    filtroArtisti->setObjectName("filtroArtisti");

    searchLayoutArtisti->addWidget(searchArtisti);
    searchLayoutArtisti->addWidget(filtroArtisti);

    artistListWidget = new QListWidget();
    artistListWidget->setObjectName("artistList");

    artistiLayout->addWidget(labelArtisti);
    artistiLayout->addLayout(searchLayoutArtisti);
    artistiLayout->addWidget(artistListWidget);

    // --- Prodotti Section ---
    QWidget *prodottiSection = new QWidget();
    QVBoxLayout *prodottiLayout = new QVBoxLayout(prodottiSection);
    prodottiLayout->setContentsMargins(12, 12, 12, 12);
    prodottiLayout->setSpacing(10);

    QLabel *labelProdotti = new QLabel("Prodotti");
    labelProdotti->setObjectName("labelProdotti");

    QHBoxLayout *searchLayoutProdotti = new QHBoxLayout();
    QLineEdit *searchProdotti = new QLineEdit();
    searchProdotti->setPlaceholderText("Cerca prodotti...");
    searchProdotti->setObjectName("searchProdotti");

    QPushButton *filtroProdotti = new QPushButton("Filtri");
    filtroProdotti->setObjectName("filtroProdotti");

    searchLayoutProdotti->addWidget(searchProdotti);
    searchLayoutProdotti->addWidget(filtroProdotti);

    productListFullWidget = new QListWidget();
    productListFullWidget->setObjectName("productList");

    prodottiLayout->addWidget(labelProdotti);
    prodottiLayout->addLayout(searchLayoutProdotti);
    prodottiLayout->addWidget(productListFullWidget);

    // --- Inserimento nelle sezioni ---
    leftSplitter->addWidget(artistiSection);
    leftSplitter->addWidget(prodottiSection);
    leftSplitter->setStretchFactor(0, 1);
    leftSplitter->setStretchFactor(1, 1);

    leftLayout->addWidget(leftSplitter);

    // === RIGHT PANEL ===
    QWidget *rightPanel = new QWidget();
    rightPanel->setObjectName("rightPanel");

    QVBoxLayout *rightOuterLayout = new QVBoxLayout(rightPanel);
    rightOuterLayout->setContentsMargins(40, 40, 40, 40);

    QWidget *detailsContainer = new QWidget();
    detailsContainer->setObjectName("detailsContainer");
    detailsContainer->setStyleSheet(R"(
        #detailsContainer {
            padding: 10px 20px;
            background-color: #ffffff;
            border-radius: 8px;
        }
    )");

    rightLayout = new QVBoxLayout(detailsContainer);
    rightLayout->setSpacing(10);
    rightLayout->setAlignment(Qt::AlignTop);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    rightOuterLayout->addWidget(detailsContainer);

    // === INSERIMENTO NEGLI SPLITTER ===
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);

    QList<int> sizes;
    sizes << width() / 3 << (2 * width() / 3);
    mainSplitter->setSizes(sizes);
    leftPanel->setMaximumWidth(width() / 3);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(mainSplitter);

    // === CONNESSIONI ===
    connect(artistListWidget, &QListWidget::itemClicked, this, &MainWindow::handleArtistSelection);
    connect(productListFullWidget, &QListWidget::itemClicked, this, &MainWindow::handleProductSelection);

    // === CARICAMENTO STILE DA FILE QSS (leftPanel.qss) ===
    QFile styleFile(":/styles/leftPanel.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString style = styleFile.readAll();
        leftPanel->setStyleSheet(style);
    }
}


void MainWindow::loadDataFromSaves(const std::string& path) {
    artists = DataManager::loadFromFileJson(path);

    for (const auto& pair : artists) {
        Artista* a = pair.second;
        artistListWidget->addItem(QString::fromStdString(a->getNome()));

        for (const auto& innerPair : a->getProducts()) {
            ArtistProduct* p = innerPair.second;
            prodotti[p->getId()] = p;
            productListFullWidget->addItem(QString::fromStdString(p->getTitle()));
        }
    }
}

void MainWindow::handleArtistSelection(QListWidgetItem* item) {
    clearRightPanel();
    for (const auto& pair : artists) {
        Artista* a = pair.second;
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
    for (const auto& pair : prodotti) {
        ArtistProduct* p = pair.second;
        if (QString::fromStdString(p->getTitle()) == item->text()) {
            VisitorGUI* visitor = new VisitorGUI();
            visitor->setArtistMap(artists);
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