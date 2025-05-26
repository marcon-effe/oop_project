#include "MainWindow.h"
#include "VisitorGUI.h"

#include <QVBoxLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QSpacerItem>
#include <QPushButton>
#include <QFileDialog>
#include <QInputDialog>
#include <QCloseEvent>
#include <QToolButton>

#include "../model/artisti/Artista.h"
#include "../model/core/ArtistProduct.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/musica/Traccia.h"

#include "../data/DataManager.h"
#include "ErrorManager.h"

#include "filters/FilterDialog.h"
#include "crud_dialogs/ArtistaEditorDialog.h"
#include "crud_dialogs/ProdottoInsertDialog.h"
#include "crud_dialogs/ProdottoEditorDialog.h"
#include "crud_dialogs/ArtistaDeleteDialog.h"
#include "crud_dialogs/ProdottoDeleteDialog.h"

static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

MainWindow::~MainWindow() {
    DataManager::cleanUpArtist(artists);
    prodotti.clear();
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

void MainWindow::closeEvent(QCloseEvent* event) {
    if (autosaveAction && autosaveAction->isChecked()) {
        DataManager::saveToFileJson(artists, "saves/autosave.json");
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::setupUI() {
    // === MENÙ SUPERIORE ===
    QMenu *fileMenu = menuBar()->addMenu("File");
    QAction* nuovoAction = fileMenu->addAction("Nuovo");
    QAction* importAction = fileMenu->addAction("Importa");
    QAction* exportAction = fileMenu->addAction("Esporta");
    QAction* autosaveAction = new QAction("Autosave", this);
    autosaveAction->setCheckable(true);
    autosaveAction->setChecked(true);
    fileMenu->addAction(autosaveAction);

    this->autosaveAction = autosaveAction;

    fileMenu->addSeparator();
    fileMenu->addAction("Esci", this, SLOT(close()));

    QMenu *inserisciMenu = menuBar()->addMenu("Inserisci");
    inserisciArtista = inserisciMenu->addAction("Artista");
    inserisciProdotto = inserisciMenu->addAction("Prodotto");

    QMenu *modificaMenu = menuBar()->addMenu("Modifica");
    modificaArtista = modificaMenu->addAction("Artista");
    modificaProdotto = modificaMenu->addAction("Prodotto");

    QMenu *eliminaMenu = menuBar()->addMenu("Elimina");
    eliminaArtista = eliminaMenu->addAction("Artista");
    eliminaProdotto = eliminaMenu->addAction("Prodotto");

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
    connect(nuovoAction, &QAction::triggered, this, &MainWindow::onNuovoProgetto);
    connect(importAction, &QAction::triggered, this, &MainWindow::importData);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportData);

    connect(inserisciArtista, &QAction::triggered, this, &MainWindow::onInserisciArtista);
    connect(inserisciProdotto, &QAction::triggered, this, &MainWindow::onInserisciProdotto);

    connect(modificaArtista, &QAction::triggered, this, &MainWindow::onModificaArtista);
    connect(modificaProdotto, &QAction::triggered, this, &MainWindow::onModificaProdotto);

    connect(eliminaArtista, &QAction::triggered, this, &MainWindow::onEliminaArtista);
    connect(eliminaProdotto, &QAction::triggered, this, &MainWindow::onEliminaProdotto);

    connect(artistListWidget, &QListWidget::itemClicked, this, &MainWindow::handleArtistSelection);
    connect(productListFullWidget, &QListWidget::itemClicked, this, &MainWindow::handleProductSelection);

    connect(searchArtisti, &QLineEdit::textChanged, this, &MainWindow::filterArtistList);
    connect(searchProdotti, &QLineEdit::textChanged, this, &MainWindow::filterProductList);

    connect(filtroArtisti, &QPushButton::clicked, this, &MainWindow::openArtistFilterDialog);
    connect(filtroProdotti, &QPushButton::clicked, this, &MainWindow::openProductFilterDialog);

    //== LOAD DATA ===
    const std::string autosavePath = "saves/autosave.json";
    if (QFile::exists(QString::fromStdString(autosavePath))) {
        loadDataFromSaves(autosavePath);
        if(!artists.empty()) {
            updateListWidgets();
        }
    }
    showMaximized();
}

// ------------ INSERIMENTO ARTISTA E PRODOTTO ------------
void MainWindow::onInserisciArtista()
{
    std::set<std::string> nomiSan;
    for (const auto& pair : artists)
        nomiSan.insert(DataManager::sanitizeForPath(pair.second->getNome()));

    ArtistaEditorDialog dialog(nullptr, this, nomiSan);
    if (dialog.exec() == QDialog::Accepted) {
        Artista* nuovo = dialog.artist();
        if (nuovo) {
            artists[nuovo->getId()] = nuovo;
            updateListWidgets();
        }
    }
    saveIfAutosaveEnabled();
}

void MainWindow::onInserisciProdotto()
{
    if (artists.empty()) {
        QMessageBox::warning(this, "Attenzione", "Nessun artista disponibile. Crea prima un artista.");
        return;
    }

    ProdottoInsertDialog* dialog = new ProdottoInsertDialog(artists, prodotti, this);
    dialog->exec();
    updateListWidgets();
    saveIfAutosaveEnabled();
}
// ------------------------------

// ------------ MODIFICA ARTISTA E PRODOTTO ------------
void MainWindow::onModificaArtista()
{
    QListWidgetItem* item = artistListWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Attenzione", "Seleziona un artista da modificare.");
        return;
    }

    const QString nomeArtista = item->text();
    Artista* artista = nullptr;

    for (const auto& [id, ptr] : artists) {
        if (QString::fromStdString(ptr->getNome()) == nomeArtista) {
            artista = ptr;
            break;
        }
    }

    if (!artista) return;

    std::set<std::string> nomiSan;
    for (const auto& pair : artists)
        nomiSan.insert(DataManager::sanitizeForPath(pair.second->getNome()));

    ArtistaEditorDialog dialog(artista, this, nomiSan);

    if (dialog.exec() == QDialog::Accepted) {
        clearRightPanel();
        updateListWidgets();
        saveIfAutosaveEnabled();
    }
}

void MainWindow::onModificaProdotto() {
    ArtistProduct* prodotto = nullptr;

    QListWidgetItem* item = productListFullWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Attenzione", "Seleziona un prodotto da modificare.");
        return;
    }

    unsigned int id = item->data(Qt::UserRole).toUInt();
    auto it = prodotti.find(id);
    if (it == prodotti.end()) {
        QMessageBox::critical(this, "Errore", "Prodotto non trovato.");
        return;
    }

    prodotto = it->second;

    if (!prodotto) {
        QDialog sceltaDialog(this);
        sceltaDialog.setWindowTitle("Seleziona prodotto da modificare");
        QVBoxLayout* layout = new QVBoxLayout(&sceltaDialog);

        QComboBox* artistaCombo = new QComboBox(&sceltaDialog);
        for (const auto& [id, artista] : artists) {
            artistaCombo->addItem(QString::fromStdString(artista->getNome()), QVariant::fromValue(id));
        }

        QComboBox* prodottoCombo = new QComboBox(&sceltaDialog);

        auto aggiornaProdotti = [&]() {
            prodottoCombo->clear();
            unsigned int id = artistaCombo->currentData().toUInt();
            auto artista = artists.at(id);
            for (const auto& [pid, prod] : artista->getProducts()) {
                prodottoCombo->addItem(QString::fromStdString(prod->getTitle()), QVariant::fromValue(prod->getId()));
            }
        };

        connect(artistaCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), aggiornaProdotti);    // disambiguazione dell'overload della funzione
        artistaCombo->setCurrentIndex(0);  // forza caricamento iniziale
        aggiornaProdotti();

        layout->addWidget(new QLabel("Artista:"));
        layout->addWidget(artistaCombo);
        layout->addWidget(new QLabel("Prodotto:"));
        layout->addWidget(prodottoCombo);

        QDialogButtonBox* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &sceltaDialog);
        layout->addWidget(btnBox);
        connect(btnBox, &QDialogButtonBox::accepted, &sceltaDialog, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, &sceltaDialog, &QDialog::reject);

        if (sceltaDialog.exec() == QDialog::Accepted) {
            unsigned int id = prodottoCombo->currentData().toUInt();
            auto it = prodotti.find(id);
            if (it != prodotti.end()) {
                prodotto = it->second;
            } else {
                QMessageBox::critical(this, "Errore", "Prodotto non trovato.");
                return;
            }
        } else {
            return;  // utente ha annullato
        }
    }

    ProdottoEditorDialog dialog(prodotto, artists, prodotti, this);
    if (dialog.exec() == QDialog::Accepted) {
        clearRightPanel();
        updateListWidgets();
        saveIfAutosaveEnabled();
    }
}
// ------------------------------

