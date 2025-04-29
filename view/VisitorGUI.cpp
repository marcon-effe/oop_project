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
#include <QSpinBox>
#include <QComboBox>
#include <QDialog>

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

    // — Product card (left)
    QWidget* productCard = new QWidget();
    productCard->setObjectName("tshirtContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* productLayout = new QVBoxLayout(productCard);
    // margine superiore maggiore per “spingere” giù il contenuto
    productLayout->setContentsMargins(24, 30, 24, 12);
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

    // Title
    QLabel* titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(tshirt->getTitle()))
    );
    titleLabel->setObjectName("tshirtTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    // Disponibilità sotto il titolo
    QLabel* availabilityLabel = new QLabel(
        tshirt->getDisponibile() ? "Disponibile" : "Sold Out"
    );
    availabilityLabel->setObjectName("tshirtAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(availabilityLabel);

    {
        // Taglia · Colore · Prezzo (in grassetto)
        QWidget* tagPriceRow = new QWidget();
        QHBoxLayout* tpLayout = new QHBoxLayout(tagPriceRow);
        tpLayout->setContentsMargins(0,0,0,0);
        tpLayout->setSpacing(20);

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

    productLayout->addSpacing(8);
    QLabel* descLabel = new QLabel(QString::fromStdString(tshirt->getDescription()));
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    descLabel->setObjectName("tshirtDescLabel");
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(12);

    QPushButton* showPurchaseBtn = new QPushButton("Procedi", productCard);
    showPurchaseBtn->setObjectName("showDetailButton");
    showPurchaseBtn->setMaximumWidth(120);
    productLayout->addWidget(showPurchaseBtn, 0, Qt::AlignHCenter);

    QWidget* purchaseOptionsRow = new QWidget(productCard);
    purchaseOptionsRow->setVisible(false);
    QHBoxLayout* purchaseLayout = new QHBoxLayout(purchaseOptionsRow);
    purchaseLayout->setContentsMargins(0,0,0,0);
    purchaseLayout->setSpacing(16);
    purchaseLayout->setAlignment(Qt::AlignHCenter);

    purchaseLayout->addWidget(new QLabel("Quantità:"));
    QSpinBox* quantitySpinner = new QSpinBox();
    quantitySpinner->setRange(1, tshirt->getQuantita());
    quantitySpinner->setValue(1);
    purchaseLayout->addWidget(quantitySpinner);

    QPushButton* addToCartBtn = new QPushButton("Acquista");
    addToCartBtn->setObjectName("addCartButton");
    purchaseLayout->addWidget(addToCartBtn);
    productLayout->addWidget(purchaseOptionsRow);

    // — Signal/slot per mostrare/nascondere
    connect(showPurchaseBtn, &QPushButton::clicked, this, [showPurchaseBtn, purchaseOptionsRow](){
        showPurchaseBtn->setVisible(false);
        purchaseOptionsRow->setVisible(true);
    });

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

    // — Immagine cliccabile con preview
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

    QPushButton* showPurchaseBtn = new QPushButton("Procedi", productCard);
    showPurchaseBtn->setObjectName("showDetailButton");
    showPurchaseBtn->setMaximumWidth(120);
    productLayout->addWidget(showPurchaseBtn, 0, Qt::AlignHCenter);

    QWidget* purchaseOptionsRow = new QWidget(productCard);
    purchaseOptionsRow->setVisible(false);
    QHBoxLayout* purchaseLayout = new QHBoxLayout(purchaseOptionsRow);
    purchaseLayout->setContentsMargins(0,0,0,0);
    purchaseLayout->setSpacing(16);
    purchaseLayout->setAlignment(Qt::AlignHCenter);

    purchaseLayout->addWidget(new QLabel("Quantità:"));
    QSpinBox* quantitySpinner = new QSpinBox();
    quantitySpinner->setRange(1, cd->getQuantita());
    quantitySpinner->setValue(1);
    purchaseLayout->addWidget(quantitySpinner);

    QPushButton* addToCartBtn = new QPushButton("Acquista");
    addToCartBtn->setObjectName("addCartButton");
    purchaseLayout->addWidget(addToCartBtn);
    productLayout->addWidget(purchaseOptionsRow);

    connect(showPurchaseBtn, &QPushButton::clicked, this, [showPurchaseBtn, purchaseOptionsRow](){
        showPurchaseBtn->setVisible(false);
        purchaseOptionsRow->setVisible(true);
    });

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
        QString::fromStdString(vinile->getTitle())
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

    // — Descrizione
    productLayout->addSpacing(8);
    QLabel* descLabel = new QLabel(QString::fromStdString(vinile->getDescription()));
    descLabel->setObjectName("vinileDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(12);

    // — Pulsante “Procedi” e opzioni di acquisto
    QPushButton* showPurchaseBtn = new QPushButton("Procedi", productCard);
    showPurchaseBtn->setObjectName("showDetailButton");
    showPurchaseBtn->setMaximumWidth(120);
    productLayout->addWidget(showPurchaseBtn, 0, Qt::AlignHCenter);

    QWidget* purchaseOptionsRow = new QWidget(productCard);
    purchaseOptionsRow->setVisible(false);
    QHBoxLayout* purchaseLayout = new QHBoxLayout(purchaseOptionsRow);
    purchaseLayout->setContentsMargins(0,0,0,0);
    purchaseLayout->setSpacing(16);
    purchaseLayout->setAlignment(Qt::AlignHCenter);

    purchaseLayout->addWidget(new QLabel("Quantità:"));
    QSpinBox* quantitySpinner = new QSpinBox();
    quantitySpinner->setRange(1, vinile->getQuantita());
    quantitySpinner->setValue(1);
    purchaseLayout->addWidget(quantitySpinner);

    QPushButton* addToCartBtn = new QPushButton("Acquista");
    addToCartBtn->setObjectName("addCartButton");
    purchaseLayout->addWidget(addToCartBtn);
    productLayout->addWidget(purchaseOptionsRow);

    connect(showPurchaseBtn, &QPushButton::clicked, this, [showPurchaseBtn, purchaseOptionsRow]() {
        showPurchaseBtn->setVisible(false);
        purchaseOptionsRow->setVisible(true);
    });

    // — Inserisco la card nel mainLayout
    mainLayout->addWidget(productCard, /*stretch=*/1);

    // — Inserisco il mainWrapper nel layout di this
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);
}
// ------------------------------------


// ---------- musica ----------------
void VisitorGUI::visit(const Singolo* singolo) {
    clearLayout();
    qDebug() << "VisitorGUI::visit(Singolo)";

    // — Main wrapper
    QWidget* mainWrapper = new QWidget();
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8,8,8,8);
    mainLayout->setSpacing(12);

    // — Product card (left)
    QWidget* productCard = new QWidget();
    productCard->setObjectName("singoloContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout* productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24,30,24,12);
    productLayout->setSpacing(10);

    // — Immagine cliccabile con anteprima
    QPixmap pix(QString::fromStdString(singolo->getImagePath()));
    if (pix.isNull()) pix.load(":/icons/placeholder.png");

    ClickableLabel* imageLabel = new ClickableLabel(productCard);
    imageLabel->setObjectName("singoloImageLabel");
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

    // — Titolo Singolo
    QLabel* titleLabel = new QLabel(
        QString::fromStdString(singolo->getTitle())
    );
    titleLabel->setObjectName("singoloTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    // — Prima riga: Genere · Versione · Classifica
    {
        QWidget* row1 = new QWidget(productCard);
        QHBoxLayout* l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0,0,0,0);
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

    // — Descrizione
    productLayout->addSpacing(8);
    QLabel* descLabel = new QLabel(QString::fromStdString(singolo->getDescription()));
    descLabel->setObjectName("singoloDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(12);

    // — Info traccia principale (stile track)
    const Traccia& track = singolo->getMainTrack();
    QLabel* trackLabel = new QLabel(
        QString("<span style='font-size:14pt; font-weight:600;'>%1</span> ")
        .arg(QString::fromStdString(track.getNome())) +
        QString("<span style='font-size:10pt; color:#555;'>%1</span>")
        .arg(QString::fromStdString(track.getDurata().toString()))
    );
    trackLabel->setObjectName("singoloTrackInfoLabel");
    trackLabel->setAlignment(Qt::AlignCenter);
    productLayout->addWidget(trackLabel);

    // — Pulsante “Procedi” e opzioni di acquisto
    QPushButton* showPurchaseBtn = new QPushButton("Procedi", productCard);
    showPurchaseBtn->setObjectName("showDetailButton");
    showPurchaseBtn->setMaximumWidth(120);
    productLayout->addWidget(showPurchaseBtn, 0, Qt::AlignHCenter);

    QWidget* purchaseOptionsRow = new QWidget(productCard);
    purchaseOptionsRow->setVisible(false);
    QHBoxLayout* purchaseLayout = new QHBoxLayout(purchaseOptionsRow);
    purchaseLayout->setContentsMargins(0,0,0,0);
    purchaseLayout->setSpacing(16);
    purchaseLayout->setAlignment(Qt::AlignHCenter);

    /* purchaseLayout->addWidget(new QLabel("Quantità:"));
    QSpinBox* quantitySpinner = new QSpinBox();
    quantitySpinner->setRange(1, singolo->getQuantita());
    quantitySpinner->setValue(1);
    purchaseLayout->addWidget(quantitySpinner); */

    QPushButton* addToCartBtn = new QPushButton("Acquista");
    addToCartBtn->setObjectName("addCartButton");
    purchaseLayout->addWidget(addToCartBtn);
    productLayout->addWidget(purchaseOptionsRow);

    connect(showPurchaseBtn, &QPushButton::clicked, this, [showPurchaseBtn, purchaseOptionsRow]() {
        showPurchaseBtn->setVisible(false);
        purchaseOptionsRow->setVisible(true);
    });

    // — Inserisco la card nel mainLayout
    mainLayout->addWidget(productCard, /*stretch=*/1);

    // — Inserisco il mainWrapper
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);
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

    // — Titolo
    QLabel* titleLabel = new QLabel(QString::fromStdString(tour->getTitle()));
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

    productLayout->addSpacing(8);
    QLabel* descLabel = new QLabel(QString::fromStdString(tour->getDescription()));
    descLabel->setObjectName("tourDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(12);

    QPushButton* showPurchaseBtn = new QPushButton("Procedi", productCard);
    showPurchaseBtn->setObjectName("showDetailButton");
    showPurchaseBtn->setMaximumWidth(120);
    productLayout->addWidget(showPurchaseBtn, 0, Qt::AlignHCenter);

    QWidget* purchaseOptionsRow = new QWidget(productCard);
    purchaseOptionsRow->setVisible(false);
    QHBoxLayout* purchaseLayout = new QHBoxLayout(purchaseOptionsRow);
    purchaseLayout->setContentsMargins(0,0,0,0);
    purchaseLayout->setSpacing(16);
    purchaseLayout->setAlignment(Qt::AlignHCenter);

    purchaseLayout->addWidget(new QLabel("Quantità:"));
    QSpinBox* quantitySpinner = new QSpinBox();
    quantitySpinner->setRange(1, tour->getQuantita());
    quantitySpinner->setValue(1);
    purchaseLayout->addWidget(quantitySpinner);

    QPushButton* addToCartBtn = new QPushButton("Acquista");
    addToCartBtn->setObjectName("addCartButton");
    purchaseLayout->addWidget(addToCartBtn);
    productLayout->addWidget(purchaseOptionsRow);

    connect(showPurchaseBtn, &QPushButton::clicked, this, [showPurchaseBtn, purchaseOptionsRow]() {
        showPurchaseBtn->setVisible(false);
        purchaseOptionsRow->setVisible(true);
    });

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(mainWrapper);
}

// ------------------------------------