#include "ProdottoFormBuilder.h"
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

#include "../../data/DataManager.h"


static int debug_counter = 0;

// Costruttore: se prodottoEsistente == nullptr, inserimento; altrimenti modifica
ProdottoFormBuilder::ProdottoFormBuilder(
    std::unordered_map<unsigned int, Artista*>& artistiRef,
    std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
    const ArtistProduct* prodottoEsistente,
    QWidget* parent
) : QWidget(parent),
    artisti(artistiRef),
    prodotti(prodottiRef),
    prodottoCorrente(prodottoEsistente),
    artistaSelezionato(nullptr),
    titoloEdit(nullptr),
    descrizioneEdit(nullptr),
    imagePathEdit(nullptr),
    prezzoSpin(nullptr),
    disponibileCheck(nullptr),
    quantitaSpin(nullptr),
    codiceProdottoEdit(nullptr),
    tagliaEdit(nullptr),
    coloreEdit(nullptr),
    produttoreStampeEdit(nullptr),
    codiceRiconoscimentoEdit(nullptr),
    tipoProdottoEdit(nullptr),
    formatoEdit(nullptr),
    rpmSpin(nullptr),
    diametroSpin(nullptr),
    dataUscitaEdit(nullptr),
    genereEdit(nullptr),
    nomeTracciaEdit(nullptr),
    oreTraccia(nullptr),
    minutiTraccia(nullptr),
    secondiTraccia(nullptr),
    hasTestoTracciaCheck(nullptr),
    testoTracciaEdit(nullptr),
    partecipantiSingoloWidget(nullptr),
    partecipantiSingoloLayout(nullptr),
    aggiungiPartecipanteSingoloBtn(nullptr),
    remixCheck(nullptr),
    chartPosSpin(nullptr),
    labelEdit(nullptr),
    tracceWidget(nullptr),
    tracceLayout(nullptr),
    aggiungiTracciaBtn(nullptr),
    annullaBtn(nullptr),
    salvaBtn(nullptr)
{
    setWindowTitle(prodottoCorrente ? "Modifica prodotto" : "Inserisci nuovo prodotto");
    setMinimumWidth(500);

    mainLayout = new QVBoxLayout(this);

    if (!prodottoCorrente) {
        setupInserimento();
    } else {
        setupModifica();
    }
}

void ProdottoFormBuilder::setupInserimento()
{
    selezioneContainer = new QWidget(this);
    auto* artistaLayout = new QHBoxLayout(selezioneContainer);

    QLabel* artistaLabel = new QLabel("Seleziona artista:", this);
    artistaComboBox = new QComboBox(this);
    for (const auto& kv : artisti) {
        artistaComboBox->addItem(
            QString::fromStdString(kv.second->getNome()),
            QVariant::fromValue(kv.first)
        );
    }
    confermaArtistaBtn = new QPushButton("Conferma artista", this);
    connect(confermaArtistaBtn, &QPushButton::clicked,
            this, &ProdottoFormBuilder::onConfermaArtista);

    artistaLayout->addWidget(artistaLabel);
    artistaLayout->addWidget(artistaComboBox);
    artistaLayout->addWidget(confermaArtistaBtn);
    mainLayout->addWidget(selezioneContainer);
}

void ProdottoFormBuilder::onConfermaArtista()
{
    if (artistaComboBox->currentIndex() < 0) {
        QMessageBox::warning(this, "Errore", "Devi selezionare un artista.");
        return;
    }
    unsigned int id = artistaComboBox->currentData().toUInt();
    artistaSelezionato = artisti.at(id);

    // Disabilita selezione artista e rimuove il pulsante
    artistaComboBox->setEnabled(false);
    confermaArtistaBtn->deleteLater();

    setupScrollArea();

    tipoBox = new QGroupBox("Tipo di prodotto", this);
    auto* tipoLayout = new QHBoxLayout(tipoBox);
    tipoComboBox = new QComboBox(this);
    tipoComboBox->addItems({ "TShirt", "CD", "Vinile", "Album", "Singolo", "Tour" });
    tipoLayout->addWidget(new QLabel("Tipo:", this));
    tipoLayout->addWidget(tipoComboBox);
    scrollLayout->addWidget(tipoBox);

    campiSpecificiContainer = new QWidget(this);
    campiSpecificiLayout = new QVBoxLayout(campiSpecificiContainer);
    scrollLayout->addWidget(campiSpecificiContainer);

    connect(tipoComboBox, &QComboBox::currentTextChanged,
            this, &ProdottoFormBuilder::onTipoChanged);

    onTipoChanged(tipoComboBox->currentText());


    if (auto* screen = QGuiApplication::primaryScreen()) {
        QSize s = screen->availableGeometry().size();
        int w = int(s.width() * 0.3), h = int(s.height() * 0.85);
        resize(w, h);
        move((s.width() - w) / 2, 5);
    }

    auto* bottomLayout = new QHBoxLayout;
    annullaBtn = new QPushButton("Annulla", this);
    salvaBtn   = new QPushButton("Aggiungi prodotto", this);
    connect(annullaBtn, &QPushButton::clicked, this, &QWidget::close);
    connect(salvaBtn,   &QPushButton::clicked, this, &ProdottoFormBuilder::onConfermaSalvataggio);
    bottomLayout->addStretch();
    bottomLayout->addWidget(annullaBtn);
    bottomLayout->addWidget(salvaBtn);
    mainLayout->addLayout(bottomLayout);
}


