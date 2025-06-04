#include "ArtistWidget.h"
#include "ClickableLabel.h"     // per createImageLabel
#include "visitors/VisitorGUI.h" // (serve solo se usi VisitorGUI internamente, ma non serve qui)
#include <QFile>
#include <QPainterPath>
#include <QPainter>
#include <QListWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

ArtistWidget::ArtistWidget(const std::unordered_map<unsigned int, Artista*>* artistsMap,
                           QObject* parent)
    : QObject(parent),
      m_artists(artistsMap)
{
    scrollContent = new QWidget();
    layout        = new QVBoxLayout(scrollContent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    scrollContent->setLayout(layout);

    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollContent);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    widget = scrollArea;
}

QWidget* ArtistWidget::getWidget() const {
    return widget;
}

void ArtistWidget::clearLayout()
{
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }
}

void ArtistWidget::showArtista(const Artista* artista, QWidget* parent)
{
    clearLayout();

    QWidget* container = new QWidget(scrollContent);
    container->setObjectName("artistContainer");

    QVBoxLayout* vbox = new QVBoxLayout(container);
    vbox->setContentsMargins(24, 30, 24, 12);
    vbox->setSpacing(12);

    QSize artistImgSize(200, 200);
    QLabel* imageLabel = createImageLabel(
        artista->getImagePath(),
        artistImgSize,
        /*isArtist=*/true,
        container
    );
    imageLabel->setObjectName("artistImage");
    vbox->addWidget(imageLabel, 0, Qt::AlignHCenter);

    QLabel* nameLabel = new QLabel(QString::fromStdString(artista->getNome()), container);
    nameLabel->setObjectName("artistName");
    nameLabel->setAlignment(Qt::AlignCenter);
    vbox->addWidget(nameLabel);

    QLabel* genreLabel = new QLabel(QString::fromStdString(artista->getGenere()), container);
    genreLabel->setObjectName("artistGenre");
    genreLabel->setAlignment(Qt::AlignCenter);
    vbox->addWidget(genreLabel);

    QGroupBox* descBox = new QGroupBox("Descrizione", container);
    descBox->setObjectName("artistDescBox");
    descBox->setAlignment(Qt::AlignCenter);
    QVBoxLayout* descLayout = new QVBoxLayout(descBox);
    QLabel* descLabel = new QLabel(QString::fromStdString(artista->getInfo()), descBox);
    descLabel->setWordWrap(true);
    descLayout->addWidget(descLabel);
    vbox->addWidget(descBox);

    QGroupBox* prodBox = new QGroupBox("Prodotti", container);
    prodBox->setObjectName("artistProdBox");
    QVBoxLayout* prodLayout = new QVBoxLayout(prodBox);
    QListWidget* prodList = new QListWidget(prodBox);
    prodList->setObjectName("artistProdList");
    prodList->setSelectionMode(QAbstractItemView::NoSelection);
    prodList->setSpacing(12);
    prodList->setUniformItemSizes(false);
    prodLayout->addWidget(prodList);

    for (const auto& pair : artista->getProducts()) {
        ArtistProduct* p = pair.second;

        QListWidgetItem* item = new QListWidgetItem(prodList);
        item->setSizeHint(QSize(0, 100));

        QWidget* card = new QWidget();
        card->setObjectName("productCard");
        QHBoxLayout* h = new QHBoxLayout(card);
        h->setContentsMargins(10, 10, 10, 10);
        h->setSpacing(12);

        QSize thumbSize(80, 80);
        QLabel* thumb = createImageLabel(
            p->getImagePath(),
            thumbSize,
            /*isArtist=*/false,
            card
        );
        thumb->setObjectName("productThumb");
        h->addWidget(thumb);

        QVBoxLayout* info = new QVBoxLayout();
        QLabel* title = new QLabel(QString::fromStdString(p->getTitle()), card);
        title->setObjectName("productTitle");
        title->setWordWrap(true);
        info->addWidget(title);

        if (auto descr = p->getDescription(); !descr.empty()) {
            QLabel* desc2 = new QLabel(QString::fromStdString(descr), card);
            desc2->setObjectName("productDesc");
            desc2->setWordWrap(true);
            info->addWidget(desc2);
        }
        info->addStretch();
        h->addLayout(info, /*stretch=*/1);

        QPushButton* btn = new QPushButton("Dettagli", card);
        btn->setObjectName("productButton");
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [p, this]() {
            emit prodottoSelezionato(p);
        });
        h->addWidget(btn, /*stretch=*/0, Qt::AlignVCenter);

        prodList->setItemWidget(item, card);
    }

    vbox->addWidget(prodBox);
    layout->addWidget(container);
}