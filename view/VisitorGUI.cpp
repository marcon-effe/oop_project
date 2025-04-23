#include "VisitorGUI.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QGroupBox>
#include <QImage>
#include <QDir>
#include <QCoreApplication>
#include <QListWidget>
#include <QListWidgetItem>
#include <QAbstractItemView>
#include <QScrollArea>
#include <QVariant>
#include <QObject>
#include <QPushButton>
#include <QFile>
#include <QSize>
#include <QPen>

#include <QDebug>

#include "../model/core/ArtistProduct.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/merch/TShirt.h"
#include "../model/tour/Tour.h"
#include "../model/artisti/Artista.h"

VisitorGUI::VisitorGUI(QObject* parent)
    : QObject(parent),
      widget(new QWidget()),
      layout(new QVBoxLayout(widget))
{
    widget->setLayout(layout);
}

QWidget* VisitorGUI::getWidget() const {
    return widget;
}

void VisitorGUI::clearLayout() {
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }
}

QLabel* VisitorGUI::createImageLabel(const std::string& imagePathStr, bool isArtist) const {
    QString imagePath = QString::fromStdString(imagePathStr);
    QPixmap pixmap;

    // Prova a caricare immagine se il path è valido
    if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
        pixmap.load(imagePath);
    }

    // Fallback
    if (pixmap.isNull()) {
        pixmap.load(":/icons/placeholder.png");
    }

    QLabel* label = new QLabel();
    label->setFixedSize(200, 200);
    label->setAlignment(Qt::AlignCenter);

    if (isArtist) {
        QPixmap circle(200, 200);
        circle.fill(Qt::transparent);

        QPainter p(&circle);
        p.setRenderHint(QPainter::Antialiasing);

        // Solo bordo bianco, sfondo trasparente
        QPen pen(Qt::white);
        pen.setWidth(4);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(2, 2, 196, 196);

        // Se immagine valida, disegnala centrata e ritagliata nel cerchio
        if (!pixmap.isNull()) {
            QPainterPath path;
            path.addEllipse(0, 0, 200, 200);
            p.setClipPath(path);
            p.drawPixmap(0, 0,
                pixmap.scaled(200, 200,
                              Qt::KeepAspectRatioByExpanding,
                              Qt::SmoothTransformation));
        }

        p.end();
        label->setPixmap(circle);
        label->setObjectName("artistImageLabel");
    } else {
        label->setPixmap(
            pixmap.scaled(200, 200,
                          Qt::KeepAspectRatio,
                          Qt::SmoothTransformation));
        label->setObjectName("defaultImageLabel");
    }

    return label;
}
// ---------------- VISITORS ----------------