// ------------ ELIMINA ARTISTA E PRODOTTO ------------
void MainWindow::onEliminaArtista() {
    if (artists.empty()) {
        QMessageBox::information(this, "Eliminazione artista", "Non ci sono artisti da eliminare.");
        return;
    }

    ArtistaDeleteDialog dialog(artists, prodotti, this);
    if (dialog.esegui()) {
        updateListWidgets();
        saveIfAutosaveEnabled();
    }
}

void MainWindow::onEliminaProdotto() {
    if (prodotti.empty()) {
        QMessageBox::information(this, "Eliminazione prodotto", "Non ci sono prodotti da eliminare.");
        return;
    }

    ProdottoDeleteDialog dialog(artists, prodotti, this);
    if (dialog.esegui()) {
        updateListWidgets();
        saveIfAutosaveEnabled();
    }
}



/// RIPRISTINO GUI ------------
void MainWindow::sortArtistListWidget() {
    QList<QListWidgetItem*> items;
    for (int i = 0; i < artistListWidget->count(); ++i) {
        items.append(artistListWidget->takeItem(i));
        --i;
    }

    std::sort(items.begin(), items.end(), [](QListWidgetItem* a, QListWidgetItem* b) {
        return a->text().toLower() < b->text().toLower();
    });

    for (QListWidgetItem* item : items)
        artistListWidget->addItem(item);
}

