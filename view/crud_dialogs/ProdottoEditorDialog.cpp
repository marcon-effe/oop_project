#include "ProdottoEditorDialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QGuiApplication>
#include <QScreen>
#include <QLabel>
#include <QFormLayout>

ProdottoEditorDialog::ProdottoEditorDialog(
    ArtistProduct* prodottoOriginale,
    std::unordered_map<unsigned int, Artista*>& artistiRef,
    std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
    QWidget* parent
) : QDialog(parent),
    prodotto(prodottoOriginale),
    artistaSelezionato(nullptr),
    artisti(artistiRef),
    prodotti(prodottiRef)
{
    if (prodottoOriginale) {
        auto it = artisti.find(prodottoOriginale->getArtistId());
        if (it != artisti.end())
            artistaSelezionato = it->second;
    }

    setWindowTitle("Modifica prodotto");
    setMinimumWidth(500);
    setupUI();
}

void ProdottoEditorDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Scroll area per i contenuti
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    QWidget* scrollContent = new QWidget(this);
    scrollLayout = new QVBoxLayout(scrollContent);

    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    // Contenitore per i campi specifici
    campiSpecificiContainer = new QWidget;
    campiSpecificiLayout = new QVBoxLayout(campiSpecificiContainer);
    scrollLayout->addWidget(campiSpecificiContainer);

    // Costruzione UI specifica per tipo
    if (dynamic_cast<TShirt*>(prodotto)) buildTShirt();
    else if (dynamic_cast<CD*>(prodotto)) buildCD();
    else if (dynamic_cast<Vinile*>(prodotto)) buildVinile();
    else if (dynamic_cast<Tour*>(prodotto)) buildTour();
    else if (dynamic_cast<Singolo*>(prodotto)) buildSingolo();
    else if (dynamic_cast<Album*>(prodotto)) buildAlbum();

    // Pulsanti di conferma/annulla
    QDialogButtonBox* btnBox = new QDialogButtonBox(Qt::Horizontal, this);
    QPushButton* annullaBtn = btnBox->addButton(QDialogButtonBox::Cancel);
    QPushButton* confermaBtn = btnBox->addButton("Salva modifiche", QDialogButtonBox::AcceptRole);

    btnBox->setCenterButtons(false);
    btnBox->setContentsMargins(0, 10, 0, 0);

    connect(annullaBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(confermaBtn, &QPushButton::clicked, this, &ProdottoEditorDialog::confermaModifica);

    scrollLayout->addWidget(btnBox);

    // Posizionamento e dimensione
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QSize screenSize = screen->availableGeometry().size();
        int w = static_cast<int>(screenSize.width() * 0.3);
        int h = static_cast<int>(screenSize.height() * 0.95);
        resize(w, h);
        move((screenSize.width() - w) / 2, 5);
    }
}

void ProdottoEditorDialog::selezionaImmagine() {
    QString filePath = QFileDialog::getOpenFileName(this, "Seleziona immagine", QString(),
                                                    "Immagini (*.png *.jpg *.jpeg)");
    if (!filePath.isEmpty()) {
        imagePathEdit->setText(filePath);
    }
}

void ProdottoEditorDialog::buildArtistProduct() {
    QGroupBox* box = new QGroupBox("Dati generali");
    QFormLayout* form = new QFormLayout;

    titoloEdit = new QLineEdit(this);
    titoloEdit->setText(QString::fromStdString(prodotto->getTitle()));
    form->addRow("Titolo:", titoloEdit);

    descrizioneEdit = new QTextEdit(this);
    descrizioneEdit->setPlainText(QString::fromStdString(prodotto->getDescription()));
    descrizioneEdit->setFixedHeight(60);
    form->addRow("Descrizione:", descrizioneEdit);

    imagePathEdit = new QLineEdit(this);
    imagePathEdit->setReadOnly(true);
    imagePathEdit->setText(QString::fromStdString(prodotto->getImagePath()));

    QPushButton* sfogliaBtn = new QPushButton("Sfoglia...", this);
    connect(sfogliaBtn, &QPushButton::clicked, this, &ProdottoEditorDialog::selezionaImmagine);

    QHBoxLayout* imgLayout = new QHBoxLayout;
    imgLayout->addWidget(imagePathEdit);
    imgLayout->addWidget(sfogliaBtn);
    form->addRow("Immagine:", imgLayout);

    box->setLayout(form);
    campiSpecificiLayout->addWidget(box);
}