// ---------------- artista -----------------
void VisitorGUI::visit(const Artista* artista) {
    // Da cancellare
    qDebug() << "VisitorGUI::visit(Artista)" << QString::fromStdString(artista->getNome());
    
    // 1) Svuota il layout
    clearLayout();

    // 2) Container & layout interno
    QWidget* container = new QWidget();
    container->setObjectName("artistContainer");
    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(40,20,40,20);
    vbox->setSpacing(16);

    // 3) Immagine circolare
    QLabel* imageLabel = createImageLabel(artista->getImagePath(), true);
    imageLabel->setObjectName("artistImage");
    vbox->addWidget(imageLabel, 0, Qt::AlignHCenter);

    // 4) Nome
    QLabel* nameLabel = new QLabel(QString::fromStdString(artista->getNome()), container);
    nameLabel->setObjectName("artistName");
    nameLabel->setAlignment(Qt::AlignCenter);
    vbox->addWidget(nameLabel);

    QLabel* genreLabel = new QLabel(QString::fromStdString(artista->getGenere()), container);
    genreLabel->setObjectName("artistGenre");
    genreLabel->setAlignment(Qt::AlignCenter);
    vbox->addWidget(genreLabel);

    // 5) Descrizione
    QGroupBox* descBox = new QGroupBox("Descrizione", container);
    descBox->setObjectName("artistDescBox");
    descBox->setAlignment(Qt::AlignCenter);
    QVBoxLayout* descLayout = new QVBoxLayout(descBox);
    QLabel* descLabel = new QLabel(QString::fromStdString(artista->getInfo()), descBox);
    descLabel->setWordWrap(true);
    descLayout->addWidget(descLabel);
    vbox->addWidget(descBox);

    // 6) Prodotti
    QGroupBox* prodBox = new QGroupBox("Prodotti", container);
    prodBox->setObjectName("artistProdBox");
    QVBoxLayout* prodLayout = new QVBoxLayout(prodBox);
    QListWidget* prodList = new QListWidget(prodBox);
    prodList->setObjectName("artistProdList");
    prodList->setSelectionMode(QAbstractItemView::NoSelection);
    prodList->setSpacing(8);          // spazio tra le card
    prodList->setUniformItemSizes(false);
    prodLayout->addWidget(prodList);

    // per ogni prodotto…
    for (auto* p : artista->getProducts()) {
        // 1) crea l'item e imposta l'altezza minima
        auto* item = new QListWidgetItem(prodList);
        item->setSizeHint(QSize(0, 100));  // altezza fissa 100px

        // 2) crea il widget “card” che andrà nell'item
        QWidget* card = new QWidget();
        card->setObjectName("productCard");
        QHBoxLayout* h = new QHBoxLayout(card);
        h->setContentsMargins(10,10,10,10);
        h->setSpacing(12);

        // 3) immagine miniatura
        QLabel* thumb = createImageLabel(p->getImagePath(), false);
        thumb->setFixedSize(80,80);
        thumb->setObjectName("productThumb");
        h->addWidget(thumb);

        // 4) titolo e descrizione in colonna
        QVBoxLayout* info = new QVBoxLayout();
        QLabel* title = new QLabel(QString::fromStdString(p->getTitle()), card);
        title->setObjectName("productTitle");
        title->setWordWrap(true);
        info->addWidget(title);

        if (auto descr = p->getDescription(); !descr.empty()) {
            QLabel* desc = new QLabel(QString::fromStdString(descr), card);
            desc->setObjectName("productDesc");
            desc->setWordWrap(true);
            info->addWidget(desc);
        }
        info->addStretch();
        h->addLayout(info);

        // 5) eventuale pulsante azione
        QPushButton* btn = new QPushButton("Dettagli", card);
        btn->setObjectName("productButton");
        connect(btn, &QPushButton::clicked, this, [=](){
            p->accept(this);
        });
        h->addWidget(btn, 0, Qt::AlignVCenter);

        // 6) inserisci la card nell'item
        prodList->setItemWidget(item, card);
    }
    
    vbox->addWidget(prodBox);

    layout->addWidget(container);
}
// ------------------------------------