void MainWindow::sortProductListWidget() {
    QList<QListWidgetItem*> items;
    for (int i = 0; i < productListFullWidget->count(); ++i) {
        items.append(productListFullWidget->takeItem(i));
        --i;  // decrementa perché la lista si accorcia
    }

    std::sort(items.begin(), items.end(), [](QListWidgetItem* a, QListWidgetItem* b) {
        return a->text().toLower() < b->text().toLower();
    });

    for (QListWidgetItem* item : items)
        productListFullWidget->addItem(item);
}

void MainWindow::updateListWidgets() {
    artistListWidget->clear();
    productListFullWidget->clear();

    for (const auto& pair : artists)
        artistListWidget->addItem(QString::fromStdString(pair.second->getNome()));

    for (const auto& pair : prodotti) {
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(pair.second->getTitle()));
        item->setData(Qt::UserRole, QVariant::fromValue(pair.first));
        productListFullWidget->addItem(item);
    }

    sortArtistListWidget();
    sortProductListWidget();
}

void MainWindow::clearAll() {
    artistListWidget->clear();
    productListFullWidget->clear();
    clearRightPanel();
    DataManager::cleanUpArtist(artists);
    prodotti.clear();
}

void MainWindow::clearRightPanel() {
    QLayoutItem* child;
    while ((child = rightLayout->takeAt(0)) != nullptr) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }
}

void MainWindow::onNuovoProgetto() {
    QMessageBox::StandardButton risposta = QMessageBox::question(
        this,
        "Nuovo progetto",
        "Vuoi salvare il progetto corrente prima di iniziare uno nuovo?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
    );

    if (risposta == QMessageBox::Cancel) {
        return;  // utente ha annullato
    }

    if (risposta == QMessageBox::Yes) {
        exportData();  // salva prima di cancellare
    }

    clearAll();  // azzera i dati a prescindere da Yes/No
}
// -----------------------------


// IMPORTAZIONE/ESPORTAZIONE DATI
void MainWindow::saveIfAutosaveEnabled() {
    if (autosaveAction && autosaveAction->isChecked()) {
        DataManager::saveToFileJson(artists, "saves/autosave.json");
    }
}

