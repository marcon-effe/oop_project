#include "VisitorGUI.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QGroupBox>
#include <QImage>
#include <QScrollArea>
#include <QListWidget>
#include <QPushButton>
#include <QFile>
#include <QDialog>
#include <QTextEdit>
#include <QLayoutItem>
#include <QFrame>
#include <QDebug>

#include "../model/core/ArtistProduct.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/merch/TShirt.h"
#include "../model/tour/Tour.h"
#include "../model/artisti/Artista.h"
#include "./util/ClickableLabel.h"


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

// Gestione della sezione correlati 
void VisitorGUI::setArtistMap(const std::unordered_map<unsigned int, Artista*>& artistsMap) {
    m_artists = &artistsMap;
}

QWidget* VisitorGUI::createRelatedProductsSection(unsigned int artistId) {
    // 1) Scroll area full-width, altezza minimal
    auto* scroll = new QScrollArea;
    scroll->setObjectName("relatedProductsScrollArea");
    scroll->setWidgetResizable(true);
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 2) Container interno espandibile orizzontalmente
    QWidget* container = new QWidget;
    container->setObjectName("relatedProductsContainer");
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* hLayout = new QHBoxLayout(container);
    hLayout->setContentsMargins(12, 6, 12, 6);   // margini esterni
    hLayout->setSpacing(24);                     // più spazio tra le card
    hLayout->setSizeConstraint(QLayout::SetFixedSize);

    if (!m_artists) {
        scroll->setWidget(container);
        return scroll;
    }
    auto it = m_artists->find(artistId);
    if (it == m_artists->end() || !it->second) {
        scroll->setWidget(container);
        return scroll;
    }

    // 3) Creazione delle card prodotto, ancora più grandi
    for (auto& pp : it->second->getProducts()) {
        auto* prod = pp.second;

        QWidget* card = new QWidget;
        card->setObjectName("productCard");
        card->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
        auto* vLayout = new QVBoxLayout(card);
        vLayout->setContentsMargins(16, 16, 16, 16);
        vLayout->setSpacing(16);
        vLayout->setSizeConstraint(QLayout::SetFixedSize);
        vLayout->setAlignment(Qt::AlignCenter);

        QLabel* thumb = new QLabel;
        thumb->setObjectName("productThumbnail");
        thumb->setFixedSize(120, 120);
        thumb->setAlignment(Qt::AlignCenter);
        thumb->setScaledContents(true);

        QString imgPath = QString::fromStdString(prod->getImagePath());
        QPixmap pixmap;

        // prova a caricare l’immagine “reale”
        if (!imgPath.isEmpty() && pixmap.load(imgPath)) {
            thumb->setPixmap(pixmap.scaled(thumb->size(),
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
        }
        else if (pixmap.load(":/icons/placeholder.png")) {
            thumb->setPixmap(pixmap.scaled(thumb->size(),
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
        }



        QLabel* title = new QLabel(QString::fromStdString(prod->getTitle()));
        title->setObjectName("productTitle");
        title->setWordWrap(true);
        title->setAlignment(Qt::AlignCenter);
        title->setFixedWidth(120);

        QPushButton* btn = new QPushButton("Apri", card);
        btn->setObjectName("productButton");
        btn->setFixedHeight(36);
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [=]() {
            prod->accept(this);
        });

        vLayout->addWidget(thumb);
        vLayout->addWidget(title);
        vLayout->addWidget(btn);
        hLayout->addWidget(card, 0, Qt::AlignTop);
    }

    scroll->setWidget(container);
    return scroll;
}

QWidget* VisitorGUI::createTrackWidget(const Traccia& track) {
    // Container come QFrame per ereditare lo stile QSS
    QFrame* trackContainer = new QFrame;
    trackContainer->setObjectName("singoloTrackContainer");
    trackContainer->setFrameShape(QFrame::StyledPanel);
    trackContainer->setFrameShadow(QFrame::Raised);

    auto* layout = new QHBoxLayout(trackContainer);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(12);

    // Info (nome e durata)
    auto* infoWidget = new QWidget(trackContainer);
    auto* infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0,0,0,0);
    infoLayout->setSpacing(4);

    auto* nameLabel = new QLabel(QString::fromStdString(track.getNome()), infoWidget);
    nameLabel->setObjectName("trackNameLabel");
    QFont f = nameLabel->font(); f.setPointSize(12); f.setBold(true);
    nameLabel->setFont(f);
    infoLayout->addWidget(nameLabel);

    auto* durLabel = new QLabel(QString::fromStdString(track.getDurata().toString()), infoWidget);
    durLabel->setObjectName("trackDurationLabel");
    f = durLabel->font(); f.setPointSize(10);
    durLabel->setFont(f);
    durLabel->setStyleSheet("color: #555555;");
    infoLayout->addWidget(durLabel);

    layout->addWidget(infoWidget, 1);

    auto* testoBtn = new QPushButton("Testo", trackContainer);
    testoBtn->setObjectName("textTrackButton");
    testoBtn->setCursor(Qt::PointingHandCursor);
    layout->addWidget(testoBtn, 0, Qt::AlignVCenter);

    connect(testoBtn, &QPushButton::clicked, this, [this, track, trackContainer]() {
        QDialog dlg(trackContainer);   // mantengo scollegato -> obbliga la chiusura del dialog prima della chiusura della pagina
        dlg.setWindowTitle("Testo: " + QString::fromStdString(track.getNome()));
        auto* dlgLayout = new QVBoxLayout(&dlg);
        auto* editor = new QTextEdit(&dlg);
        editor->setReadOnly(true);
        editor->setText(QString::fromStdString(track.getTesto()));
        dlgLayout->addWidget(editor);
        auto* closeBtn = new QPushButton("Chiudi", &dlg);
        connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        dlgLayout->addWidget(closeBtn, 0, Qt::AlignRight);
        dlg.exec();
    });

    return trackContainer;
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
    for (const auto& pair : artista->getProducts()) {
        ArtistProduct* p = pair.second;
        
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
    clearLayout();
    qDebug() << "VisitorGUI::visit(TShirt)";

    // — Main wrapper
    QWidget* mainWrapper = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8,8,8,8);
    mainLayout->setSpacing(12);

    QWidget* productCard = new QWidget();
    productCard->setObjectName("tshirtContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24,30,24,12);
    productLayout->setSpacing(10);


    QPixmap pix(QString::fromStdString(tshirt->getImagePath()));
    if (pix.isNull()) pix.load(":/icons/placeholder.png");

    ClickableLabel* imageLabel = new ClickableLabel(productCard);
    imageLabel->setObjectName("tshirtImageLabel");
    imageLabel->setPixmap(pix.scaled(180,180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setFixedSize(180,180);
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    connect(imageLabel, &ClickableLabel::clicked, this, [pix](){
        QDialog dlg;
        dlg.setWindowTitle("Anteprima");
        QVBoxLayout L(&dlg);
        QLabel* big = new QLabel();
        big->setPixmap(pix.scaled(600,600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        big->setAlignment(Qt::AlignCenter);
        L.addWidget(big);
        dlg.exec();
    });


    QLabel* titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(tshirt->getTitle()))
    );
    titleLabel->setObjectName("tshirtTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    QLabel* availabilityLabel = new QLabel(
        tshirt->getDisponibile() ? "Disponibile" : "Sold Out"
    );
    availabilityLabel->setObjectName("tshirtAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(availabilityLabel);

    {
        QWidget* tagPriceRow = new QWidget();
        QHBoxLayout* tpLayout = new QHBoxLayout(tagPriceRow);
        tpLayout->setContentsMargins(0,0,0,0);
        tpLayout->setSpacing(12);

        tpLayout->addWidget(new QLabel(
            "<b>Taglia:</b> " + QString::fromStdString(tshirt->getTaglia())
        ));
        tpLayout->addWidget(new QLabel(
            "<b>Colore:</b> " + QString::fromStdString(tshirt->getColore())
        ));
        QLabel* priceLabel = new QLabel(
            "<b>Prezzo:</b> " + QString("%1 €").arg(tshirt->getPrezzo())
        );
        priceLabel->setObjectName("tshirtPriceLabel");
        tpLayout->addWidget(priceLabel, 0, Qt::AlignRight);

        productLayout->addWidget(tagPriceRow, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(6);
    QLabel* descLabel = new QLabel(QString::fromStdString(tshirt->getDescription()));
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    descLabel->setObjectName("tshirtDescLabel");
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(6);

    // Sezione correlati 
    unsigned int artistId = tshirt->getArtistId();

    QLabel* correlatiLabel = new QLabel("Prodotti correlati");
    correlatiLabel->setStyleSheet("font-weight: bold; font-size: 14pt; margin-top: 30px; margin-bottom: 20px;");
    productLayout->addWidget(correlatiLabel);

    QWidget* relatedProductsWidget = createRelatedProductsSection(artistId);
    productLayout->addWidget(relatedProductsWidget);

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);    
}

void VisitorGUI::visit(const CD* cd) {
    clearLayout();
    qDebug() << "VisitorGUI::visit(CD)";

    QWidget* mainWrapper = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8,8,8,8);
    mainLayout->setSpacing(12);

    QWidget* productCard = new QWidget();
    productCard->setObjectName("cdContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24,30,24,12);
    productLayout->setSpacing(10);

    QPixmap pix(QString::fromStdString(cd->getImagePath()));
    if (pix.isNull()) pix.load(":/icons/placeholder.png");

    ClickableLabel* imageLabel = new ClickableLabel(productCard);
    imageLabel->setObjectName("cdImageLabel");
    imageLabel->setPixmap(pix.scaled(180,180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setFixedSize(180,180);
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    connect(imageLabel, &ClickableLabel::clicked, this, [pix](){
        QDialog dlg;
        dlg.setWindowTitle("Anteprima");
        QVBoxLayout L(&dlg);
        QLabel* big = new QLabel();
        big->setPixmap(pix.scaled(600,600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        big->setAlignment(Qt::AlignCenter);
        L.addWidget(big);
        dlg.exec();
    });

    QLabel* titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(cd->getTitle()))
    );
    titleLabel->setObjectName("cdTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    QLabel* availabilityLabel = new QLabel(
        cd->getDisponibile() ? "Disponibile" : "Sold Out"
    );
    availabilityLabel->setObjectName("cdAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(availabilityLabel);

    {
        QWidget* row1 = new QWidget(productCard);
        QHBoxLayout* l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0,0,0,0);
        l1->setSpacing(20);

        l1->addWidget(new QLabel(
            "<b>Tipo:</b> " + QString::fromStdString(cd->getTipoProdotto())
        ));
        l1->addWidget(new QLabel(
            "<b>Produttore:</b> " + QString::fromStdString(cd->getProduttoreStampe())
        ));

        productLayout->addWidget(row1, 0, Qt::AlignHCenter);
    }

    {
        QWidget* row2 = new QWidget(productCard);
        QHBoxLayout* l2 = new QHBoxLayout(row2);
        l2->setContentsMargins(0,0,0,0);
        l2->setSpacing(20);

        l2->addWidget(new QLabel(
            "<b>Prezzo:</b> " + QString("%1 €").arg(cd->getPrezzo())
        ));

        productLayout->addWidget(row2, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(8);
    QLabel* descLabel = new QLabel(QString::fromStdString(cd->getDescription()));
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    descLabel->setObjectName("cdDescLabel");
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(12);
    
    // Sezione correlati 
    unsigned int artistId = cd->getArtistId();

    QLabel* correlatiLabel = new QLabel("Prodotti correlati");
    correlatiLabel->setStyleSheet("font-weight: bold; font-size: 14pt; margin-top: 30px; margin-bottom: 20px;");
    productLayout->addWidget(correlatiLabel);

    QWidget* relatedProductsWidget = createRelatedProductsSection(artistId);
    productLayout->addWidget(relatedProductsWidget);

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);
}

void VisitorGUI::visit(const Vinile* vinile) {
    clearLayout();
    qDebug() << "VisitorGUI::visit(Vinile)";

    QWidget* mainWrapper = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8,8,8,8);
    mainLayout->setSpacing(12);

    QWidget* productCard = new QWidget();
    productCard->setObjectName("vinileContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24,30,24,12);
    productLayout->setSpacing(10);

    QPixmap pix(QString::fromStdString(vinile->getImagePath()));
    if (pix.isNull()) pix.load(":/icons/placeholder.png");

    ClickableLabel* imageLabel = new ClickableLabel(productCard);
    imageLabel->setObjectName("vinileImageLabel");
    imageLabel->setPixmap(pix.scaled(180,180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setFixedSize(180,180);
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    connect(imageLabel, &ClickableLabel::clicked, this, [pix]() {
        QDialog dlg;
        dlg.setWindowTitle("Anteprima");
        QVBoxLayout L(&dlg);
        QLabel* big = new QLabel();
        big->setPixmap(pix.scaled(600,600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        big->setAlignment(Qt::AlignCenter);
        L.addWidget(big);
        dlg.exec();
    });

    QLabel* titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(vinile->getTitle()))
    );
    titleLabel->setObjectName("vinileTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    QLabel* availabilityLabel = new QLabel(
        vinile->getDisponibile() ? "Disponibile" : "Non disponibile"
    );
    availabilityLabel->setObjectName("vinileAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(availabilityLabel);

    {
        QWidget* row1 = new QWidget(productCard);
        QHBoxLayout* l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0,0,0,0);
        l1->setSpacing(20);

        l1->addWidget(new QLabel(
            "<b>Tipo:</b> " + QString::fromStdString(vinile->getTipoProdotto())
        ));
        l1->addWidget(new QLabel(
            "<b>Riconoscimento:</b> " + QString::fromStdString(vinile->getCodiceRiconoscimento())
        ));
        l1->addWidget(new QLabel(
            "<b>Produttore:</b> " + QString::fromStdString(vinile->getProduttoreStampe())
        ));

        productLayout->addWidget(row1, 0, Qt::AlignHCenter);
    }

    {
        QWidget* row2 = new QWidget(productCard);
        QHBoxLayout* l2 = new QHBoxLayout(row2);
        l2->setContentsMargins(0,0,0,0);
        l2->setSpacing(20);

        l2->addWidget(new QLabel(
            "<b>Prezzo:</b> " + QString("%1 €").arg(vinile->getPrezzo())
        ));

        productLayout->addWidget(row2, 0, Qt::AlignHCenter);
    }

    {
        QWidget* row3 = new QWidget(productCard);
        QHBoxLayout* l3 = new QHBoxLayout(row3);
        l3->setContentsMargins(0,0,0,0);
        l3->setSpacing(20);

        l3->addWidget(new QLabel(
            "<b>Diametro:</b> " + QString::number(vinile->getDiametro()) + " cm"
        ));
        l3->addWidget(new QLabel(
            "<b>RPM:</b> " + QString::number(vinile->getRpm())
        ));

        productLayout->addWidget(row3, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(6);
    QLabel* descLabel = new QLabel(QString::fromStdString(vinile->getDescription()));
    descLabel->setObjectName("vinileDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(6);

    // Sezione correlati 
    unsigned int artistId = vinile->getArtistId();

    QLabel* correlatiLabel = new QLabel("Prodotti correlati");
    correlatiLabel->setStyleSheet("font-weight: bold; font-size: 14pt; margin-top: 30px; margin-bottom: 20px;");
    productLayout->addWidget(correlatiLabel);

    QWidget* relatedProductsWidget = createRelatedProductsSection(artistId);
    productLayout->addWidget(relatedProductsWidget);

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);
}
// ------------------------------------


// ---------- musica ----------------
void VisitorGUI::visit(const Singolo* singolo) {
    clearLayout();
    qDebug() << "VisitorGUI::visit(Singolo)";

    QWidget* mainWrapper = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    QWidget* productCard = new QWidget;
    productCard->setObjectName("singoloContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24, 30, 24, 12);
    productLayout->setSpacing(10);

    QPixmap pix(QString::fromStdString(singolo->getImagePath()));
    if (pix.isNull()) pix.load(":/icons/placeholder.png");
    ClickableLabel* imageLabel = new ClickableLabel(productCard);
    imageLabel->setObjectName("singoloImageLabel");
    imageLabel->setPixmap(pix.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setFixedSize(180, 180);
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);
    connect(imageLabel, &ClickableLabel::clicked, this, [pix]() {
        QDialog dlg;
        dlg.setWindowTitle("Anteprima");
        QVBoxLayout L(&dlg);
        QLabel* big = new QLabel;
        big->setPixmap(pix.scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        big->setAlignment(Qt::AlignCenter);
        L.addWidget(big);
        dlg.exec();
    });

    QLabel* titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(singolo->getTitle()))
    );
    titleLabel->setObjectName("singoloTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    {
        QWidget* row1 = new QWidget(productCard);
        QHBoxLayout* l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0, 0, 0, 0);
        l1->setSpacing(20);
        l1->addWidget(new QLabel(
            "<b>Genere:</b> " + QString::fromStdString(singolo->getGenere())
        ));
        l1->addWidget(new QLabel(
            "<b>Versione:</b> " + QString(singolo->getIsRemix() ? "Remix" : "Original")
        ));
        l1->addWidget(new QLabel(
            "<b>Classifica:</b> " + QString::number(singolo->getChartPosition())
        ));
        productLayout->addWidget(row1, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(6);
    QLabel* descLabel = new QLabel(QString::fromStdString(singolo->getDescription()));
    descLabel->setObjectName("singoloDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(6);

    const Traccia& track = singolo->getMainTrack();
    QWidget* trackWidget = createTrackWidget(track);
    productLayout->addWidget(trackWidget);

    mainLayout->addWidget(productCard, 1);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);
}


void VisitorGUI::visit(const Album* album) {
    clearLayout();
    qDebug() << "VisitorGUI::visit(Album)";

    QWidget* mainWrapper = new QWidget;
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    QWidget* productCard = new QWidget;
    productCard->setObjectName("albumContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24, 30, 24, 12);
    productLayout->setSpacing(10);

    QPixmap pix(QString::fromStdString(album->getImagePath()));
    if (pix.isNull()) pix.load(":/icons/placeholder.png");
    ClickableLabel* imageLabel = new ClickableLabel(productCard);
    imageLabel->setObjectName("albumImageLabel");
    imageLabel->setPixmap(pix.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setFixedSize(180, 180);
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);
    connect(imageLabel, &ClickableLabel::clicked, this, [pix]() {
        QDialog dlg(nullptr);
        dlg.setWindowTitle("Anteprima Album");
        QVBoxLayout L(&dlg);
        QLabel* big = new QLabel;
        big->setPixmap(pix.scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        big->setAlignment(Qt::AlignCenter);
        L.addWidget(big);
        dlg.exec();
    });

    QLabel* titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(album->getTitle()))
    );
    titleLabel->setObjectName("albumTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    QLabel* desc = new QLabel(QString::fromStdString(album->getDescription()));
    desc->setObjectName("albumDescLabel");
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(desc);

    {
        QWidget* infoRow = new QWidget(productCard);
        QHBoxLayout* infoLayout = new QHBoxLayout(infoRow);
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoLayout->setSpacing(20);
        infoLayout->addWidget(new QLabel(
            "<b>Data Uscita:</b> " +
            QString::fromStdString(album->getDataUscita().toString())
        ));
        infoLayout->addWidget(new QLabel(
            "<b>Durata Totale:</b> " +
            QString::fromStdString(album->getDurata().toString())
        ));
        productLayout->addWidget(infoRow, 0, Qt::AlignHCenter);
    }

    for (const auto& track : album->getTracce()) {
        QWidget* trackWidget = createTrackWidget(track);
        productLayout->addWidget(trackWidget);
    }

    QLabel* labelLabel = new QLabel(
        "<b>Label:</b> " +
        QString::fromStdString(album->getLabel())
    );
    labelLabel->setObjectName("albumLabelLabel");
    labelLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(labelLabel);

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);
}

// ------------------------------------

// ---------- Tour ----------------
void VisitorGUI::visit(const Tour* tour) {
    clearLayout();
    qDebug() << "VisitorGUI::visit(Tour)";

    QWidget* mainWrapper = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8,8,8,8);
    mainLayout->setSpacing(12);

    QWidget* productCard = new QWidget();
    productCard->setObjectName("tourContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24,30,24,12);
    productLayout->setSpacing(10);

    QPixmap pix(QString::fromStdString(tour->getImagePath()));
    if (pix.isNull()) pix.load(":/icons/placeholder.png");

    ClickableLabel* imageLabel = new ClickableLabel(productCard);
    imageLabel->setObjectName("tourImageLabel");
    imageLabel->setPixmap(pix.scaled(180,180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->setFixedSize(180,180);
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    connect(imageLabel, &ClickableLabel::clicked, this, [pix]() {
        QDialog dlg;
        dlg.setWindowTitle("Anteprima");
        QVBoxLayout L(&dlg);
        QLabel* big = new QLabel();
        big->setPixmap(pix.scaled(600,600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        big->setAlignment(Qt::AlignCenter);
        L.addWidget(big);
        dlg.exec();
    });

    QLabel* titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(tour->getTitle()))
    );
    titleLabel->setObjectName("tourTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    QLabel* availabilityLabel = new QLabel(
        tour->getDisponibile() ? "Disponibile" : "Non disponibile"
    );
    availabilityLabel->setObjectName("tourAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(availabilityLabel);

    {
        QWidget* row1 = new QWidget(productCard);
        QHBoxLayout* l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0,0,0,0);
        l1->setSpacing(20);

        const auto& dateTour = tour->getDateTour();
        if (!dateTour.empty()) {
            const Data inizio = dateTour.front().getData();
            const Data fine   = dateTour.back().getData();
            l1->setContentsMargins(0,0,0,0);
            l1->setSpacing(20);

            l1->addWidget(new QLabel(
                "<b>Periodo:</b> " +
                QString::fromStdString(inizio.toString()) + " → " +
                QString::fromStdString(fine.toString())
            ));
        }

        l1->addWidget(new QLabel(
            "<b>Prezzo:</b> " + QString("%1 €").arg(tour->getPrezzo())
        ));

        productLayout->addWidget(row1, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(6);
    QLabel* descLabel = new QLabel(QString::fromStdString(tour->getDescription()));
    descLabel->setObjectName("tourDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(6);

    unsigned int artistId = tour->getArtistId();

    QLabel* correlatiLabel = new QLabel("Prodotti correlati");
    correlatiLabel->setStyleSheet("font-weight: bold; font-size: 14pt; margin-top: 30px; margin-bottom: 20px;");
    productLayout->addWidget(correlatiLabel);

    QWidget* relatedProductsWidget = createRelatedProductsSection(artistId);
    productLayout->addWidget(relatedProductsWidget);

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);
}

// ------------------------------------