// ------------- merch ----------------
void VisitorGUI::visit(const TShirt* tshirt) {
    qDebug() << "VisitorGUI::visit(TShirt)";

    clearLayout();

    // Layout principale: nessun margine/spazio extra
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Container principale
    QWidget* container = new QWidget();
    container->setObjectName("tshirtContainer");
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Layout verticale interno con padding/spacing ridotti
    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(40, 8, 40, 8);  // left, top, right, bottom
    vbox->setSpacing(6);                      // spazio verticale tra widget

    // Immagine
    QLabel* imgLabel = createImageLabel(tshirt->getImagePath(), false);
    imgLabel->setObjectName("tshirtImageLabel");
    imgLabel->setFixedSize(200, 200);
    imgLabel->setMargin(0);
    vbox->addWidget(imgLabel, 0, Qt::AlignHCenter);

    // Titolo
    QLabel* title = new QLabel(QString::fromStdString(tshirt->getTitle()));
    title->setObjectName("tshirtTitleLabel");
    title->setAlignment(Qt::AlignCenter);
    title->setMargin(0);
    vbox->addWidget(title);

    // --- PRIMA riga (codice prodotto) ---
    QWidget* grid0 = new QWidget();
    grid0->setObjectName("tshirtGrid0");
    QHBoxLayout* layout0 = new QHBoxLayout(grid0);
    layout0->setContentsMargins(0, 0, 0, 0);
    layout0->setSpacing(4);
    layout0->setAlignment(Qt::AlignCenter);
    QLabel* codeLabel = new QLabel(
        "<b>Codice Prodotto:</b> " +
        QString::fromStdString(tshirt->getCodiceProdotto())
    );
    codeLabel->setObjectName("tshirtCodeLabel");
    codeLabel->setMargin(0);
    layout0->addWidget(codeLabel);
    vbox->addWidget(grid0);

    // --- SECONDA riga (disponibilità, quantità, prezzo) ---
    QWidget* grid1 = new QWidget();
    grid1->setObjectName("tshirtGrid1");
    QHBoxLayout* layout1 = new QHBoxLayout(grid1);
    layout1->setContentsMargins(0, 0, 0, 0);
    layout1->setSpacing(12);
    layout1->setAlignment(Qt::AlignCenter);
    QLabel* dispoLabel = new QLabel(
        "<b>Disponibilità:</b> " +
        QString(tshirt->getDisponibile() ? "Disponibile" : "Non disponibile")
    );
    dispoLabel->setObjectName("tshirtAvailabilityLabel");
    dispoLabel->setMargin(0);
    QLabel* qtyLabel = new QLabel(
        "<b>Quantità:</b> " +
        QString::number(tshirt->getQuantita())
    );
    qtyLabel->setObjectName("tshirtQuantityLabel");
    qtyLabel->setMargin(0);
    QLabel* priceLabel = new QLabel(
        "<b>Prezzo:</b> " +
        QString("%1 €").arg(tshirt->getPrezzo())
    );
    priceLabel->setObjectName("tshirtPriceLabel");
    priceLabel->setMargin(0);
    layout1->addWidget(dispoLabel);
    layout1->addWidget(qtyLabel);
    layout1->addWidget(priceLabel);
    vbox->addWidget(grid1);

    // --- TERZA riga (taglia, colore) ---
    QWidget* grid2 = new QWidget();
    grid2->setObjectName("tshirtGrid2");
    QHBoxLayout* layout2 = new QHBoxLayout(grid2);
    layout2->setContentsMargins(0, 0, 0, 0);
    layout2->setSpacing(12);
    layout2->setAlignment(Qt::AlignCenter);
    QLabel* sizeLabel = new QLabel(
        "<b>Taglia:</b> " +
        QString::fromStdString(tshirt->getTaglia())
    );
    sizeLabel->setObjectName("tshirtSizeLabel");
    sizeLabel->setMargin(0);
    QLabel* colorLabel = new QLabel(
        "<b>Colore:</b> " +
        QString::fromStdString(tshirt->getColore())
    );
    colorLabel->setObjectName("tshirtColorLabel");
    colorLabel->setMargin(0);
    layout2->addWidget(sizeLabel);
    layout2->addWidget(colorLabel);
    vbox->addWidget(grid2);

    // Descrizione
    QLabel* desc = new QLabel(QString::fromStdString(tshirt->getDescription()));
    desc->setObjectName("tshirtDescLabel");
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignLeft);
    desc->setMargin(0);
    vbox->addWidget(desc);

    // Aggiungo il container al layout principale
    layout->addWidget(container, /*stretch=*/0);
}