void MainWindow::importData() {
    QString fileName = QFileDialog::getOpenFileName(this, "Importa dati", "", "JSON Files (*.json);;XML Files (*.xml)");
    if (fileName.isEmpty()) return;

    clearAll();

    loadDataFromSaves(fileName.toStdString());

    if (artists.empty()) {
        ErrorManager::showError("Importazione fallita o file vuoto/non valido.");
        ErrorManager::logError("Errore durante l'importazione da file: " + fileName.toStdString());
    } else {
        QMessageBox::information(this, "Importa", "Dati importati con successo.");
    }
}

void MainWindow::loadDataFromSaves(const std::string& path) {
    DataManager::cleanUpArtist(artists);
    prodotti.clear();

    if (endsWith(path, ".json")) {
        artists = DataManager::loadFromFileJson(path);
    } else if (endsWith(path, ".xml")) {
        artists = DataManager::loadFromFileXml(path);
    } else {
        std::string msg = "Formato file non supportato: " + path;
        ErrorManager::showError("Estensione del file non supportata. Sono ammessi solo .json e .xml.");
        ErrorManager::logError(msg);
        return;
    }

    for (const auto& pair : artists) {
        Artista* a = pair.second;
        for (const auto& innerPair : a->getProducts()) {
            ArtistProduct* p = innerPair.second;
            prodotti[p->getId()] = p;
        }
    }

    saveIfAutosaveEnabled();
    updateListWidgets();
}

void MainWindow::exportData() {
    QDialog dialog(this);
    dialog.setWindowTitle("Esporta dati");
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    // Combo formato
    QComboBox* formatCombo = new QComboBox(&dialog);
    formatCombo->addItems({"JSON", "XML"});
    layout->addWidget(new QLabel("Scegli il formato di esportazione:", &dialog));
    layout->addWidget(formatCombo);

    // Checkbox esportazione ridotta
    QHBoxLayout* reducedLayout = new QHBoxLayout;
    QCheckBox* reducedCheck = new QCheckBox("Esportazione ridotta", &dialog);

    QToolButton* infoButton = new QToolButton(&dialog);
    infoButton->setIcon(QIcon(":/icons/info.png"));  // usa una tua icona oppure lascia il tema
    infoButton->setToolTip("Non salva le immagini in formato Base64 all'interno del file.");
    infoButton->setAutoRaise(true);
    infoButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");

    reducedLayout->addWidget(reducedCheck);
    reducedLayout->addWidget(infoButton);
    reducedLayout->addStretch();
    layout->addLayout(reducedLayout);

    // Bottoni
    QDialogButtonBox* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(btnBox);
    connect(btnBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) return;

    const QString selectedFormat = formatCombo->currentText();
    const bool reduced = reducedCheck->isChecked();
    const QString filter = selectedFormat == "JSON" ? "JSON Files (*.json)" : "XML Files (*.xml)";
    const QString extension = selectedFormat == "JSON" ? ".json" : ".xml";

    QString fileName = QFileDialog::getSaveFileName(this, "Salva file", "", filter);
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(extension)) fileName += extension;

    bool success = false;
    if (selectedFormat == "JSON") {
        success = DataManager::saveToFileJson(artists, fileName.toStdString(), reduced);
    } else {
        success = DataManager::saveToFileXml(artists, fileName.toStdString(), reduced);
    }

    if (success) {
        QMessageBox::information(this, "Esporta", "Dati esportati con successo.");
    } else {
        ErrorManager::showError("Errore durante l'esportazione dei dati.");
        ErrorManager::logError("Errore salvataggio su file: " + fileName.toStdString());
    }
}
// -----------------------------


// LISTA ARTISTI E PRODOTTI ----
void MainWindow::handleArtistSelection(QListWidgetItem* item) {
    clearRightPanel();
    for (const auto& pair : artists) {
        Artista* a = pair.second;
        if (QString::fromStdString(a->getNome()) == item->text()) {
            VisitorGUI* visitor = new VisitorGUI(&artists, this);
            a->accept(visitor);
            rightLayout->addWidget(visitor->getWidget());
            return;
        }
    }
}