void ProdottoFormBuilder::setupModifica()
{
    qDebug() << "Modifica prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    unsigned int artId = prodottoCorrente->getArtistId();
    artistaSelezionato = artisti.at(artId);

    setupScrollArea();

    campiSpecificiContainer = new QWidget(this);
    campiSpecificiLayout    = new QVBoxLayout(campiSpecificiContainer);
    scrollLayout->addWidget(campiSpecificiContainer);

    buildCampiBase();
    if (   dynamic_cast<TShirt*> (const_cast<ArtistProduct*>(prodottoCorrente))
        || dynamic_cast<CD*>     (const_cast<ArtistProduct*>(prodottoCorrente))
        || dynamic_cast<Vinile*> (const_cast<ArtistProduct*>(prodottoCorrente))
        || dynamic_cast<Tour*>   (const_cast<ArtistProduct*>(prodottoCorrente)))
    {
        buildCampoVendita();
    }

    popolaCampiDaProdotto();

    auto* pulsLayout = new QHBoxLayout;
    annullaBtn = new QPushButton("Annulla", this);
    salvaBtn   = new QPushButton("Salva modifiche", this);
    connect(annullaBtn, &QPushButton::clicked, this, &QWidget::close);
    connect(salvaBtn,   &QPushButton::clicked, this, &ProdottoFormBuilder::onConfermaSalvataggio);
    pulsLayout->addStretch();
    pulsLayout->addWidget(annullaBtn);
    pulsLayout->addWidget(salvaBtn);
    scrollLayout->addLayout(pulsLayout);

    if (auto* screen = QGuiApplication::primaryScreen()) {
        QSize s = screen->availableGeometry().size();
        int w = int(s.width() * 0.3), h = int(s.height() * 0.85);
        resize(w, h);
        move((s.width() - w) / 2, 5);
    }
}

void ProdottoFormBuilder::setupScrollArea()
{
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollContent = new QWidget(this);
    scrollLayout = new QVBoxLayout(scrollContent);
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);
}

void ProdottoFormBuilder::buildCampiBase()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi base per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi base per prodotto  // c " << debug_counter++;

    auto* box = new QGroupBox("Dati generali", this);
    auto* form = new QFormLayout(box);

    titoloEdit = new QLineEdit(this);
    form->addRow("Titolo:", titoloEdit);

    descrizioneEdit = new QTextEdit(this);
    descrizioneEdit->setFixedHeight(60);
    form->addRow("Descrizione:", descrizioneEdit);

    imagePathEdit = new QLineEdit(this);
    imagePathEdit->setReadOnly(true);
    auto* sfoglia = new QPushButton("Sfoglia...", this);
    connect(sfoglia, &QPushButton::clicked, this, &ProdottoFormBuilder::onSelezionaImmagine);
    auto* hImg = new QHBoxLayout;
    hImg->addWidget(imagePathEdit);
    hImg->addWidget(sfoglia);
    form->addRow("Immagine:", hImg);

    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoVendita()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi vend per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi vend per prodotto  // c " << debug_counter++;

    auto* box = new QGroupBox("Dati di vendita", this);
    auto* form = new QFormLayout(box);

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

    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoMerch()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi merch per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi merch per prodotto  // c " << debug_counter++;

    auto* box = new QGroupBox("Dati di vendita", this);
    auto* form = new QFormLayout(box);

    codiceProdottoEdit = new QLineEdit(this);
    form->addRow("Codice prodotto:", codiceProdottoEdit);

    campiSpecificiLayout->addWidget(box);

}

void ProdottoFormBuilder::buildCampoDisco()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi disc per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi disc per prodotto  // c " << debug_counter++;

    buildCampoMerch();

    auto* box = new QGroupBox("Dettagli supporto fisico", this);
    auto* form = new QFormLayout(box);

    produttoreStampeEdit = new QLineEdit(this);
    form->addRow("Produttore stampe:", produttoreStampeEdit);

    codiceRiconoscimentoEdit = new QLineEdit(this);
    form->addRow("Codice riconoscimento:", codiceRiconoscimentoEdit);

    tipoProdottoEdit = new QLineEdit(this);
    form->addRow("Tipo prodotto:", tipoProdottoEdit);

    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoTShirt()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi ts per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi ts per prodotto  // c " << debug_counter++;

    buildCampoMerch();

    auto* box = new QGroupBox("Dettagli T-Shirt", this);
    auto* form = new QFormLayout(box);

    tagliaEdit = new QLineEdit(this);
    form->addRow("Taglia:", tagliaEdit);

    coloreEdit = new QLineEdit(this);
    form->addRow("Colore:", coloreEdit);

    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoCD()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi cd per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi cd per prodotto  // c " << debug_counter++;

    buildCampoDisco();

    auto* box = new QGroupBox("Dettagli CD", this);
    auto* form = new QFormLayout(box);

    formatoEdit = new QLineEdit(this);
    form->addRow("Formato:", formatoEdit);

    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoVinile()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi vn per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi vn per prodotto  // c " << debug_counter++;
    
    buildCampoDisco();

    auto* box = new QGroupBox("Dettagli Vinile", this);
    auto* form = new QFormLayout(box);

    rpmSpin = new QSpinBox(this);
    rpmSpin->setRange(1, 10000);
    form->addRow("RPM:", rpmSpin);

    diametroSpin = new QSpinBox(this);
    diametroSpin->setRange(1, 100);
    form->addRow("Diametro:", diametroSpin);

    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoTour()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi to per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi to per prodotto  // c " << debug_counter++;

    auto* box = new QGroupBox("Date del Tour", this);
    auto* layout = new QVBoxLayout(box);

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

        auto* form = new QFormLayout;
        form->addRow("Data:", newDate.dataEdit);
        form->addRow("Orario:", newDate.orarioEdit);
        form->addRow("Luogo:", newDate.luogoEdit);

        auto* container = new QWidget(this);
        container->setLayout(form);
        dateTourLayout->addWidget(container);
        dateTourWidgets.push_back(newDate);
    });

    layout->addWidget(dateTourWidget);
    layout->addWidget(aggiungiDataTourBtn);
    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoMusica()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi mus per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi mus per prodotto  // c " << debug_counter++;

    auto* box = new QGroupBox("Dati musicali", this);
    auto* form = new QFormLayout(box);

    dataUscitaEdit = new QDateEdit(this);
    dataUscitaEdit->setCalendarPopup(true);
    dataUscitaEdit->setDisplayFormat("dd/MM/yyyy");
    dataUscitaEdit->setDate(QDate::currentDate());
    form->addRow("Data di uscita:", dataUscitaEdit);

    genereEdit = new QLineEdit(this);
    form->addRow("Genere:", genereEdit);

    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoSingolo()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi si per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi si per prodotto  // c " << debug_counter++;

    buildCampoMusica();
    auto* box = new QGroupBox("Traccia principale", this);
    auto* form = new QFormLayout(box);

    nomeTracciaEdit = new QLineEdit(this);
    form->addRow("Nome traccia:", nomeTracciaEdit);

    oreTraccia = new QSpinBox(this);
    minutiTraccia = new QSpinBox(this);
    secondiTraccia = new QSpinBox(this);
    oreTraccia->setRange(0,23);
    minutiTraccia->setRange(0,59);
    secondiTraccia->setRange(0,59);

    auto* hDur = new QHBoxLayout;
    hDur->addWidget(new QLabel("Durata:", this));
    hDur->addWidget(oreTraccia);
    hDur->addWidget(new QLabel("h", this));
    hDur->addWidget(minutiTraccia);
    hDur->addWidget(new QLabel("m", this));
    hDur->addWidget(secondiTraccia);
    hDur->addWidget(new QLabel("s", this));
    form->addRow(hDur);

    hasTestoTracciaCheck = new QCheckBox("Ha testo", this);
    form->addRow("", hasTestoTracciaCheck);

    testoTracciaEdit = new QTextEdit(this);
    testoTracciaEdit->setFixedHeight(60);
    form->addRow("Testo:", testoTracciaEdit);

    partecipantiSingoloWidget = new QWidget(this);
    partecipantiSingoloLayout = new QVBoxLayout(partecipantiSingoloWidget);
    aggiungiPartecipanteSingoloBtn = new QPushButton("Aggiungi partecipante", this);
    connect(aggiungiPartecipanteSingoloBtn, &QPushButton::clicked, this, [this]() {
        auto* line = new QLineEdit(this);
        partecipantiSingoloLayout->addWidget(line);
        partecipantiSingoloLines.push_back(line);
    });

    auto* vPar = new QVBoxLayout;
    vPar->addWidget(partecipantiSingoloWidget);
    vPar->addWidget(aggiungiPartecipanteSingoloBtn);
    form->addRow("Partecipanti:", vPar);

    remixCheck = new QCheckBox("È un remix", this);
    form->addRow("", remixCheck);

    chartPosSpin = new QSpinBox(this);
    chartPosSpin->setRange(0,1000);
    form->addRow("Posizione in classifica:", chartPosSpin);

    campiSpecificiLayout->addWidget(box);
}