void VisitorGUI::visit(const CD* cd) {
    qDebug() << "VisitorGUI::visit(CD)";

    clearLayout();

    // Container principale
    QWidget* container = new QWidget();
    container->setObjectName("cdContainer");
    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(40, 20, 40, 20);
    vbox->setSpacing(8);

    // Immagine
    QLabel* imgLabel = createImageLabel(cd->getImagePath(), false);
    imgLabel->setObjectName("cdImageLabel");
    vbox->addWidget(imgLabel, 0, Qt::AlignCenter);

    // Titolo
    QLabel* title = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(cd->getTitle()))
    );
    title->setObjectName("cdTitleLabel");
    title->setAlignment(Qt::AlignCenter);
    vbox->addWidget(title);

    // === PRIMA riga (tipo, codici, produttore) ===
    QWidget* grid1 = new QWidget();
    grid1->setObjectName("cdGrid1");
    QHBoxLayout* layout1 = new QHBoxLayout(grid1);
    layout1->setContentsMargins(10, 0, 10, 0);
    layout1->setSpacing(15);
    layout1->setAlignment(Qt::AlignCenter);

    QLabel* typeLabel = new QLabel(
        "<b>Tipo Prodotto:</b> " +
        QString::fromStdString(cd->getTipoProdotto())
    );
    typeLabel->setObjectName("cdTypeLabel");

    QLabel* codeLabel = new QLabel(
        "<b>Codice Prodotto:</b> " +
        QString::fromStdString(cd->getCodiceProdotto())
    );
    codeLabel->setObjectName("cdCodeLabel");

    QLabel* recLabel = new QLabel(
        "<b>Codice Riconoscimento:</b> " +
        QString::fromStdString(cd->getCodiceRiconoscimento())
    );
    recLabel->setObjectName("cdRecognitionLabel");

    QLabel* prodLabel = new QLabel(
        "<b>Produttore Stampe:</b> " +
        QString::fromStdString(cd->getProduttoreStampe())
    );
    prodLabel->setObjectName("cdProducerLabel");

    layout1->addWidget(typeLabel);
    layout1->addWidget(codeLabel);
    layout1->addWidget(recLabel);
    layout1->addWidget(prodLabel);
    vbox->addWidget(grid1, 0, Qt::AlignCenter);

    // === SECONDA riga (disponibilità, quantità, prezzo) ===
    QWidget* grid2 = new QWidget();
    grid2->setObjectName("cdGrid2");
    QHBoxLayout* layout2 = new QHBoxLayout(grid2);
    layout2->setContentsMargins(10, 0, 10, 0);
    layout2->setSpacing(15);
    layout2->setAlignment(Qt::AlignCenter);

    QLabel* dispoLabel = new QLabel(
        "<b>Disponibilità:</b> " +
        QString(cd->getDisponibile() ? "Disponibile" : "Non disponibile")
    );
    dispoLabel->setObjectName("cdAvailabilityLabel");

    QLabel* qtyLabel = new QLabel(
        "<b>Quantità:</b> " +
        QString::number(cd->getQuantita())
    );
    qtyLabel->setObjectName("cdQuantityLabel");

    QLabel* priceLabel = new QLabel(
        "<b>Prezzo:</b> " +
        QString("%1 €").arg(cd->getPrezzo())
    );
    priceLabel->setObjectName("cdPriceLabel");

    layout2->addWidget(dispoLabel);
    layout2->addWidget(qtyLabel);
    layout2->addWidget(priceLabel);
    vbox->addWidget(grid2, 0, Qt::AlignCenter);

    // === TERZA riga (formato) ===
    QLabel* formato = new QLabel(
        "<b>Formato:</b> " +
        QString::fromStdString(cd->getFormato())
    );
    formato->setObjectName("cdFormatLabel");
    formato->setAlignment(Qt::AlignCenter);
    vbox->addWidget(formato);

    // Descrizione
    QLabel* desc = new QLabel(QString::fromStdString(cd->getDescription()));
    desc->setObjectName("cdDescLabel");
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignLeft);
    vbox->addWidget(desc);

    layout->addWidget(container);
}