void MainWindow::handleProductSelection(QListWidgetItem* item) {
    clearRightPanel();

    unsigned int id = item->data(Qt::UserRole).toUInt();
    auto it = prodotti.find(id);
    if (it != prodotti.end()) {
        ArtistProduct* p = it->second;
        VisitorGUI* visitor = new VisitorGUI(&artists, this);
        p->accept(visitor);
        rightLayout->addWidget(visitor->getWidget());
    }
}

//  ricerca
void MainWindow::filterArtistList(const QString& query) {
    artistListWidget->clear();
    QString lowerQuery = query.trimmed().toLower();

    for (const auto& pair : artists) {
        const Artista* a = pair.second;
        QString nome = QString::fromStdString(a->getNome());
        if (nome.toLower().contains(lowerQuery)) {
            artistListWidget->addItem(nome);
        }
    }
}

void MainWindow::filterProductList(const QString& query) {
    productListFullWidget->clear();
    QString lowerQuery = query.trimmed().toLower();

    for (const auto& pair : prodotti) {
        const ArtistProduct* p = pair.second;
        QString titolo = QString::fromStdString(p->getTitle());
        if (titolo.toLower().contains(lowerQuery)) {
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(p->getTitle()));
            item->setData(Qt::UserRole, QVariant::fromValue(p->getId()));
            productListFullWidget->addItem(item);
        }
    }
}


// filtri
void MainWindow::applyArtistGenreFilter(const QString& genere) {
    artistListWidget->clear();

    for (const auto& pair : artists) {
        Artista* a = pair.second;
        if (genere == "Tutti" || QString::fromStdString(a->getGenere()) == genere) {
            artistListWidget->addItem(QString::fromStdString(a->getNome()));
        }
    }

    sortArtistListWidget();
}

void MainWindow::applyProductFilters(const std::vector<std::string>& tipi, const QString& genereMusica, bool disponibile, const QString& artistaId) 
{
    productListFullWidget->clear();

    for (const auto& pair : prodotti) {
        ArtistProduct* p = pair.second;

        // Filtro per tipo
        bool tipoMatch = tipi.empty();
        for (const std::string& t : tipi) {
            if ((t == "Album" && dynamic_cast<Album*>(p)) ||
                (t == "Singolo" && dynamic_cast<Singolo*>(p)) ||
                (t == "CD" && dynamic_cast<CD*>(p)) ||
                (t == "Vinile" && dynamic_cast<Vinile*>(p)) ||
                (t == "TShirt" && dynamic_cast<TShirt*>(p)) ||
                (t == "Tour" && dynamic_cast<Tour*>(p))) {
                tipoMatch = true;
                break;
            }
        }
        if (!tipoMatch) continue;

        // Filtro per genere (solo se è Musica)
        if (auto* m = dynamic_cast<Musica*>(p)) {
            if (genereMusica != "Tutti" && QString::fromStdString(m->getGenere()) != genereMusica)
                continue;
        }

        // Filtro per disponibile (solo se è NotMusica)
        if (auto* nm = dynamic_cast<NotMusica*>(p)) {
            if (disponibile && !nm->getDisponibile())
                continue;
        }

        // Filtro per artista
        if (artistaId != "Tutti" && QString::number(p->getArtistId()) != artistaId)
            continue;

        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(p->getTitle()));
        item->setData(Qt::UserRole, QVariant::fromValue(p->getId()));
        productListFullWidget->addItem(item);
    }
    sortProductListWidget();
}

void MainWindow::openArtistFilterDialog() {
    FilterDialog dialog(this);
    dialog.setupForArtisti(artists);

    if (dialog.exec() == QDialog::Accepted) {
        QString genere = dialog.getSelectedGenere();
        applyArtistGenreFilter(genere);
    }
}

void MainWindow::openProductFilterDialog() {
    FilterDialog dialog(this);
    dialog.setupForProdotti(prodotti, artists);

    if (dialog.exec() == QDialog::Accepted) {
        std::vector<std::string> tipi = dialog.getSelectedTipoProdotto();
        QString genereMusica = dialog.getSelectedGenereMusica();  // per musica
        bool disponibile = dialog.isDisponibileChecked();   // per not musica
        QString artistaId = dialog.getSelectedArtistaId();  // filtro artista

        applyProductFilters(tipi, genereMusica, disponibile, artistaId);
    }
}
// --------------------------------------