void ProdottoEditorDialog::buildMusica() {
    buildArtistProduct();

    Musica* musica = dynamic_cast<Musica*>(prodotto);
    if (!musica) return;

    QGroupBox* box = new QGroupBox("Dati musicali");
    QFormLayout* form = new QFormLayout;

    dataUscitaEdit = new QDateEdit(this);
    dataUscitaEdit->setCalendarPopup(true);
    dataUscitaEdit->setDisplayFormat("dd/MM/yyyy");

    const Data& data = musica->getDataUscita();
    dataUscitaEdit->setDate(QDate(data.getAnno(), data.getMese(), data.getGiorno()));
    form->addRow("Data di uscita:", dataUscitaEdit);

    genereEdit = new QLineEdit(this);
    genereEdit->setText(QString::fromStdString(musica->getGenere()));
    form->addRow("Genere:", genereEdit);

    box->setLayout(form);
    campiSpecificiLayout->addWidget(box);
}

void ProdottoEditorDialog::buildNotMusica() {
    buildArtistProduct();

    NotMusica* notMusica = dynamic_cast<NotMusica*>(prodotto);
    if (!notMusica) return;

    QGroupBox* box = new QGroupBox("Dati di vendita");
    QFormLayout* form = new QFormLayout;

    prezzoSpin = new QDoubleSpinBox(this);
    prezzoSpin->setRange(0, 10000);
    prezzoSpin->setDecimals(2);
    prezzoSpin->setPrefix("€ ");
    prezzoSpin->setValue(notMusica->getPrezzo());
    form->addRow("Prezzo:", prezzoSpin);

    disponibileCheck = new QCheckBox("Disponibile", this);
    disponibileCheck->setChecked(notMusica->getDisponibile());
    form->addRow("", disponibileCheck);

    quantitaSpin = new QSpinBox(this);
    quantitaSpin->setRange(0, 100000);
    quantitaSpin->setValue(static_cast<int>(notMusica->getQuantita()));
    form->addRow("Quantità:", quantitaSpin);

    box->setLayout(form);
    campiSpecificiLayout->addWidget(box);
}

void ProdottoEditorDialog::buildMerch() {
    buildNotMusica();

    Merch* merch = dynamic_cast<Merch*>(prodotto);
    if (!merch) return;

    QGroupBox* merchBox = new QGroupBox("Informazioni di Merchandising");
    QFormLayout* merchLayout = new QFormLayout;

    codiceProdottoEdit = new QLineEdit(this);
    codiceProdottoEdit->setText(QString::fromStdString(merch->getCodiceProdotto()));
    merchLayout->addRow("Codice prodotto:", codiceProdottoEdit);

    merchBox->setLayout(merchLayout);
    campiSpecificiLayout->addWidget(merchBox);
}

void ProdottoEditorDialog::buildDisco() {
    Disco* disco = dynamic_cast<Disco*>(prodotto);
    if (!disco) return;

    buildMerch();  // include ArtistProduct, NotMusica, Merch

    QGroupBox* discoBox = new QGroupBox("Dettagli del supporto fisico");
    QFormLayout* discoLayout = new QFormLayout;

    produttoreStampeEdit = new QLineEdit(this);
    produttoreStampeEdit->setText(QString::fromStdString(disco->getProduttoreStampe()));
    discoLayout->addRow("Produttore stampe:", produttoreStampeEdit);

    codiceRiconoscimentoEdit = new QLineEdit(this);
    codiceRiconoscimentoEdit->setText(QString::fromStdString(disco->getCodiceRiconoscimento()));
    discoLayout->addRow("Codice riconoscimento:", codiceRiconoscimentoEdit);

    tipoProdottoEdit = new QLineEdit(this);
    tipoProdottoEdit->setText(QString::fromStdString(disco->getTipoProdotto()));
    discoLayout->addRow("Tipo prodotto:", tipoProdottoEdit);

    discoBox->setLayout(discoLayout);
    campiSpecificiLayout->addWidget(discoBox);
}