void VisitorGUI::visit(const Vinile* vinile) {
    qDebug() << "VisitorGUI::visit(Vinile)";

    clearLayout();

    // Container principale
    QWidget* container = new QWidget();
    container->setObjectName("vinileContainer");
    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(40, 20, 40, 20);
    vbox->setSpacing(8);

    // Immagine
    QLabel* imgLabel = createImageLabel(vinile->getImagePath(), false);
    imgLabel->setObjectName("vinileImageLabel");
    vbox->addWidget(imgLabel, 0, Qt::AlignCenter);

    // Titolo
    QLabel* title = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(vinile->getTitle()))
    );
    title->setObjectName("vinileTitleLabel");
    title->setAlignment(Qt::AlignCenter);
    vbox->addWidget(title);

    // === PRIMA riga (tipo, codici, produttore) ===
    QWidget* grid1 = new QWidget();
    grid1->setObjectName("vinileGrid1");
    QHBoxLayout* layout1 = new QHBoxLayout(grid1);
    layout1->setContentsMargins(10, 0, 10, 0);
    layout1->setSpacing(15);
    layout1->setAlignment(Qt::AlignCenter);

    QLabel* typeLabel = new QLabel(
        "<b>Tipo Prodotto:</b> " +
        QString::fromStdString(vinile->getTipoProdotto())
    );
    typeLabel->setObjectName("vinileTypeLabel");

    QLabel* codeLabel = new QLabel(
        "<b>Codice Prodotto:</b> " +
        QString::fromStdString(vinile->getCodiceProdotto())
    );
    codeLabel->setObjectName("vinileCodeLabel");

    QLabel* recLabel = new QLabel(
        "<b>Codice Riconoscimento:</b> " +
        QString::fromStdString(vinile->getCodiceRiconoscimento())
    );
    recLabel->setObjectName("vinileRecognitionLabel");

    QLabel* prodLabel = new QLabel(
        "<b>Produttore Stampe:</b> " +
        QString::fromStdString(vinile->getProduttoreStampe())
    );
    prodLabel->setObjectName("vinileProducerLabel");

    layout1->addWidget(typeLabel);
    layout1->addWidget(codeLabel);
    layout1->addWidget(recLabel);
    layout1->addWidget(prodLabel);
    vbox->addWidget(grid1, 0, Qt::AlignCenter);

    // === SECONDA riga (disponibilità, quantità, prezzo) ===
    QWidget* grid2 = new QWidget();
    grid2->setObjectName("vinileGrid2");
    QHBoxLayout* layout2 = new QHBoxLayout(grid2);
    layout2->setContentsMargins(10, 0, 10, 0);
    layout2->setSpacing(15);
    layout2->setAlignment(Qt::AlignCenter);

    QLabel* dispoLabel = new QLabel(
        "<b>Disponibilità:</b> " +
        QString(vinile->getDisponibile() ? "Disponibile" : "Non disponibile")
    );
    dispoLabel->setObjectName("vinileAvailabilityLabel");

    QLabel* qtyLabel = new QLabel(
        "<b>Quantità:</b> " +
        QString::number(vinile->getQuantita())
    );
    qtyLabel->setObjectName("vinileQuantityLabel");

    QLabel* priceLabel = new QLabel(
        "<b>Prezzo:</b> " +
        QString("%1 €").arg(vinile->getPrezzo())
    );
    priceLabel->setObjectName("vinilePriceLabel");

    layout2->addWidget(dispoLabel);
    layout2->addWidget(qtyLabel);
    layout2->addWidget(priceLabel);
    vbox->addWidget(grid2, 0, Qt::AlignCenter);

    // === TERZA riga (diametro, RPM) ===
    QWidget* grid3 = new QWidget();
    grid3->setObjectName("vinileGrid3");
    QHBoxLayout* layout3 = new QHBoxLayout(grid3);
    layout3->setContentsMargins(10, 0, 10, 0);
    layout3->setSpacing(15);
    layout3->setAlignment(Qt::AlignCenter);

    QLabel* diamLabel = new QLabel(
        "<b>Diametro:</b> " +
        QString::number(vinile->getDiametro()) + " cm"
    );
    diamLabel->setObjectName("vinileDiameterLabel");

    QLabel* rpmLabel = new QLabel(
        "<b>RPM:</b> " +
        QString::number(vinile->getRpm())
    );
    rpmLabel->setObjectName("vinileRpmLabel");

    layout3->addWidget(diamLabel);
    layout3->addWidget(rpmLabel);
    vbox->addWidget(grid3, 0, Qt::AlignCenter);

    // Descrizione
    QLabel* desc = new QLabel(QString::fromStdString(vinile->getDescription()));
    desc->setObjectName("vinileDescLabel");
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignLeft);
    vbox->addWidget(desc);

    layout->addWidget(container);
}
// ------------------------------------


