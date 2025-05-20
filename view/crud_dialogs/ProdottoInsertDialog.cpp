#include "ProdottoInsertDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QGuiApplication>
#include <QScreen>
#include <QDialogButtonBox>
#include "../../data/DataManager.h"


ProdottoInsertDialog::ProdottoInsertDialog(
    std::unordered_map<unsigned int, Artista*>& artistiRef,
    std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
    QWidget* parent
)
    : QDialog(parent), artisti(artistiRef), prodotti(prodottiRef), artistaSelezionato(nullptr)
{
    setWindowTitle("Inserisci nuovo prodotto");
    setMinimumWidth(500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* artistaLayout = new QHBoxLayout;
    QLabel* artistaLabel = new QLabel("Seleziona artista:");
    artistaComboBox = new QComboBox;

    for (const auto& pair : artisti) {
        artistaComboBox->addItem(QString::fromStdString(pair.second->getNome()), QVariant::fromValue(pair.first));
    }

    artistaLayout->addWidget(artistaLabel);
    artistaLayout->addWidget(artistaComboBox);

    QPushButton* confermaArtistaBtn = new QPushButton("Conferma artista");
    connect(confermaArtistaBtn, &QPushButton::clicked, this, [this]() {
        if (artistaComboBox->currentIndex() < 0) {
            QMessageBox::warning(this, "Selezione artista", "Seleziona un artista prima di procedere.");
            return;
        }

        unsigned int id = artistaComboBox->currentData().toUInt();
        artistaSelezionato = artisti.at(id);
        artistaComboBox->setEnabled(false);
        sender()->deleteLater();
        setupUI();
    });

    QHBoxLayout* artistaBox = new QHBoxLayout;
    artistaBox->addLayout(artistaLayout);
    artistaBox->addWidget(confermaArtistaBtn);

    mainLayout->addLayout(artistaBox);
}

void ProdottoInsertDialog::setupUI()
{
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout());

    // Scroll area per la parte dinamica
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    QWidget* scrollContent = new QWidget(this);
    scrollLayout = new QVBoxLayout(scrollContent);

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    // BOX: Selezione tipo prodotto
    QGroupBox* tipoBox = new QGroupBox("Tipo di prodotto");
    QHBoxLayout* tipoLayout = new QHBoxLayout;
    tipoComboBox = new QComboBox;
    tipoComboBox->addItems({"TShirt", "CD", "Vinile", "Album", "Singolo", "Tour"});

    tipoLayout->addWidget(new QLabel("Tipo:"));
    tipoLayout->addWidget(tipoComboBox);
    tipoBox->setLayout(tipoLayout);
    scrollLayout->addWidget(tipoBox);

    // Contenitore dei campi dinamici
    campiSpecificiContainer = new QWidget;
    campiSpecificiLayout = new QVBoxLayout(campiSpecificiContainer);
    scrollLayout->addWidget(campiSpecificiContainer);

    // Cambio dinamico in base al tipo selezionato
    connect(tipoComboBox, &QComboBox::currentTextChanged, this, [this](const QString& tipo) {
        // Pulisce i campi precedenti
        QLayoutItem* item;
        while ((item = campiSpecificiLayout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }


        if (tipo == "TShirt") buildTShirt();
        else if (tipo == "CD") buildCD();
        else if (tipo == "Vinile") buildVinile();
        else if (tipo == "Tour") buildTour();
        else if (tipo == "Album") buildAlbum();
        else if (tipo == "Singolo") buildSingolo();
    });

    QDialogButtonBox* btnBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton* annullaBtn = btnBox->addButton(QDialogButtonBox::Cancel);
    QPushButton* confermaBtn = btnBox->addButton("Aggiungi prodotto", QDialogButtonBox::AcceptRole);

    btnBox->setCenterButtons(false);  // allinea a destra (default)
    btnBox->setContentsMargins(0, 10, 0, 0);

    connect(annullaBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(confermaBtn, &QPushButton::clicked, this, &ProdottoInsertDialog::confermaInserimento);

    scrollLayout->addWidget(btnBox);

    // Innesca la prima costruzione iniziale
    emit tipoComboBox->currentTextChanged(tipoComboBox->currentText());

    // Ridimensionamento dopo selezione artista
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QSize screenSize = screen->availableGeometry().size();
        int w = static_cast<int>(screenSize.width() * 0.3);
        int h = static_cast<int>(screenSize.height() * 0.95);
        resize(w, h);
        move((screenSize.width() - w) / 2, 5);
    }

}

void ProdottoInsertDialog::selezionaImmagine() {
    QString filePath = QFileDialog::getOpenFileName(this, "Seleziona immagine", QString(),
                                                    "Immagini (*.png *.jpg *.jpeg)");
    if (!filePath.isEmpty()) {
        imagePathEdit->setText(filePath);
    }
}

void ProdottoInsertDialog::aggiornaCampiSpecifici() {
    // Pulisce i campi precedenti
    QLayoutItem* item;
    while ((item = campiSpecificiLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    const QString tipo = tipoComboBox->currentText();

    if (tipo == "TShirt") buildTShirt();
    else if (tipo == "CD") buildCD();
    else if (tipo == "Vinile") buildVinile();
    else if (tipo == "Album") buildAlbum();
    else if (tipo == "Singolo") buildSingolo();
    else if (tipo == "Tour") buildTour();
}

void ProdottoInsertDialog::buildArtistProduct() {
    QGroupBox* box = new QGroupBox("Dati generali");
    QFormLayout* form = new QFormLayout;

    titoloEdit = new QLineEdit(this);
    form->addRow("Titolo:", titoloEdit);

    descrizioneEdit = new QTextEdit(this);
    descrizioneEdit->setFixedHeight(60);
    form->addRow("Descrizione:", descrizioneEdit);

    imagePathEdit = new QLineEdit(this);
    imagePathEdit->setReadOnly(true);
    QPushButton* sfogliaBtn = new QPushButton("Sfoglia...", this);
    connect(sfogliaBtn, &QPushButton::clicked, this, &ProdottoInsertDialog::selezionaImmagine);

    QHBoxLayout* imgLayout = new QHBoxLayout;
    imgLayout->addWidget(imagePathEdit);
    imgLayout->addWidget(sfogliaBtn);
    form->addRow("Immagine:", imgLayout);

    box->setLayout(form);
    campiSpecificiLayout->addWidget(box);
}

void ProdottoInsertDialog::buildMusica() {
    buildArtistProduct();
    QGroupBox* box = new QGroupBox("Dati musicali");
    QFormLayout* form = new QFormLayout;

    dataUscitaEdit = new QDateEdit(this);
    dataUscitaEdit->setCalendarPopup(true);
    dataUscitaEdit->setDisplayFormat("dd/MM/yyyy");
    dataUscitaEdit->setDate(QDate::currentDate());
    form->addRow("Data di uscita:", dataUscitaEdit);

    genereEdit = new QLineEdit(this);
    form->addRow("Genere:", genereEdit);

    box->setLayout(form);
    campiSpecificiLayout->addWidget(box);
}

void ProdottoInsertDialog::buildNotMusica() {
    buildArtistProduct();
    QGroupBox* box = new QGroupBox("Dati di vendita");
    QFormLayout* form = new QFormLayout;

    prezzoSpin = new QDoubleSpinBox(this);
    prezzoSpin->setRange(0, 10000);
    prezzoSpin->setDecimals(2);
    prezzoSpin->setPrefix("€ ");
    form->addRow("Prezzo:", prezzoSpin);

    disponibileCheck = new QCheckBox("Disponibile", this);
    form->addRow("", disponibileCheck);

    quantitaSpin = new QSpinBox(this);
    quantitaSpin->setRange(0, 100000);
    form->addRow("Quantità:", quantitaSpin);

    box->setLayout(form);
    campiSpecificiLayout->addWidget(box);
}

void ProdottoInsertDialog::buildMerch() {
    buildNotMusica();
    QGroupBox* merchBox = new QGroupBox("Informazioni di Merchandising");
    QFormLayout* merchLayout = new QFormLayout;

    codiceProdottoEdit = new QLineEdit(this);
    merchLayout->addRow("Codice prodotto:", codiceProdottoEdit);

    merchBox->setLayout(merchLayout);
    campiSpecificiLayout->addWidget(merchBox);
}

void ProdottoInsertDialog::buildDisco() {
    buildMerch();
    QGroupBox* discoBox = new QGroupBox("Dettagli del supporto fisico");
    QFormLayout* discoLayout = new QFormLayout;

    produttoreStampeEdit = new QLineEdit(this);
    discoLayout->addRow("Produttore stampe:", produttoreStampeEdit);

    codiceRiconoscimentoEdit = new QLineEdit(this);
    discoLayout->addRow("Codice riconoscimento:", codiceRiconoscimentoEdit);

    tipoProdottoEdit = new QLineEdit(this);
    discoLayout->addRow("Tipo prodotto:", tipoProdottoEdit);

    discoBox->setLayout(discoLayout);
    campiSpecificiLayout->addWidget(discoBox);
}

void ProdottoInsertDialog::buildCD() {
    buildDisco();

    QGroupBox* cdBox = new QGroupBox("Dettagli CD");
    QFormLayout* cdLayout = new QFormLayout;

    formatoEdit = new QLineEdit(this);
    cdLayout->addRow("Formato:", formatoEdit);

    cdBox->setLayout(cdLayout);
    campiSpecificiLayout->addWidget(cdBox);
}

void ProdottoInsertDialog::buildVinile() {
    buildDisco();  // Campi comuni del Disco

    QGroupBox* vinileBox = new QGroupBox("Dettagli Vinile");
    QFormLayout* vinileLayout = new QFormLayout;

    rpmSpin = new QSpinBox(this);
    rpmSpin->setRange(1, 10000);  // valore massimo ragionevole per RPM
    vinileLayout->addRow("RPM:", rpmSpin);

    diametroSpin = new QSpinBox(this);
    diametroSpin->setRange(1, 100);  // in pollici o cm a seconda della convenzione
    vinileLayout->addRow("Diametro:", diametroSpin);

    vinileBox->setLayout(vinileLayout);
    campiSpecificiLayout->addWidget(vinileBox);
}

void ProdottoInsertDialog::buildTShirt() {
    buildMerch();  // Campi base del merchandising

    QGroupBox* tshirtBox = new QGroupBox("Dettagli T-Shirt");
    QFormLayout* tshirtLayout = new QFormLayout;

    tagliaEdit = new QLineEdit(this);
    tshirtLayout->addRow("Taglia:", tagliaEdit);

    coloreEdit = new QLineEdit(this);
    tshirtLayout->addRow("Colore:", coloreEdit);

    tshirtBox->setLayout(tshirtLayout);
    campiSpecificiLayout->addWidget(tshirtBox);
}

void ProdottoInsertDialog::buildTour() {
    buildNotMusica();  // Include i campi comuni come prezzo, disponibile, quantita, codice prodotto

    QGroupBox* tourBox = new QGroupBox("Date del Tour");
    QVBoxLayout* tourMainLayout = new QVBoxLayout;

    dateTourWidget = new QWidget(this);
    dateTourLayout = new QVBoxLayout(dateTourWidget);

    aggiungiDataTourBtn = new QPushButton("Aggiungi data", this);
    connect(aggiungiDataTourBtn, &QPushButton::clicked, this, [this]() {
        TourDataWidget newDate;

        newDate.dataEdit = new QDateEdit(this);
        newDate.dataEdit->setCalendarPopup(true);
        newDate.dataEdit->setDate(QDate::currentDate());

        newDate.orarioEdit = new QTimeEdit(this);
        newDate.orarioEdit->setTime(QTime::currentTime());

        newDate.luogoEdit = new QLineEdit(this);

        QFormLayout* dataLayout = new QFormLayout;
        dataLayout->addRow("Data:", newDate.dataEdit);
        dataLayout->addRow("Orario:", newDate.orarioEdit);
        dataLayout->addRow("Luogo:", newDate.luogoEdit);

        QWidget* dataGroup = new QWidget(this);
        dataGroup->setLayout(dataLayout);

        dateTourLayout->addWidget(dataGroup);
        dateTourWidgets.push_back(newDate);
    });

    tourMainLayout->addWidget(dateTourWidget);
    tourMainLayout->addWidget(aggiungiDataTourBtn);
    tourBox->setLayout(tourMainLayout);

    campiSpecificiLayout->addWidget(tourBox);
}

void ProdottoInsertDialog::buildSingolo() {
    buildMusica();  // Campi: data di uscita, genere

    QGroupBox* singoloBox = new QGroupBox("Traccia principale");
    QFormLayout* singoloLayout = new QFormLayout;

    nomeTracciaEdit = new QLineEdit(this);
    singoloLayout->addRow("Nome traccia:", nomeTracciaEdit);

    oreTraccia = new QSpinBox(this);
    minutiTraccia = new QSpinBox(this);
    secondiTraccia = new QSpinBox(this);
    oreTraccia->setRange(0, 23);
    minutiTraccia->setRange(0, 59);
    secondiTraccia->setRange(0, 59);

    QHBoxLayout* durataLayout = new QHBoxLayout;
    durataLayout->addWidget(new QLabel("Durata:"));
    durataLayout->addWidget(oreTraccia);
    durataLayout->addWidget(new QLabel("h"));
    durataLayout->addWidget(minutiTraccia);
    durataLayout->addWidget(new QLabel("m"));
    durataLayout->addWidget(secondiTraccia);
    durataLayout->addWidget(new QLabel("s"));

    QWidget* durataWidget = new QWidget(this);
    durataWidget->setLayout(durataLayout);
    singoloLayout->addRow(durataWidget);

    hasTestoTracciaCheck = new QCheckBox("Ha testo", this);
    singoloLayout->addRow("", hasTestoTracciaCheck);

    testoTracciaEdit = new QTextEdit(this);
    testoTracciaEdit->setFixedHeight(60);
    singoloLayout->addRow("Testo:", testoTracciaEdit);

    partecipantiSingoloWidget = new QWidget(this);
    partecipantiSingoloLayout = new QVBoxLayout(partecipantiSingoloWidget);
    aggiungiPartecipanteSingoloBtn = new QPushButton("Aggiungi partecipante", this);
    connect(aggiungiPartecipanteSingoloBtn, &QPushButton::clicked, this, [this]() {
        QLineEdit* nuovoPartecipante = new QLineEdit(this);
        partecipantiSingoloLayout->addWidget(nuovoPartecipante);
        partecipantiSingoloLines.push_back(nuovoPartecipante);
    });

    QVBoxLayout* partecipantiSection = new QVBoxLayout;
    partecipantiSection->addWidget(partecipantiSingoloWidget);
    partecipantiSection->addWidget(aggiungiPartecipanteSingoloBtn);

    QWidget* partecipantiWrapper = new QWidget(this);
    partecipantiWrapper->setLayout(partecipantiSection);
    singoloLayout->addRow("Partecipanti:", partecipantiWrapper);

    remixCheck = new QCheckBox("È un remix", this);
    singoloLayout->addRow("", remixCheck);

    chartPosSpin = new QSpinBox(this);
    chartPosSpin->setRange(0, 1000);
    singoloLayout->addRow("Posizione in classifica:", chartPosSpin);

    singoloBox->setLayout(singoloLayout);
    campiSpecificiLayout->addWidget(singoloBox);
}

void ProdottoInsertDialog::buildAlbum() {
    buildMusica();  // Campi: data di uscita, genere

    QGroupBox* albumBox = new QGroupBox("Dettagli Album");
    QFormLayout* albumLayout = new QFormLayout;

    labelEdit = new QLineEdit(this);
    albumLayout->addRow("Label discografica:", labelEdit);

    albumBox->setLayout(albumLayout);
    campiSpecificiLayout->addWidget(albumBox);

    // Sezione Tracce
    QGroupBox* tracceBox = new QGroupBox("Tracce");
    QVBoxLayout* tracceBoxLayout = new QVBoxLayout(tracceBox);

    tracceWidget = new QWidget(this);
    tracceLayout = new QVBoxLayout(tracceWidget);

    aggiungiTracciaBtn = new QPushButton("Aggiungi traccia", this);
    connect(aggiungiTracciaBtn, &QPushButton::clicked, this, [this]() {
        TracciaEditor* editor = new TracciaEditor;

        editor->nomeEdit = new QLineEdit(this);

        editor->oreDurata = new QSpinBox(this);
        editor->minutiDurata = new QSpinBox(this);
        editor->secondiDurata = new QSpinBox(this);
        editor->oreDurata->setRange(0, 23);
        editor->minutiDurata->setRange(0, 59);
        editor->secondiDurata->setRange(0, 59);

        editor->hasTestoCheck = new QCheckBox("Ha testo", this);
        editor->testoEdit = new QTextEdit(this);
        editor->testoEdit->setFixedHeight(50);

        // Partecipanti
        editor->partecipantiWidget = new QWidget(this);
        editor->partecipantiLayout = new QVBoxLayout(editor->partecipantiWidget);
        editor->aggiungiPartecipanteBtn = new QPushButton("Aggiungi partecipante", this);
        connect(editor->aggiungiPartecipanteBtn, &QPushButton::clicked, this, [this, editor]() {
            QLineEdit* nuovo = new QLineEdit(this);
            editor->partecipantiLayout->addWidget(nuovo);
            editor->partecipantiLines.push_back(nuovo);
        });

        QFormLayout* tracciaLayout = new QFormLayout;
        tracciaLayout->addRow("Nome:", editor->nomeEdit);

        QHBoxLayout* durataLayout = new QHBoxLayout;
        durataLayout->addWidget(editor->oreDurata);
        durataLayout->addWidget(new QLabel("h"));
        durataLayout->addWidget(editor->minutiDurata);
        durataLayout->addWidget(new QLabel("m"));
        durataLayout->addWidget(editor->secondiDurata);
        durataLayout->addWidget(new QLabel("s"));

        QWidget* durataWidget = new QWidget(this);
        durataWidget->setLayout(durataLayout);
        tracciaLayout->addRow("Durata:", durataWidget);

        tracciaLayout->addRow("", editor->hasTestoCheck);
        tracciaLayout->addRow("Testo:", editor->testoEdit);

        QVBoxLayout* partecipantiSection = new QVBoxLayout;
        partecipantiSection->addWidget(editor->partecipantiWidget);
        partecipantiSection->addWidget(editor->aggiungiPartecipanteBtn);

        QWidget* partecipantiWrapper = new QWidget(this);
        partecipantiWrapper->setLayout(partecipantiSection);
        tracciaLayout->addRow("Partecipanti:", partecipantiWrapper);

        QGroupBox* tracciaBox = new QGroupBox("Traccia");
        tracciaBox->setLayout(tracciaLayout);

        tracceLayout->addWidget(tracciaBox);
        tracceEditors.push_back(editor);
    });
    
    tracceBoxLayout->addWidget(tracceWidget);
    tracceBoxLayout->addWidget(aggiungiTracciaBtn);
    campiSpecificiLayout->addWidget(tracceBox);
}

void ProdottoInsertDialog::confermaInserimento() {
    if (!artistaSelezionato) {
        QMessageBox::warning(this, "Errore", "Seleziona un artista.");
        return;
    }
    std::string titolo = titoloEdit->text().toStdString();
    if(titolo.empty()) {
        QMessageBox::warning(this, "Errore", "Il titolo non può essere vuoto.");
        return;
    }
    for (const auto& pair : artistaSelezionato->getProducts()) {
        if (DataManager::sanitizeForPath(pair.second->getTitle()) == DataManager::sanitizeForPath(titolo)) {
            QMessageBox::warning(this, "Prodotto già esistente", "Esiste già un prodotto con questo titolo (post sanitizzazione) per l'artista selezionato.");
            return;
        }
    }
    std::string descrizione = descrizioneEdit->toPlainText().toStdString();
    std::string imagePath = imagePathEdit->text().toStdString();

    QString tipo = tipoComboBox->currentText();

    try {
        if (tipo == "TShirt") {
            double prezzo = prezzoSpin->value();
            bool disponibile = disponibileCheck->isChecked();
            unsigned int quantita = quantitaSpin->value();
            std::string codice = codiceProdottoEdit->text().toStdString();
            std::string taglia = tagliaEdit->text().toStdString();
            std::string colore = coloreEdit->text().toStdString();

            auto tshirt = new TShirt(artistaSelezionato, titolo, descrizione, prezzo, disponibile, quantita, codice, taglia, colore);
            tshirt->setImagePath(imagePath);
            prodotti[tshirt->getId()] = tshirt;
            artistaSelezionato->addProduct(tshirt);
        } else if (tipo == "CD") {
            double prezzo = prezzoSpin->value();
            bool disponibile = disponibileCheck->isChecked();
            unsigned int quantita = quantitaSpin->value();
            std::string codice = codiceProdottoEdit->text().toStdString();
            std::string produttore = produttoreStampeEdit->text().toStdString();
            std::string riconoscimento = codiceRiconoscimentoEdit->text().toStdString();
            std::string tipoProd = tipoProdottoEdit->text().toStdString();
            std::string formato = formatoEdit->text().toStdString();

            auto cd = new CD(artistaSelezionato, titolo, descrizione, prezzo, disponibile, quantita, codice, produttore, riconoscimento, tipoProd, formato);
            cd->setImagePath(imagePath);
            prodotti[cd->getId()] = cd;
            artistaSelezionato->addProduct(cd);
        } else if (tipo == "Vinile") {
            double prezzo = prezzoSpin->value();
            bool disponibile = disponibileCheck->isChecked();
            unsigned int quantita = quantitaSpin->value();
            std::string codice = codiceProdottoEdit->text().toStdString();
            std::string produttore = produttoreStampeEdit->text().toStdString();
            std::string riconoscimento = codiceRiconoscimentoEdit->text().toStdString();
            std::string tipoProd = tipoProdottoEdit->text().toStdString();
            unsigned int rpm = rpmSpin->value();
            unsigned int diametro = diametroSpin->value();

            auto vinile = new Vinile(artistaSelezionato, titolo, descrizione, prezzo, disponibile, quantita, codice, produttore, riconoscimento, tipoProd, rpm, diametro);
            vinile->setImagePath(imagePath);
            prodotti[vinile->getId()] = vinile;
            artistaSelezionato->addProduct(vinile);
        } else if (tipo == "Tour") {
            double prezzo = prezzoSpin->value();
            bool disponibile = disponibileCheck->isChecked();
            unsigned int quantita = quantitaSpin->value();

            auto tour = new Tour(artistaSelezionato, titolo, descrizione, prezzo, disponibile, quantita);
            tour->setImagePath(imagePath);

            for (const auto& w : dateTourWidgets) {
                QDate d = w.dataEdit->date();
                QTime t = w.orarioEdit->time();
                std::string luogo = w.luogoEdit->text().toStdString();
                tour->addDataTour(DataTour(d.day(), d.month(), d.year(), t.hour(), t.minute(), t.second(), luogo));
            }

            prodotti[tour->getId()] = tour;
            artistaSelezionato->addProduct(tour);
        } else if (tipo == "Singolo") {
            QDate d = dataUscitaEdit->date();
            std::string genere = genereEdit->text().toStdString();

            std::string nome = nomeTracciaEdit->text().toStdString();
            if (nome.empty()) {
                QMessageBox::warning(this, "Errore", "Il nome della traccia principale non può essere vuoto.");
                return;
            }
            bool hasTesto = hasTestoTracciaCheck->isChecked();
            std::string testo = testoTracciaEdit->toPlainText().toStdString();
            Durata durata(oreTraccia->value(), minutiTraccia->value(), secondiTraccia->value());

            std::vector<std::string> partecipanti;
            for (auto* line : partecipantiSingoloLines) {
                if(!line->text().isEmpty()) {
                    partecipanti.push_back(line->text().toStdString());
                }
            }

            Traccia traccia(nome, partecipanti, durata, testo, hasTesto);
            bool remix = remixCheck->isChecked();
            int chart = chartPosSpin->value();

            auto singolo = new Singolo(artistaSelezionato, titolo, descrizione, Data(d.day(), d.month(), d.year()), Durata(0, 0, 0), genere, traccia, remix, chart);
            singolo->updateDurata();
            singolo->setImagePath(imagePath);
            prodotti[singolo->getId()] = singolo;
            artistaSelezionato->addProduct(singolo);
        } else if (tipo == "Album") {
            QDate d = dataUscitaEdit->date();
            std::string genere = genereEdit->text().toStdString();
            std::string label = labelEdit->text().toStdString();

            std::vector<Traccia> tracce;
            for (const auto& ed : tracceEditors) {
                std::string nome = ed->nomeEdit->text().toStdString();
                if (nome.empty()) {
                    QMessageBox::warning(this, "Errore", "Ogni traccia deve avere un nome.");
                    return;
                }
                bool hasTesto = ed->hasTestoCheck->isChecked();
                std::string testo = ed->testoEdit->toPlainText().toStdString();
                Durata durata(ed->oreDurata->value(), ed->minutiDurata->value(), ed->secondiDurata->value());

                std::vector<std::string> partecipanti;
                for (auto* line : partecipantiSingoloLines) {
                    if(!line->text().isEmpty()) {
                        partecipanti.push_back(line->text().toStdString());
                    }
                }

                tracce.emplace_back(nome, partecipanti, durata, testo, hasTesto);
            }

            auto album = new Album(artistaSelezionato, titolo, descrizione, Data(d.day(), d.month(), d.year()), Durata(0, 0, 0), genere, tracce, label);
            album->updateDurata();
            album->setImagePath(imagePath);
            prodotti[album->getId()] = album;
            artistaSelezionato->addProduct(album);
        }

        for (TracciaEditor* editor : tracceEditors) {
            delete editor;
        }
        tracceEditors.clear();
        accept();
    } catch (const std::exception& ex) {
        QMessageBox::critical(this, "Errore", ex.what());
    }
}