void ProdottoEditorDialog::buildTShirt() {
    TShirt* tshirt = dynamic_cast<TShirt*>(prodotto);
    if (!tshirt) return;

    buildMerch();  // include tutti i campi da ArtistProduct fino a Merch

    QGroupBox* tshirtBox = new QGroupBox("Dettagli T-Shirt");
    QFormLayout* tshirtLayout = new QFormLayout;

    tagliaEdit = new QLineEdit(this);
    tagliaEdit->setText(QString::fromStdString(tshirt->getTaglia()));
    tshirtLayout->addRow("Taglia:", tagliaEdit);

    coloreEdit = new QLineEdit(this);
    coloreEdit->setText(QString::fromStdString(tshirt->getColore()));
    tshirtLayout->addRow("Colore:", coloreEdit);

    tshirtBox->setLayout(tshirtLayout);
    campiSpecificiLayout->addWidget(tshirtBox);
}

void ProdottoEditorDialog::buildTour() {
    Tour* tour = dynamic_cast<Tour*>(prodotto);
    if (!tour) return;

    buildMerch();  // include ArtistProduct, NotMusica, Merch

    QGroupBox* tourBox = new QGroupBox("Date del Tour");
    QVBoxLayout* tourMainLayout = new QVBoxLayout;

    dateTourWidget = new QWidget(this);
    dateTourLayout = new QVBoxLayout(dateTourWidget);

    // Precarica le date esistenti
    const std::vector<DataTour>& date = tour->getDateTour();
    for (const auto& dt : date) {
        TourDataWidget newDate;

        newDate.dataEdit = new QDateEdit(this);
        newDate.dataEdit->setCalendarPopup(true);
        newDate.dataEdit->setDate(QDate(dt.getAnno(), dt.getMese(), dt.getGiorno()));

        newDate.orarioEdit = new QTimeEdit(this);
        newDate.orarioEdit->setTime(QTime(dt.getOrario().getOre(), dt.getOrario().getMinuti(), dt.getOrario().getSecondi()));

        newDate.luogoEdit = new QLineEdit(this);
        newDate.luogoEdit->setText(QString::fromStdString(dt.getLuogo()));

        QFormLayout* dataLayout = new QFormLayout;
        dataLayout->addRow("Data:", newDate.dataEdit);
        dataLayout->addRow("Orario:", newDate.orarioEdit);
        dataLayout->addRow("Luogo:", newDate.luogoEdit);

        QWidget* dataGroup = new QWidget(this);
        dataGroup->setLayout(dataLayout);

        dateTourLayout->addWidget(dataGroup);
        dateTourWidgets.push_back(newDate);
    }

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

void ProdottoEditorDialog::buildCD() {
    CD* cd = dynamic_cast<CD*>(prodotto);
    if (!cd) return;

    buildDisco();  // include anche tutti i campi base

    QGroupBox* cdBox = new QGroupBox("Dettagli CD");
    QFormLayout* cdLayout = new QFormLayout;

    formatoEdit = new QLineEdit(this);
    formatoEdit->setText(QString::fromStdString(cd->getFormato()));
    cdLayout->addRow("Formato:", formatoEdit);

    cdBox->setLayout(cdLayout);
    campiSpecificiLayout->addWidget(cdBox);
}

void ProdottoEditorDialog::buildVinile() {
    Vinile* vinile = dynamic_cast<Vinile*>(prodotto);
    if (!vinile) return;

    buildDisco();  // include buildMerch() → buildNotMusica() → buildArtistProduct()

    QGroupBox* vinileBox = new QGroupBox("Dettagli Vinile");
    QFormLayout* vinileLayout = new QFormLayout;

    rpmSpin = new QSpinBox(this);
    rpmSpin->setRange(1, 10000);
    rpmSpin->setValue(static_cast<int>(vinile->getRpm()));
    vinileLayout->addRow("RPM:", rpmSpin);

    diametroSpin = new QSpinBox(this);
    diametroSpin->setRange(1, 100);
    diametroSpin->setValue(static_cast<int>(vinile->getDiametro()));
    vinileLayout->addRow("Diametro:", diametroSpin);

    vinileBox->setLayout(vinileLayout);
    campiSpecificiLayout->addWidget(vinileBox);
}

void ProdottoEditorDialog::buildSingolo() {
    Singolo* singolo = dynamic_cast<Singolo*>(prodotto);
    if (!singolo) return;

    buildMusica();  // include buildArtistProduct()

    QGroupBox* singoloBox = new QGroupBox("Traccia principale");
    QFormLayout* singoloLayout = new QFormLayout;

    const Traccia& traccia = singolo->getMainTrack();

    nomeTracciaEdit = new QLineEdit(this);
    nomeTracciaEdit->setText(QString::fromStdString(traccia.getNome()));
    singoloLayout->addRow("Nome traccia:", nomeTracciaEdit);

    oreTraccia = new QSpinBox(this);
    minutiTraccia = new QSpinBox(this);
    secondiTraccia = new QSpinBox(this);
    oreTraccia->setRange(0, 23);
    minutiTraccia->setRange(0, 59);
    secondiTraccia->setRange(0, 59);

    const Durata& d = traccia.getDurata();
    oreTraccia->setValue(d.getOre());
    minutiTraccia->setValue(d.getMinuti());
    secondiTraccia->setValue(d.getSecondi());

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
    hasTestoTracciaCheck->setChecked(traccia.hasTestoPresent());
    singoloLayout->addRow("", hasTestoTracciaCheck);

    testoTracciaEdit = new QTextEdit(this);
    testoTracciaEdit->setPlainText(QString::fromStdString(traccia.getTesto()));
    testoTracciaEdit->setFixedHeight(60);
    singoloLayout->addRow("Testo:", testoTracciaEdit);

    partecipantiSingoloWidget = new QWidget(this);
    partecipantiSingoloLayout = new QVBoxLayout(partecipantiSingoloWidget);

    const std::vector<std::string>& partecipanti = traccia.getPartecipanti();
    for (const std::string& p : partecipanti) {
        QLineEdit* line = new QLineEdit(this);
        line->setText(QString::fromStdString(p));
        partecipantiSingoloLayout->addWidget(line);
        partecipantiSingoloLines.push_back(line);
    }

    aggiungiPartecipanteSingoloBtn = new QPushButton("Aggiungi partecipante", this);
    connect(aggiungiPartecipanteSingoloBtn, &QPushButton::clicked, this, [this]() {
        QLineEdit* nuovo = new QLineEdit(this);
        partecipantiSingoloLayout->addWidget(nuovo);
        partecipantiSingoloLines.push_back(nuovo);
    });

    QVBoxLayout* partecipantiSection = new QVBoxLayout;
    partecipantiSection->addWidget(partecipantiSingoloWidget);
    partecipantiSection->addWidget(aggiungiPartecipanteSingoloBtn);

    QWidget* partecipantiWrapper = new QWidget(this);
    partecipantiWrapper->setLayout(partecipantiSection);
    singoloLayout->addRow("Partecipanti:", partecipantiWrapper);

    remixCheck = new QCheckBox("È un remix", this);
    remixCheck->setChecked(singolo->getIsRemix());
    singoloLayout->addRow("", remixCheck);

    chartPosSpin = new QSpinBox(this);
    chartPosSpin->setRange(0, 1000);
    chartPosSpin->setValue(singolo->getChartPosition());
    singoloLayout->addRow("Posizione in classifica:", chartPosSpin);

    singoloBox->setLayout(singoloLayout);
    campiSpecificiLayout->addWidget(singoloBox);
}

void ProdottoEditorDialog::buildAlbum() {
    Album* album = dynamic_cast<Album*>(prodotto);
    if (!album) return;

    buildMusica();  // include buildArtistProduct()

    // BOX: label discografica
    QGroupBox* albumBox = new QGroupBox("Dettagli Album");
    QFormLayout* albumLayout = new QFormLayout;

    labelEdit = new QLineEdit(this);
    labelEdit->setText(QString::fromStdString(album->getLabel()));
    albumLayout->addRow("Label discografica:", labelEdit);

    albumBox->setLayout(albumLayout);
    campiSpecificiLayout->addWidget(albumBox);

    // BOX: elenco tracce
    QGroupBox* tracceBox = new QGroupBox("Tracce");
    QVBoxLayout* tracceBoxLayout = new QVBoxLayout(tracceBox);

    tracceWidget = new QWidget(this);
    tracceLayout = new QVBoxLayout(tracceWidget);

    const std::vector<Traccia>& tracce = album->getTracce();
    for (const Traccia& t : tracce) {
        TracciaEditor* editor = new TracciaEditor;

        editor->nomeEdit = new QLineEdit(this);
        editor->nomeEdit->setText(QString::fromStdString(t.getNome()));

        editor->oreDurata = new QSpinBox(this);
        editor->minutiDurata = new QSpinBox(this);
        editor->secondiDurata = new QSpinBox(this);
        editor->oreDurata->setRange(0, 23);
        editor->minutiDurata->setRange(0, 59);
        editor->secondiDurata->setRange(0, 59);

        const Durata& durata = t.getDurata();
        editor->oreDurata->setValue(durata.getOre());
        editor->minutiDurata->setValue(durata.getMinuti());
        editor->secondiDurata->setValue(durata.getSecondi());

        editor->hasTestoCheck = new QCheckBox("Ha testo", this);
        editor->hasTestoCheck->setChecked(t.hasTestoPresent());

        editor->testoEdit = new QTextEdit(this);
        editor->testoEdit->setPlainText(QString::fromStdString(t.getTesto()));
        editor->testoEdit->setFixedHeight(50);

        // Partecipanti
        editor->partecipantiWidget = new QWidget(this);
        editor->partecipantiLayout = new QVBoxLayout(editor->partecipantiWidget);
        for (const std::string& p : t.getPartecipanti()) {
            QLineEdit* line = new QLineEdit(this);
            line->setText(QString::fromStdString(p));
            editor->partecipantiLayout->addWidget(line);
            editor->partecipantiLines.push_back(line);
        }

        editor->aggiungiPartecipanteBtn = new QPushButton("Aggiungi partecipante", this);
        connect(editor->aggiungiPartecipanteBtn, &QPushButton::clicked, this, [this, editor]() {
            QLineEdit* nuovo = new QLineEdit(this);
            editor->partecipantiLayout->addWidget(nuovo);
            editor->partecipantiLines.push_back(nuovo);
        });

        // Layout traccia
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
    }

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

void ProdottoEditorDialog::confermaModifica() {
    if (!artistaSelezionato || !prodotto) {
        QMessageBox::warning(this, "Errore", "Prodotto o artista non valido.");
        return;
    }

    // Validazione titolo: non vuoto
    std::string nuovoTitolo = titoloEdit->text().toStdString();
    if (nuovoTitolo.empty()) {
        QMessageBox::warning(this, "Errore", "Il titolo non può essere vuoto.");
        return;
    }

    // Verifica unicità del titolo (confronto sanitized, escludendo se stesso)
    for (const auto& pair : artistaSelezionato->getProducts()) {
        if (pair.second != prodotto &&
            DataManager::sanitizeForPath(pair.second->getTitle()) == DataManager::sanitizeForPath(nuovoTitolo)) {
            QMessageBox::warning(this, "Titolo duplicato",
                                 "Esiste già un altro prodotto con questo titolo (dopo sanitizzazione).");
            return;
        }
    }

    // Campi comuni
    if (nuovoTitolo != prodotto->getTitle())
        prodotto->setTitle(nuovoTitolo);

    std::string nuovaDescrizione = descrizioneEdit->toPlainText().toStdString();
    if (nuovaDescrizione != prodotto->getDescription())
        prodotto->setDescription(nuovaDescrizione);

    std::string nuovoImagePath = imagePathEdit->text().toStdString();
    if (nuovoImagePath != prodotto->getImagePath())
        prodotto->setImagePath(nuovoImagePath);

    // Dispatch dinamico per modifiche specifiche
    if (auto tshirt = dynamic_cast<TShirt*>(prodotto)) {
        // Campi da NotMusica
        double prezzo = prezzoSpin->value();
        if (prezzo != tshirt->getPrezzo())
            tshirt->setPrezzo(prezzo);

        bool disponibile = disponibileCheck->isChecked();
        if (disponibile != tshirt->getDisponibile())
            tshirt->setDisponibile(disponibile);

        unsigned int quantita = quantitaSpin->value();
        if (quantita != tshirt->getQuantita())
            tshirt->setQuantita(quantita);

        // Campi da Merch
        std::string codice = codiceProdottoEdit->text().toStdString();
        if (codice != tshirt->getCodiceProdotto())
            tshirt->setCodiceProdotto(codice);

        // Campi specifici TShirt
        std::string taglia = tagliaEdit->text().toStdString();
        if (taglia != tshirt->getTaglia())
            tshirt->setTaglia(taglia);

        std::string colore = coloreEdit->text().toStdString();
        if (colore != tshirt->getColore())
            tshirt->setColore(colore);
    }
    else if (auto tour = dynamic_cast<Tour*>(prodotto)) {
        // NotMusica
        double prezzo = prezzoSpin->value();
        if (prezzo != tour->getPrezzo())
            tour->setPrezzo(prezzo);

        bool disponibile = disponibileCheck->isChecked();
        if (disponibile != tour->getDisponibile())
            tour->setDisponibile(disponibile);

        unsigned int quantita = quantitaSpin->value();
        if (quantita != tour->getQuantita())
            tour->setQuantita(quantita);

        // Ricostruzione nuova lista date
        std::vector<DataTour> nuoveDate;
        for (const auto& w : dateTourWidgets) {
            QDate d = w.dataEdit->date();
            QTime t = w.orarioEdit->time();
            std::string luogo = w.luogoEdit->text().toStdString();
            nuoveDate.emplace_back(
                d.day(), d.month(), d.year(),
                t.hour(), t.minute(), t.second(),
                luogo
            );
        }

        // Confronto con la lista originale
        const std::vector<DataTour>& originali = tour->getDateTour();
        bool dateModificate = (nuoveDate.size() != originali.size());

        if (!dateModificate) {
            for (size_t i = 0; i < nuoveDate.size(); ++i) {
                if (!(nuoveDate[i] == originali[i])) {
                    dateModificate = true;
                    break;
                }
            }
        }

        if (dateModificate) {
            tour->setDateTour(nuoveDate);  // usa il metodo nuovo
        }
    }
    else if (auto cd = dynamic_cast<CD*>(prodotto)) {
        // NotMusica
        double prezzo = prezzoSpin->value();
        if (prezzo != cd->getPrezzo())
            cd->setPrezzo(prezzo);

        bool disponibile = disponibileCheck->isChecked();
        if (disponibile != cd->getDisponibile())
            cd->setDisponibile(disponibile);

        unsigned int quantita = quantitaSpin->value();
        if (quantita != cd->getQuantita())
            cd->setQuantita(quantita);

        // Merch
        std::string codice = codiceProdottoEdit->text().toStdString();
        if (codice != cd->getCodiceProdotto())
            cd->setCodiceProdotto(codice);

        // Disco
        std::string produttore = produttoreStampeEdit->text().toStdString();
        if (produttore != cd->getProduttoreStampe())
            cd->setProduttoreStampe(produttore);

        std::string riconoscimento = codiceRiconoscimentoEdit->text().toStdString();
        if (riconoscimento != cd->getCodiceRiconoscimento())
            cd->setCodiceRiconoscimento(riconoscimento);

        std::string tipoProd = tipoProdottoEdit->text().toStdString();
        if (tipoProd != cd->getTipoProdotto())
            cd->setTipoProdotto(tipoProd);

        // CD
        std::string formato = formatoEdit->text().toStdString();
        if (formato != cd->getFormato())
            cd->setFormato(formato);
    }
    else if (auto vinile = dynamic_cast<Vinile*>(prodotto)) {
        // NotMusica
        double prezzo = prezzoSpin->value();
        if (prezzo != vinile->getPrezzo())
            vinile->setPrezzo(prezzo);

        bool disponibile = disponibileCheck->isChecked();
        if (disponibile != vinile->getDisponibile())
            vinile->setDisponibile(disponibile);

        unsigned int quantita = quantitaSpin->value();
        if (quantita != vinile->getQuantita())
            vinile->setQuantita(quantita);

        // Merch
        std::string codice = codiceProdottoEdit->text().toStdString();
        if (codice != vinile->getCodiceProdotto())
            vinile->setCodiceProdotto(codice);

        // Disco
        std::string produttore = produttoreStampeEdit->text().toStdString();
        if (produttore != vinile->getProduttoreStampe())
            vinile->setProduttoreStampe(produttore);

        std::string riconoscimento = codiceRiconoscimentoEdit->text().toStdString();
        if (riconoscimento != vinile->getCodiceRiconoscimento())
            vinile->setCodiceRiconoscimento(riconoscimento);

        std::string tipoProd = tipoProdottoEdit->text().toStdString();
        if (tipoProd != vinile->getTipoProdotto())
            vinile->setTipoProdotto(tipoProd);

        // Vinile
        unsigned int rpm = rpmSpin->value();
        if (rpm != vinile->getRpm())
            vinile->setRpm(rpm);

        unsigned int diametro = diametroSpin->value();
        if (diametro != vinile->getDiametro())
            vinile->setDiametro(diametro);
    }
    else if (auto singolo = dynamic_cast<Singolo*>(prodotto)) {
        // Musica
        QDate d = dataUscitaEdit->date();
        Data nuovaData(d.day(), d.month(), d.year());
        if (nuovaData != singolo->getDataUscita())
            singolo->setDataUscita(nuovaData);

        std::string genere = genereEdit->text().toStdString();
        if (genere != singolo->getGenere())
            singolo->setGenere(genere);

        // Traccia
        std::string nome = nomeTracciaEdit->text().toStdString();
        if (nome.empty()) {
            QMessageBox::warning(this, "Errore", "Il nome della traccia principale non può essere vuoto.");
            return;
        }
        bool hasTesto = hasTestoTracciaCheck->isChecked();
        std::string testo = testoTracciaEdit->toPlainText().toStdString();
        Durata durata(oreTraccia->value(), minutiTraccia->value(), secondiTraccia->value());

        std::vector<std::string> partecipanti;
        for (QLineEdit* p : partecipantiSingoloLines) {
            if (!p->text().isEmpty())
                partecipanti.push_back(p->text().toStdString());
        }

        const Traccia& t = singolo->getMainTrack();
        bool tracciaDiversa =
            t.getNome() != nome ||
            t.getTesto() != testo ||
            t.hasTestoPresent() != hasTesto ||
            t.getDurata() != durata ||
            t.getPartecipanti() != partecipanti;

        if (tracciaDiversa) {
            Traccia nuovaTraccia(nome, partecipanti, durata, testo, hasTesto);
            singolo->setMainTrack(nuovaTraccia);
        }

        // Remix e Chart Position
        bool remix = remixCheck->isChecked();
        if (remix != singolo->getIsRemix())
            singolo->setIsRemix(remix);

        int chart = chartPosSpin->value();
        if (chart != singolo->getChartPosition())
            singolo->setChartPosition(chart);

        singolo->updateDurata();
    }
    else if (auto album = dynamic_cast<Album*>(prodotto)) {
        // Musica
        QDate d = dataUscitaEdit->date();
        Data nuovaData(d.day(), d.month(), d.year());
        if (nuovaData != album->getDataUscita())
            album->setDataUscita(nuovaData);

        std::string genere = genereEdit->text().toStdString();
        if (genere != album->getGenere())
            album->setGenere(genere);

        std::string label = labelEdit->text().toStdString();
        if (label != album->getLabel())
            album->setLabel(label);

        // Ricostruzione nuove tracce
        std::vector<Traccia> nuoveTracce;
        for (TracciaEditor* ed : tracceEditors) {
            std::string nome = ed->nomeEdit->text().toStdString();
            if (nome.empty())
                continue; // traccia considerata eliminata

            Durata durata(ed->oreDurata->value(), ed->minutiDurata->value(), ed->secondiDurata->value());
            std::string testo = ed->testoEdit->toPlainText().toStdString();
            bool hasTesto = ed->hasTestoCheck->isChecked();

            std::vector<std::string> partecipanti;
            for(QLineEdit* p : ed->partecipantiLines){
                if(!p->text().isEmpty())
                    partecipanti.push_back(p->text().toStdString());
            }

            nuoveTracce.emplace_back(nome, partecipanti, durata, testo, hasTesto);
        }

        if (nuoveTracce.empty()) {
            QMessageBox::warning(this, "Errore", "Un album deve contenere almeno una traccia con titolo.");
            return;
        }

        // Confronto profondo tracce
        const std::vector<Traccia>& originali = album->getTracce();
        bool tracceModificate = nuoveTracce.size() != originali.size();

        if (!tracceModificate) {
            for (size_t i = 0; i < nuoveTracce.size(); ++i) {
                if (!(nuoveTracce[i] == originali[i])) {
                    tracceModificate = true;
                    break;
                }
            }
        }

        if (tracceModificate) {
            album->setTracce(nuoveTracce);
            album->updateDurata();
        }
    }

    accept();
}