void ProdottoFormBuilder::buildCampoAlbum()
{
    if (prodottoCorrente) 
        qDebug() << "Costruzione campi al per prodotto: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "Costruzione campi al per prodotto  // c " << debug_counter++;

    buildCampoMusica();  

    auto* box = new QGroupBox("Dettagli Album", this);
    auto* form = new QFormLayout(box);

    labelEdit = new QLineEdit(this);
    form->addRow("Label discografica:", labelEdit);

    campiSpecificiLayout->addWidget(box);

    auto* tracceBox = new QGroupBox("Tracce", this);
    auto* vTr = new QVBoxLayout(tracceBox);

    tracceWidget = new QWidget(this);
    tracceLayout = new QVBoxLayout(tracceWidget);

    aggiungiTracciaBtn = new QPushButton("Aggiungi traccia", this);
    connect(aggiungiTracciaBtn, &QPushButton::clicked, this, [this]() {
        auto* editor = new TracciaEditor;

        editor->nomeEdit = new QLineEdit(this);

        editor->oreDurata    = new QSpinBox(this);
        editor->minutiDurata = new QSpinBox(this);
        editor->secondiDurata= new QSpinBox(this);
        editor->oreDurata->setRange(0, 23);
        editor->minutiDurata->setRange(0, 59);
        editor->secondiDurata->setRange(0, 59);

        editor->hasTestoCheck = new QCheckBox("Ha testo", this);
        editor->testoEdit = new QTextEdit(this);
        editor->testoEdit->setFixedHeight(50);

        editor->partecipantiWidget = new QWidget(this);
        editor->partecipantiLayout = new QVBoxLayout(editor->partecipantiWidget);
        editor->aggiungiPartecipanteBtn = new QPushButton("Aggiungi partecipante", this);
        connect(editor->aggiungiPartecipanteBtn, &QPushButton::clicked, this, [this, editor]() {
            auto* line = new QLineEdit(this);
            editor->partecipantiLayout->addWidget(line);
            editor->partecipantiLines.push_back(line);
        });

        auto* formT = new QFormLayout;
        formT->addRow("Nome:", editor->nomeEdit);

        auto* hDur = new QHBoxLayout;
        hDur->addWidget(editor->oreDurata);
        hDur->addWidget(new QLabel("h", this));
        hDur->addWidget(editor->minutiDurata);
        hDur->addWidget(new QLabel("m", this));
        hDur->addWidget(editor->secondiDurata);
        hDur->addWidget(new QLabel("s", this));
        formT->addRow("Durata:", hDur);

        formT->addRow("", editor->hasTestoCheck);
        formT->addRow("Testo:", editor->testoEdit);

        auto* partSec = new QVBoxLayout;
        partSec->addWidget(editor->partecipantiWidget);
        partSec->addWidget(editor->aggiungiPartecipanteBtn);
        formT->addRow("Partecipanti:", partSec);

        auto* gb = new QGroupBox("Traccia", this);
        gb->setLayout(formT);
        tracceLayout->addWidget(gb);

        tracceEditors.push_back(editor);
    });

    vTr->addWidget(tracceWidget);
    vTr->addWidget(aggiungiTracciaBtn);

    campiSpecificiLayout->addWidget(tracceBox);
}