// ---------- musica ----------------
void VisitorGUI::visit(const Singolo* singolo) {
    qDebug() << "VisitorGUI::visit(Singolo)";


    clearLayout();

    // Container principale
    QWidget* container = new QWidget();
    container->setObjectName("singoloContainer");
    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(40, 20, 40, 20);
    vbox->setSpacing(8);

    // Immagine
    QLabel* imgLabel = createImageLabel(singolo->getImagePath(), false);
    imgLabel->setObjectName("singoloImageLabel");
    vbox->addWidget(imgLabel, 0, Qt::AlignCenter);

    // Titolo
    QLabel* title = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(singolo->getTitle()))
    );
    title->setObjectName("singoloTitleLabel");
    title->setAlignment(Qt::AlignCenter);
    vbox->addWidget(title);

    // --- Primo blocco: Data Uscita + Durata ---
    QWidget* infoRow1 = new QWidget();
    infoRow1->setObjectName("singoloInfoRow1");
    QHBoxLayout* layoutInfo1 = new QHBoxLayout(infoRow1);
    layoutInfo1->setContentsMargins(0, 0, 0, 0);
    layoutInfo1->setSpacing(15);
    layoutInfo1->setAlignment(Qt::AlignCenter);

    QLabel* dataLabel = new QLabel(
        QString("<b>Data Uscita:</b> ") +
        QString::fromStdString(singolo->getDataUscita().toString())
    );
    dataLabel->setObjectName("singoloReleaseDateLabel");

    QLabel* durataLabel = new QLabel(
        "<b>Durata:</b> " +
        QString::fromStdString(singolo->getDurata().toString())
    );
    durataLabel->setObjectName("singoloDurationLabel");

    layoutInfo1->addWidget(dataLabel);
    layoutInfo1->addWidget(durataLabel);
    vbox->addWidget(infoRow1, 0, Qt::AlignCenter);

    // --- Secondo blocco: Genere + Versione + Chart ---
    QWidget* infoRow2 = new QWidget();
    infoRow2->setObjectName("singoloInfoRow2");
    QHBoxLayout* layoutInfo2 = new QHBoxLayout(infoRow2);
    layoutInfo2->setContentsMargins(0, 0, 0, 0);
    layoutInfo2->setSpacing(15);
    layoutInfo2->setAlignment(Qt::AlignCenter);

    QLabel* genreLabel = new QLabel(
        "<b>Genere:</b> " +
        QString::fromStdString(singolo->getGenere())
    );
    genreLabel->setObjectName("singoloGenreLabel");

    QLabel* versionLabel = new QLabel(
        "<b>Versione:</b> " +
        QString(singolo->getIsRemix() ? "Remix" : "Original")
    );
    versionLabel->setObjectName("singoloVersionLabel");

    QLabel* chartLabel = new QLabel(
        "<b>Classifica:</b> " +
        QString::number(singolo->getChartPosition())
    );
    chartLabel->setObjectName("singoloChartLabel");

    layoutInfo2->addWidget(genreLabel);
    layoutInfo2->addWidget(versionLabel);
    layoutInfo2->addWidget(chartLabel);
    vbox->addWidget(infoRow2, 0, Qt::AlignCenter);

    // Descrizione
    QLabel* desc = new QLabel(QString::fromStdString(singolo->getDescription()));
    desc->setObjectName("singoloDescLabel");
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignLeft);
    vbox->addWidget(desc);

    // Info traccia principale
    const Traccia& track = singolo->getMainTrack();
    QLabel* trackInfo = new QLabel(
        QString("%1 (%2)")
            .arg(QString::fromStdString(track.getNome()))
            .arg(QString::fromStdString(track.getDurata().toString()))
    );
    trackInfo->setObjectName("singoloTrackInfoLabel");
    trackInfo->setAlignment(Qt::AlignLeft);
    vbox->addWidget(trackInfo);

    layout->addWidget(container);
}

void VisitorGUI::visit(const Album* album) {
    qDebug() << "VisitorGUI::visit(Album)";


    clearLayout();

    // Container principale
    QWidget* container = new QWidget();
    container->setObjectName("albumContainer");
    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(40, 20, 40, 20);
    vbox->setSpacing(8);

    // Immagine
    QLabel* imgLabel = createImageLabel(album->getImagePath(), false);
    imgLabel->setObjectName("albumImageLabel");
    vbox->addWidget(imgLabel, 0, Qt::AlignCenter);

    // Titolo
    QLabel* title = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(album->getTitle()))
    );
    title->setObjectName("albumTitleLabel");
    title->setAlignment(Qt::AlignCenter);
    vbox->addWidget(title);

    // Primo blocco info: data uscita + durata totale
    QWidget* infoRow1 = new QWidget();
    infoRow1->setObjectName("albumInfoRow1");
    QHBoxLayout* layoutInfo1 = new QHBoxLayout(infoRow1);
    layoutInfo1->setContentsMargins(0, 0, 0, 0);
    layoutInfo1->setSpacing(15);
    layoutInfo1->setAlignment(Qt::AlignCenter);

    QLabel* releaseDateLabel = new QLabel(
        "<b>Data Uscita:</b> " +
        QString::fromStdString(album->getDataUscita().toString())
    );
    releaseDateLabel->setObjectName("albumReleaseDateLabel");

    QLabel* totalDurationLabel = new QLabel(
        "<b>Durata Totale:</b> " +
        QString::fromStdString(album->getDurata().toString())
    );
    totalDurationLabel->setObjectName("albumTotalDurationLabel");

    layoutInfo1->addWidget(releaseDateLabel);
    layoutInfo1->addWidget(totalDurationLabel);
    vbox->addWidget(infoRow1, 0, Qt::AlignCenter);

    // Label discografica
    QLabel* labelLabel = new QLabel(
        "<b>Label:</b> " +
        QString::fromStdString(album->getLabel())
    );
    labelLabel->setObjectName("albumLabelLabel");
    labelLabel->setAlignment(Qt::AlignCenter);
    vbox->addWidget(labelLabel);

    // Descrizione
    QLabel* desc = new QLabel(QString::fromStdString(album->getDescription()));
    desc->setObjectName("albumDescLabel");
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignLeft);
    vbox->addWidget(desc);

    // Tracce
    int idx = 0;
    for (const auto& track : album->getTracce()) {
        QString info = QString("• %1 (%2)")
            .arg(QString::fromStdString(track.getNome()))
            .arg(QString::fromStdString(track.getDurata().toString()));
        QLabel* trackLabel = new QLabel(info);
        trackLabel->setObjectName(QString("albumTrackLabel_%1").arg(idx++));
        trackLabel->setAlignment(Qt::AlignLeft);
        vbox->addWidget(trackLabel);
    }

    layout->addWidget(container);
}
// ------------------------------------