void ProdottoFormBuilder::onTipoChanged(const QString& tipo)
{
    if (prodottoCorrente) 
        qDebug() << "FCKN strano -> " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "TIPO CHANGED  // c " << debug_counter++;
    // Rimuove TUTTO ciò che c’è nel layout dinamico (campi precedenti)
    QLayoutItem* item;
    while ((item = campiSpecificiLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    // Svincolo eventuali dati temporanei
    dateTourWidgets.clear();
    partecipantiSingoloLines.clear();
    tracceEditors.clear();

    {
        auto* box = new QGroupBox("Dati generali", this);
        auto* form = new QFormLayout(box);

        titoloEdit = new QLineEdit(this);
        form->addRow("Titolo:", titoloEdit);

        descrizioneEdit = new QTextEdit(this);
        descrizioneEdit->setFixedHeight(60);
        form->addRow("Descrizione:", descrizioneEdit);

        imagePathEdit = new QLineEdit(this);
        imagePathEdit->setReadOnly(true);
        auto* sfoglia = new QPushButton("Sfoglia...", this);
        connect(sfoglia, &QPushButton::clicked, this, &ProdottoFormBuilder::onSelezionaImmagine);
        auto* hImg = new QHBoxLayout;
        hImg->addWidget(imagePathEdit);
        hImg->addWidget(sfoglia);
        form->addRow("Immagine:", hImg);

        campiSpecificiLayout->addWidget(box);
    }

    if (tipo == "TShirt" || tipo == "CD" || tipo == "Vinile" || tipo == "Tour") {
        auto* box = new QGroupBox("Dati di vendita", this);
        auto* form = new QFormLayout(box);
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

        if (tipo == "TShirt" || tipo == "CD" || tipo == "Vinile") {
            codiceProdottoEdit = new QLineEdit(this);
            form->addRow("Codice prodotto:", codiceProdottoEdit);
        }

        campiSpecificiLayout->addWidget(box);
    }

    if (tipo == "TShirt") {
        buildCampoTShirt();    // verrà appeso sotto campiSpecificiLayout
    }
    else if (tipo == "CD") {
        buildCampoCD();
    }
    else if (tipo == "Vinile") {
        buildCampoVinile();
    }
    else if (tipo == "Tour") {
        buildCampoTour();
    }
    else if (tipo == "Singolo") {
        buildCampoSingolo();
    }
    else if (tipo == "Album") {
        buildCampoAlbum();
    }
}


void ProdottoFormBuilder::onSelezionaImmagine()
{
    if (prodottoCorrente) 
        qDebug() << "CAMBIO IMM PER: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "cambio immag  // c " << debug_counter++;


    QString f = QFileDialog::getOpenFileName(this, "Seleziona immagine", QString(),
                                             "Immagini (*.png *.jpg *.jpeg)");
    if (!f.isEmpty()) imagePathEdit->setText(f);
}

void ProdottoFormBuilder::popolaCampiDaProdotto()
{
    qDebug() << "Popola campi per : " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    
    if (!prodottoCorrente) return;
    titoloEdit->setText(QString::fromStdString(prodottoCorrente->getTitle()));
    descrizioneEdit->setPlainText(QString::fromStdString(prodottoCorrente->getDescription()));
    imagePathEdit->setText(QString::fromStdString(prodottoCorrente->getImagePath()));

    const_cast<ArtistProduct*>(prodottoCorrente)->accept(this);
}

void ProdottoFormBuilder::onConfermaSalvataggio()
{
    if (prodottoCorrente) 
        qDebug() << "CONF MODIFICHE PER : " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    else
        qDebug() << "CONF SALVATAGGIO  // c " << debug_counter++;

    const std::string titolo = titoloEdit->text().toStdString();
    if (titolo.empty()) {
        QMessageBox::warning(this, "Errore", "Il titolo non può essere vuoto.");
        return;
    }

    // Controllo unicità sanitizzata solo se inserimento
    if (!prodottoCorrente) {
        for (auto& kv : artisti[artistaSelezionato->getId()]->getProducts()) {
            if (DataManager::sanitizeForPath(kv.second->getTitle())
                == DataManager::sanitizeForPath(titolo))
            {
                QMessageBox::warning(this, "Errore", "Titolo già esistente per questo artista.");
                return;
            }
        }
    } else {
        // In modifica, se ho cambiato titolo, verifico collisione
        if (titolo != prodottoCorrente->getTitle()) {
            auto& vec = artisti[prodottoCorrente->getArtistId()]->getProducts();
            for (auto& kv : vec) {
                if (kv.second->getId() != prodottoCorrente->getId() &&
                    DataManager::sanitizeForPath(kv.second->getTitle())
                    == DataManager::sanitizeForPath(titolo))
                {
                    QMessageBox::warning(this, "Errore", "Titolo già esistente per questo artista.");
                    return;
                }
            }
        }
        qDebug() << "Verifica nome univoco fatta: " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    }

    const std::string descrizione = descrizioneEdit->toPlainText().toStdString();
    const std::string imagePath    = imagePathEdit->text().toStdString();

    if (!prodottoCorrente) {
        // === Fase INSERIMENTO ===
        const QString tipo = tipoComboBox->currentText();

        if (tipo == "TShirt") {
            double prezzo    = prezzoSpin->value();
            bool   dispon    = disponibileCheck->isChecked();
            unsigned int qtd = quantitaSpin->value();
            std::string codice  = codiceProdottoEdit->text().toStdString();
            std::string taglia  = tagliaEdit->text().toStdString();
            std::string colore  = coloreEdit->text().toStdString();

            auto* newT = new TShirt(
                artistaSelezionato,
                titolo, descrizione,
                prezzo, dispon, qtd,
                codice, taglia, colore
            );
            if(!imagePath.empty())
                newT->setImagePath(imagePath);
            prodotti[newT->getId()] = newT;
            artisti[artistaSelezionato->getId()]->addProduct(newT);
        }
        else if (tipo == "CD") {
            double prezzo    = prezzoSpin->value();
            bool   dispon    = disponibileCheck->isChecked();
            unsigned int qtd = quantitaSpin->value();
            std::string codice      = codiceProdottoEdit->text().toStdString();
            std::string producer    = produttoreStampeEdit->text().toStdString();
            std::string identicode  = codiceRiconoscimentoEdit->text().toStdString();
            std::string typeProd    = tipoProdottoEdit->text().toStdString();
            std::string formato     = formatoEdit->text().toStdString();

            auto* newCD = new CD(
                artistaSelezionato,
                titolo, descrizione,
                prezzo, dispon, qtd,
                codice, producer, identicode,
                typeProd, formato
            );
            if(!imagePath.empty())
                newCD->setImagePath(imagePath);
            prodotti[newCD->getId()] = newCD;
            artisti[artistaSelezionato->getId()]->addProduct(newCD);
        }
        else if (tipo == "Vinile") {
            double prezzo    = prezzoSpin->value();
            bool   dispon    = disponibileCheck->isChecked();
            unsigned int qtd = quantitaSpin->value();
            std::string codice      = codiceProdottoEdit->text().toStdString();
            std::string producer    = produttoreStampeEdit->text().toStdString();
            std::string identicode  = codiceRiconoscimentoEdit->text().toStdString();
            std::string typeProd    = tipoProdottoEdit->text().toStdString();
            unsigned int rpm        = rpmSpin->value();
            unsigned int diam       = diametroSpin->value();

            auto* newV = new Vinile(
                artistaSelezionato,
                titolo, descrizione,
                prezzo, dispon, qtd,
                codice, producer, identicode,
                typeProd, rpm, diam
            );
            if(!imagePath.empty())
                newV->setImagePath(imagePath);
            prodotti[newV->getId()] = newV;
            artisti[artistaSelezionato->getId()]->addProduct(newV);
        }
        else if (tipo == "Tour") {
            double prezzo    = prezzoSpin->value();
            bool   dispon    = disponibileCheck->isChecked();
            unsigned int qtd = quantitaSpin->value();

            auto* newT = new Tour(
                artistaSelezionato,
                titolo, descrizione,
                prezzo, dispon, qtd
            );
            if(!imagePath.empty())
                newT->setImagePath(imagePath);
            for (auto& w : dateTourWidgets) {
                QDate d = w.dataEdit->date();
                QTime t = w.orarioEdit->time();
                std::string luogo = w.luogoEdit->text().toStdString();
                newT->addDataTour(DataTour(
                    d.day(), d.month(), d.year(),
                    t.hour(), t.minute(), t.second(),
                    luogo
                ));
            }
            prodotti[newT->getId()] = newT;
            artisti[artistaSelezionato->getId()]->addProduct(newT);
        }
        else if (tipo == "Singolo") {
            QDate d = dataUscitaEdit->date();
            std::string genre = genereEdit->text().toStdString();
            std::string name  = nomeTracciaEdit->text().toStdString();
            if (name.empty()) {
                QMessageBox::warning(this, "Errore", "Nome traccia vuoto.");
                return;
            }
            bool hasTesto = hasTestoTracciaCheck->isChecked();
            std::string testo = testoTracciaEdit->toPlainText().toStdString();
            Durata durata(oreTraccia->value(), minutiTraccia->value(), secondiTraccia->value());

            std::vector<std::string> partecipanti;
            for (auto* line : partecipantiSingoloLines) {
                if (!line->text().isEmpty())
                    partecipanti.push_back(line->text().toStdString());
            }

            Traccia tr(name, partecipanti, durata, testo, hasTesto);
            bool remix = remixCheck->isChecked();
            int chart = chartPosSpin->value();

            auto* newS = new Singolo(
                artistaSelezionato,
                titolo, descrizione,
                Data(d.day(), d.month(), d.year()),
                Durata(0, 0, 0),
                genre, tr, remix, chart
            );
            newS->updateDurata();
            if(!imagePath.empty())
                newS->setImagePath(imagePath);
            prodotti[newS->getId()] = newS;
            artisti[artistaSelezionato->getId()]->addProduct(newS);
        }
        else if (tipo == "Album") {
            QDate d = dataUscitaEdit->date();
            std::string genre = genereEdit->text().toStdString();
            std::string label = labelEdit->text().toStdString();

            std::vector<Traccia> tracce;
            for (auto* ed : tracceEditors) {
                std::string name = ed->nomeEdit->text().toStdString();
                if (name.empty()) {
                    QMessageBox::warning(this, "Errore", "Ogni traccia deve avere un nome.");
                    return;
                }
                bool hasTesto = ed->hasTestoCheck->isChecked();
                std::string testo = ed->testoEdit->toPlainText().toStdString();
                Durata dur(ed->oreDurata->value(), ed->minutiDurata->value(), ed->secondiDurata->value());
                std::vector<std::string> par;
                for (auto* line : ed->partecipantiLines) {
                    if (!line->text().isEmpty())
                        par.push_back(line->text().toStdString());
                }
                tracce.emplace_back(name, par, dur, testo, hasTesto);
            }

            auto* newA = new Album(
                artistaSelezionato,
                titolo, descrizione,
                Data(d.day(), d.month(), d.year()),
                Durata(0, 0, 0),
                genre, tracce, label
            );
            newA->updateDurata();
            if(!imagePath.empty())
                newA->setImagePath(imagePath);
            prodotti[newA->getId()] = newA;
            artisti[artistaSelezionato->getId()]->addProduct(newA);
        }
    }
    else {
        // === Fase MODIFICA: campi già popolati, ora confronto e applico setter SOLO se necessari ===

        if (!imagePath.empty() && imagePath != prodottoCorrente->getImagePath())
            const_cast<ArtistProduct*>(prodottoCorrente)->setImagePath(imagePath);
        
        if (titolo != prodottoCorrente->getTitle())
            const_cast<ArtistProduct*>(prodottoCorrente)->setTitle(titolo);

        if (descrizione != prodottoCorrente->getDescription())
            const_cast<ArtistProduct*>(prodottoCorrente)->setDescription(descrizione);

        qDebug() << "Modifiche generali applicate per: " << titolo.c_str() << " // c " << debug_counter++;

        // In base al tipo concreto, chiamiamo direttamente il metodo di applicaModifiche
        if (auto* tsh = dynamic_cast<TShirt*>(const_cast<ArtistProduct*>(prodottoCorrente))) {
            applicaModificheTShirt(tsh);
        }
        else if (auto* cd = dynamic_cast<CD*>(const_cast<ArtistProduct*>(prodottoCorrente))) {
            applicaModificheCD(cd);
        }
        else if (auto* vin = dynamic_cast<Vinile*>(const_cast<ArtistProduct*>(prodottoCorrente))) {
            applicaModificheVinile(vin);
        }
        else if (auto* tour = dynamic_cast<Tour*>(const_cast<ArtistProduct*>(prodottoCorrente))) {
            applicaModificheTour(tour);
        }
        else if (auto* sing = dynamic_cast<Singolo*>(const_cast<ArtistProduct*>(prodottoCorrente))) {
            applicaModificheSingolo(sing);
        }
        else if (auto* alb = dynamic_cast<Album*>(const_cast<ArtistProduct*>(prodottoCorrente))) {
            applicaModificheAlbum(alb);
        }
    }

    emit prodottoSalvato();
    close();

    qDebug() << "Salvataggio completato per: " << titolo.c_str() << " // c " << debug_counter++;
}

// ===== VisitorInterfaceNotConst: popola campiSpecifici e applica modifiche =====

void ProdottoFormBuilder::visit(TShirt* tsh)
{
    if (!tagliaEdit) {
        buildCampoTShirt();
    }
    // Popolo con valori correnti
    prezzoSpin->setValue(tsh->getPrezzo());
    disponibileCheck->setChecked(tsh->getDisponibile());
    quantitaSpin->setValue(tsh->getQuantita());
    codiceProdottoEdit->setText(QString::fromStdString(tsh->getCodiceProdotto()));
    tagliaEdit->setText(QString::fromStdString(tsh->getTaglia()));
    coloreEdit->setText(QString::fromStdString(tsh->getColore()));

    // Applico modifiche se necessario
    applicaModificheTShirt(tsh);
}

void ProdottoFormBuilder::visit(CD* cd)
{
    if (!formatoEdit) {
        buildCampoCD();
    }
    prezzoSpin->setValue(cd->getPrezzo());
    disponibileCheck->setChecked(cd->getDisponibile());
    quantitaSpin->setValue(cd->getQuantita());
    codiceProdottoEdit->setText(QString::fromStdString(cd->getCodiceProdotto()));
    produttoreStampeEdit->setText(QString::fromStdString(cd->getProduttoreStampe()));
    codiceRiconoscimentoEdit->setText(QString::fromStdString(cd->getCodiceRiconoscimento()));
    tipoProdottoEdit->setText(QString::fromStdString(cd->getTipoProdotto()));
    formatoEdit->setText(QString::fromStdString(cd->getFormato()));

    applicaModificheCD(cd);
}

void ProdottoFormBuilder::visit(Vinile* vin)
{
    if (!rpmSpin) {
        buildCampoVinile();
    }
    prezzoSpin->setValue(vin->getPrezzo());
    disponibileCheck->setChecked(vin->getDisponibile());
    quantitaSpin->setValue(vin->getQuantita());
    codiceProdottoEdit->setText(QString::fromStdString(vin->getCodiceProdotto()));
    produttoreStampeEdit->setText(QString::fromStdString(vin->getProduttoreStampe()));
    codiceRiconoscimentoEdit->setText(QString::fromStdString(vin->getCodiceRiconoscimento()));
    tipoProdottoEdit->setText(QString::fromStdString(vin->getTipoProdotto()));
    rpmSpin->setValue(vin->getRpm());
    diametroSpin->setValue(vin->getDiametro());

    applicaModificheVinile(vin);
}

void ProdottoFormBuilder::visit(Tour* tour)
{
    if (!dateTourLayout) {
        buildCampoTour();
    }
    prezzoSpin->setValue(tour->getPrezzo());
    disponibileCheck->setChecked(tour->getDisponibile());
    quantitaSpin->setValue(tour->getQuantita());

    // Popolo le date esistenti
    for (auto& dt : tour->getDateTourModificabile()) {
        TourDataWidget newDate;
        newDate.dataEdit = new QDateEdit(this);
        newDate.dataEdit->setCalendarPopup(true);
        newDate.dataEdit->setDate(QDate(static_cast<Data>(dt).toQDate()));
        newDate.orarioEdit = new QTimeEdit(this);
        newDate.orarioEdit->setTime(QTime(dt.getOre(), dt.getMinuti(), dt.getSecondi()));
        newDate.luogoEdit = new QLineEdit(QString::fromStdString(dt.getLuogo()), this);

        auto* form = new QFormLayout;
        form->addRow("Data:", newDate.dataEdit);
        form->addRow("Orario:", newDate.orarioEdit);
        form->addRow("Luogo:", newDate.luogoEdit);

        auto* container = new QWidget(this);
        container->setLayout(form);
        dateTourLayout->addWidget(container);
        dateTourWidgets.push_back(newDate);
    }

    applicaModificheTour(tour);
}

void ProdottoFormBuilder::visit(Singolo* sing)
{
    if (!dataUscitaEdit) {
        buildCampoSingolo();
    }
    // Dati musicali
    const auto& rd = sing->getDataUscita();
    dataUscitaEdit->setDate(rd.toQDate());
    genereEdit->setText(QString::fromStdString(sing->getGenere()));

    const auto& tr = sing->getMainTrack();
    nomeTracciaEdit->setText(QString::fromStdString(tr.getNome()));
    const auto& d  = tr.getDurata();
    oreTraccia->setValue(d.getOre());
    minutiTraccia->setValue(d.getMinuti());
    secondiTraccia->setValue(d.getSecondi());
    hasTestoTracciaCheck->setChecked(tr.hasTestoPresent());
    testoTracciaEdit->setPlainText(QString::fromStdString(tr.getTesto()));

    for (auto& p : tr.getPartecipanti()) {
        auto* line = new QLineEdit(QString::fromStdString(p), this);
        partecipantiSingoloLayout->addWidget(line);
        partecipantiSingoloLines.push_back(line);
    }
    remixCheck->setChecked(sing->getIsRemix());
    chartPosSpin->setValue(sing->getChartPosition());

    applicaModificheSingolo(sing);
}

void ProdottoFormBuilder::visit(Album* alb)
{
    if (!dataUscitaEdit) {
        buildCampoAlbum();
    }

    const auto& rd = alb->getDataUscita();
    dataUscitaEdit->setDate(rd.toQDate());
    genereEdit->setText(QString::fromStdString(alb->getGenere()));
    labelEdit->setText(QString::fromStdString(alb->getLabel()));

    {
        // Svuota eventuali TracciaEditor rimasti
        for (auto* ed : tracceEditors) {
            delete ed;
        }
        tracceEditors.clear();

        // Rimuovi tutti i widget dal layout delle tracce
        QLayoutItem* child;
        while ((child = tracceLayout->takeAt(0)) != nullptr) {
            if (child->widget()) {
                child->widget()->deleteLater();
            }
            delete child;
        }
    }

    // Ricrea un TracciaEditor per ciascuna traccia già presente nell’album
    for (auto& tr : alb->getTracce()) {
        auto* editor = new TracciaEditor;
        editor->nomeEdit = new QLineEdit(QString::fromStdString(tr.getNome()), this);

        editor->oreDurata    = new QSpinBox(this);
        editor->minutiDurata = new QSpinBox(this);
        editor->secondiDurata= new QSpinBox(this);
        editor->oreDurata->setRange(0, 23);
        editor->minutiDurata->setRange(0, 59);
        editor->secondiDurata->setRange(0, 59);
        editor->oreDurata->setValue(tr.getDurata().getOre());
        editor->minutiDurata->setValue(tr.getDurata().getMinuti());
        editor->secondiDurata->setValue(tr.getDurata().getSecondi());

        editor->hasTestoCheck = new QCheckBox("Ha testo", this);
        editor->hasTestoCheck->setChecked(tr.hasTestoPresent());

        editor->testoEdit = new QTextEdit(QString::fromStdString(tr.getTesto()), this);
        editor->testoEdit->setFixedHeight(50);

        editor->partecipantiWidget = new QWidget(this);
        editor->partecipantiLayout = new QVBoxLayout(editor->partecipantiWidget);
        editor->aggiungiPartecipanteBtn = new QPushButton("Aggiungi partecipante", this);
        connect(editor->aggiungiPartecipanteBtn, &QPushButton::clicked, this, [this, editor]() {
            auto* line = new QLineEdit(this);
            editor->partecipantiLayout->addWidget(line);
            editor->partecipantiLines.push_back(line);
        });

        for (auto& p : tr.getPartecipanti()) {
            auto* line = new QLineEdit(QString::fromStdString(p), this);
            editor->partecipantiLayout->addWidget(line);
            editor->partecipantiLines.push_back(line);
        }

        auto* formT = new QFormLayout;
        formT->addRow("Nome:", editor->nomeEdit);

        auto* hDur = new QHBoxLayout;
        hDur->addWidget(editor->oreDurata);
        hDur->addWidget(new QLabel("h", this));
        hDur->addWidget(editor->minutiDurata);
        hDur->addWidget(new QLabel("m", this));
        hDur->addWidget(editor->secondiDurata);
        hDur->addWidget(new QLabel("s", this));
        formT->addRow("Durata:", hDur);

        formT->addRow("", editor->hasTestoCheck);
        formT->addRow("Testo:", editor->testoEdit);

        auto* partSec = new QVBoxLayout;
        partSec->addWidget(editor->partecipantiWidget);
        partSec->addWidget(editor->aggiungiPartecipanteBtn);
        formT->addRow("Partecipanti:", partSec);

        auto* gb = new QGroupBox("Traccia", this);
        gb->setLayout(formT);
        tracceLayout->addWidget(gb);

        tracceEditors.push_back(editor);
    }

    applicaModificheAlbum(alb);
}

// ======= Metodi di applicazione modifiche che confrontano e invocano setter solo se diverso =======
void ProdottoFormBuilder::applicaModificheTShirt(TShirt* tsh)
{
    qDebug() << "Applica Mod ts : " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;
    
    double newPrice = prezzoSpin->value();
    if (newPrice != tsh->getPrezzo()) tsh->setPrezzo(newPrice);

    bool newDisp = disponibileCheck->isChecked();
    if (newDisp != tsh->getDisponibile()) tsh->setDisponibile(newDisp);

    unsigned int newQtd = quantitaSpin->value();
    if (newQtd != tsh->getQuantita()) tsh->setQuantita(newQtd);

    std::string newCod = codiceProdottoEdit->text().toStdString();
    if (newCod != tsh->getCodiceProdotto()) tsh->setCodiceProdotto(newCod);

    std::string newSize = tagliaEdit->text().toStdString();
    if (newSize != tsh->getTaglia()) tsh->setTaglia(newSize);

    std::string newColor = coloreEdit->text().toStdString();
    if (newColor != tsh->getColore()) tsh->setColore(newColor);
}

void ProdottoFormBuilder::applicaModificheCD(CD* cd)
{
    qDebug() << "Applica Mod cd : " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;

    double newPrice = prezzoSpin->value();
    if (newPrice != cd->getPrezzo()) cd->setPrezzo(newPrice);

    bool newDisp = disponibileCheck->isChecked();
    if (newDisp != cd->getDisponibile()) cd->setDisponibile(newDisp);

    unsigned int newQtd = quantitaSpin->value();
    if (newQtd != cd->getQuantita()) cd->setQuantita(newQtd);

    std::string newCod = codiceProdottoEdit->text().toStdString();
    if (newCod != cd->getCodiceProdotto()) cd->setCodiceProdotto(newCod);

    std::string newProd = produttoreStampeEdit->text().toStdString();
    if (newProd != cd->getProduttoreStampe()) cd->setProduttoreStampe(newProd);

    std::string newId = codiceRiconoscimentoEdit->text().toStdString();
    if (newId != cd->getCodiceRiconoscimento()) cd->setCodiceRiconoscimento(newId);

    std::string newType = tipoProdottoEdit->text().toStdString();
    if (newType != cd->getTipoProdotto()) cd->setTipoProdotto(newType);

    std::string newFmt = formatoEdit->text().toStdString();
    if (newFmt != cd->getFormato()) cd->setFormato(newFmt);
}

void ProdottoFormBuilder::applicaModificheVinile(Vinile* vin)
{
    qDebug() << "Applica Mod vn : " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;

    double newPrice = prezzoSpin->value();
    if (newPrice != vin->getPrezzo()) vin->setPrezzo(newPrice);

    bool newDisp = disponibileCheck->isChecked();
    if (newDisp != vin->getDisponibile()) vin->setDisponibile(newDisp);

    unsigned int newQtd = quantitaSpin->value();
    if (newQtd != vin->getQuantita()) vin->setQuantita(newQtd);

    std::string newCod = codiceProdottoEdit->text().toStdString();
    if (newCod != vin->getCodiceProdotto()) vin->setCodiceProdotto(newCod);

    std::string newProd = produttoreStampeEdit->text().toStdString();
    if (newProd != vin->getProduttoreStampe()) vin->setProduttoreStampe(newProd);

    std::string newId = codiceRiconoscimentoEdit->text().toStdString();
    if (newId != vin->getCodiceRiconoscimento()) vin->setCodiceRiconoscimento(newId);

    std::string newType = tipoProdottoEdit->text().toStdString();
    if (newType != vin->getTipoProdotto()) vin->setTipoProdotto(newType);

    unsigned int newRpm = rpmSpin->value();
    if (newRpm != vin->getRpm()) vin->setRpm(newRpm);

    unsigned int newDia = diametroSpin->value();
    if (newDia != vin->getDiametro()) vin->setDiametro(newDia);
}

void ProdottoFormBuilder::applicaModificheTour(Tour* tour)
{
    qDebug() << "Applica Mod to : " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;

    double newPrice = prezzoSpin->value();
    if (newPrice != tour->getPrezzo()) tour->setPrezzo(newPrice);

    bool newDisp = disponibileCheck->isChecked();
    if (newDisp != tour->getDisponibile()) tour->setDisponibile(newDisp);

    unsigned int newQtd = quantitaSpin->value();
    if (newQtd != tour->getQuantita()) tour->setQuantita(newQtd);

    // Per le date, riallocazione completa
    tour->clearDateTour();
    for (auto& w : dateTourWidgets) {
        QDate d = w.dataEdit->date();
        QTime t = w.orarioEdit->time();
        std::string luogo = w.luogoEdit->text().toStdString();
        tour->addDataTour(DataTour(
            d.day(), d.month(), d.year(),
            t.hour(), t.minute(), t.second(),
            luogo
        ));
    }
}

void ProdottoFormBuilder::applicaModificheSingolo(Singolo* sing)
{
    qDebug() << "Applica Mod si : " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;

    QDate d = dataUscitaEdit->date();
    Data newRD(d.day(), d.month(), d.year());
    if (newRD != sing->getDataUscita()) sing->setDataUscita(newRD);

    std::string newGen = genereEdit->text().toStdString();
    if (newGen != sing->getGenere()) sing->setGenere(newGen);

    const auto& tr = sing->getMainTrack();
    std::string newName = nomeTracciaEdit->text().toStdString();
    if (newName != tr.getNome()) {
        bool hasTesto = hasTestoTracciaCheck->isChecked();
        std::string testo = testoTracciaEdit->toPlainText().toStdString();
        Durata dur(oreTraccia->value(), minutiTraccia->value(), secondiTraccia->value());
        std::vector<std::string> partecipanti;
        for (auto* line : partecipantiSingoloLines) {
            if (!line->text().isEmpty()) partecipanti.push_back(line->text().toStdString());
        }
        Traccia newTr(newName, partecipanti, dur, testo, hasTesto);
        sing->setMainTrack(newTr);
    } else {
        bool hasTesto = hasTestoTracciaCheck->isChecked();
        if (hasTesto != tr.hasTestoPresent()) {
            Traccia newTr = tr;
            newTr.setHasTesto(hasTesto);
            sing->setMainTrack(newTr);
        }
        std::string newT = testoTracciaEdit->toPlainText().toStdString();
        if (newT != tr.getTesto()) {
            Traccia newTr = tr;
            newTr.setTesto(newT);
            sing->setMainTrack(newTr);
        }
        Durata newDur(oreTraccia->value(), minutiTraccia->value(), secondiTraccia->value());
        if (newDur != tr.getDurata()) {
            Traccia newTr = tr;
            newTr.setDurata(newDur);
            sing->setMainTrack(newTr);
        }
        std::vector<std::string> newPart;
        for (auto* line : partecipantiSingoloLines) {
            if (!line->text().isEmpty()) newPart.push_back(line->text().toStdString());
        }
        if (newPart != tr.getPartecipanti()) {
            Traccia newTr = tr;
            newTr.setPartecipanti(newPart);
            sing->setMainTrack(newTr);
        }
    }

    bool newRemix = remixCheck->isChecked();
    if (newRemix != sing->getIsRemix()) sing->setIsRemix(newRemix);

    int newChart = chartPosSpin->value();
    if (newChart != sing->getChartPosition()) sing->setChartPosition(newChart);

    sing->updateDurata();
}

void ProdottoFormBuilder::applicaModificheAlbum(Album* alb)
{
    qDebug() << "Applica Mod al : " << prodottoCorrente->getTitle().c_str() << " // c " << debug_counter++;

    QDate d = dataUscitaEdit->date();
    Data newRD(d.day(), d.month(), d.year());
    if (newRD != alb->getDataUscita()) alb->setDataUscita(newRD);

    std::string newGen = genereEdit->text().toStdString();
    if (newGen != alb->getGenere()) alb->setGenere(newGen);

    std::string newLabel = labelEdit->text().toStdString();
    if (newLabel != alb->getLabel()) alb->setLabel(newLabel);

    std::vector<Traccia> newTracce;
    for (auto* ed : tracceEditors) {
        std::string name = ed->nomeEdit->text().toStdString();
        if(!name.empty()){
            bool hasTesto = ed->hasTestoCheck->isChecked();
            std::string testo = ed->testoEdit->toPlainText().toStdString();
            Durata dur(ed->oreDurata->value(), ed->minutiDurata->value(), ed->secondiDurata->value());
            std::vector<std::string> par;
            for (auto* line : ed->partecipantiLines) {
                if (!line->text().isEmpty())
                    par.push_back(line->text().toStdString());
            }
            newTracce.emplace_back(name, par, dur, testo, hasTesto);
        }
    }
    qDebug() << "Tracce trovate: " << newTracce.size() << " // c " << debug_counter++;
    if (newTracce != alb->getTracce()) {
        alb->setTracce(newTracce);
        alb->updateDurata();
    }
}

// ===============================================
// Implementazioni mancanti di VisitorInterfaceNotConst
// ===============================================

void ProdottoFormBuilder::visit(DataTour* /*dt*/)
{
    // l'ho implementato in visit(Tour*)
}

void ProdottoFormBuilder::visit(Traccia* /*tr*/)
{
    // l'ho implementato in visit(Singolo*) e visit(Album*)
}