// ---------- tour ----------------
void VisitorGUI::visit(const Tour* tour) {
    qDebug() << "VisitorGUI::visit(Tour)";

    clearLayout();

    // Container principale
    QWidget* container = new QWidget();
    container->setObjectName("tourContainer");
    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(40, 20, 40, 20);
    vbox->setSpacing(8);

    // Immagine Tour
    QLabel* imgLabel = createImageLabel(tour->getImagePath(), false);
    imgLabel->setObjectName("tourImageLabel");
    vbox->addWidget(imgLabel, 0, Qt::AlignCenter);

    // Titolo Tour
    QLabel* title = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(tour->getTitle()))
    );
    title->setObjectName("tourTitleLabel");
    title->setAlignment(Qt::AlignCenter);
    vbox->addWidget(title);

    // === PRIMA riga: disponibilità, quantità, prezzo ===
    QWidget* grid1 = new QWidget();
    grid1->setObjectName("tourGrid1");
    QHBoxLayout* layout1 = new QHBoxLayout(grid1);
    layout1->setContentsMargins(10, 0, 10, 0);
    layout1->setSpacing(15);
    layout1->setAlignment(Qt::AlignCenter);

    QLabel* dispoLabel = new QLabel(
        "<b>Disponibilità:</b> " +
        QString(tour->getDisponibile() ? "Disponibile" : "Non disponibile")
    );
    dispoLabel->setObjectName("tourAvailabilityLabel");

    QLabel* qtyLabel = new QLabel(
        "<b>Quantità:</b> " +
        QString::number(tour->getQuantita())
    );
    qtyLabel->setObjectName("tourQuantityLabel");

    QLabel* priceLabel = new QLabel(
        "<b>Prezzo:</b> " +
        QString("%1 €").arg(tour->getPrezzo())
    );
    priceLabel->setObjectName("tourPriceLabel");

    layout1->addWidget(dispoLabel);
    layout1->addWidget(qtyLabel);
    layout1->addWidget(priceLabel);
    vbox->addWidget(grid1, 0, Qt::AlignCenter);

    // === SECONDA riga: periodo del tour ===
    const auto& dateTour = tour->getDateTour();
    if (!dateTour.empty()) {
        const Data inizio = dateTour.front().getData();
        const Data fine   = dateTour.back().getData();
        QLabel* periodLabel = new QLabel(
            "<b>Periodo:</b> " +
            QString::fromStdString(inizio.toString()) + " → " +
            QString::fromStdString(fine.toString())
        );
        periodLabel->setObjectName("tourPeriodLabel");
        periodLabel->setAlignment(Qt::AlignCenter);
        vbox->addWidget(periodLabel);
    }

    // Descrizione
    QLabel* desc = new QLabel(QString::fromStdString(tour->getDescription()));
    desc->setObjectName("tourDescLabel");
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignLeft);
    vbox->addWidget(desc);

    layout->addWidget(container);
}
// ------------------------------------