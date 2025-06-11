#include "VisitorGUI.h"

#include <QHBoxLayout>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QGroupBox>
#include <QImage>
#include <QListWidget>
#include <QPushButton>
#include <QFile>
#include <QDialog>
#include <QTextEdit>
#include <QLayoutItem>
#include <QFrame>

VisitorGUI::VisitorGUI(const std::unordered_map<unsigned int, Artista *> *artistsMap, QObject *parent)
    : QObject(parent), m_artists(artistsMap)
{
    scrollContent = new QWidget(); // contenuto effettivo
    layout = new QVBoxLayout(scrollContent);
    scrollContent->setLayout(layout);

    scrollArea = new QScrollArea(); // area scrollabile
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollContent);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // opzionale
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);    // mostra solo se serve

    widget = scrollArea; // widget punta alla scrollArea
}

QWidget *VisitorGUI::getWidget() const
{
    return widget;
}

QWidget *VisitorGUI::createRelatedProductsSection(unsigned int artistId, unsigned int excludeProductId)
{
    auto *scroll = new QScrollArea;
    scroll->setObjectName("relatedProductsScrollArea");
    scroll->setWidgetResizable(true);
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget *container = new QWidget;
    container->setObjectName("relatedProductsContainer");
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto *hLayout = new QHBoxLayout(container);
    hLayout->setContentsMargins(12, 6, 12, 6);
    hLayout->setSpacing(24);
    hLayout->setSizeConstraint(QLayout::SetFixedSize);

    if (!m_artists)
    {
        scroll->setWidget(container);
        return scroll;
    }
    auto it = m_artists->find(artistId);
    if (it == m_artists->end() || !it->second)
    {
        scroll->setWidget(container);
        return scroll;
    }

    for (const auto &pp : it->second->getProducts())
    {
        unsigned int thisId = pp.first; // la chiave è l’ID
        if (thisId == excludeProductId)
            continue;

        ArtistProduct *prod = pp.second;
        QWidget *card = new QWidget;
        card->setObjectName("productCard");
        card->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        auto *vLayout = new QVBoxLayout(card);
        vLayout->setContentsMargins(16, 16, 16, 16);
        vLayout->setSpacing(16);
        vLayout->setSizeConstraint(QLayout::SetFixedSize);
        vLayout->setAlignment(Qt::AlignCenter);

        QLabel *thumb = new QLabel;
        thumb->setObjectName("productThumbnail");
        thumb->setFixedSize(120, 120);
        thumb->setAlignment(Qt::AlignCenter);

        QString imgPath = QString::fromStdString(prod->getImagePath());
        QPixmap original = loadPixmapOrPlaceholder(imgPath);
        QPixmap thumbPixmap = centerCropSquare(original, thumb->size());

        thumb->setPixmap(thumbPixmap);
        vLayout->addWidget(thumb);

        QLabel *title = new QLabel(QString::fromStdString(prod->getTitle()));
        title->setObjectName("productTitle");
        title->setWordWrap(true);
        title->setAlignment(Qt::AlignCenter);
        title->setFixedWidth(120);

        QPushButton *btn = new QPushButton("Apri", card);
        btn->setObjectName("productButton");
        btn->setFixedHeight(36);
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [=]()
                { prod->accept(this); });

        vLayout->addWidget(thumb);
        vLayout->addWidget(title);
        vLayout->addWidget(btn);
        hLayout->addWidget(card, 0, Qt::AlignTop);
    }

    scroll->setWidget(container);
    return scroll;
}

QWidget *VisitorGUI::createTrackWidget(const Traccia &track)
{
    QFrame *trackContainer = new QFrame;
    trackContainer->setObjectName("singoloTrackContainer");
    trackContainer->setFrameShape(QFrame::StyledPanel);
    trackContainer->setFrameShadow(QFrame::Raised);

    auto *layout = new QHBoxLayout(trackContainer);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(12);

    auto *infoWidget = new QWidget(trackContainer);
    auto *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(4);

    auto *titleBar = new QWidget(infoWidget);
    auto *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(8);

    auto *nameLabel = new QLabel(QString::fromStdString(track.getNome()), titleBar);
    nameLabel->setObjectName("trackNameLabel");
    QFont f = nameLabel->font();
    f.setPointSize(12);
    f.setBold(true);
    nameLabel->setFont(f);
    titleLayout->addWidget(nameLabel);

    if (!track.getPartecipanti().empty())
    {
        auto *featLabel = new QLabel("feat.", titleBar);
        featLabel->setObjectName("trackFeatLabel");
        QFont fl = featLabel->font();
        fl.setPointSize(10);
        fl.setBold(true);
        titleLayout->addWidget(featLabel);
    }

    for (const auto &artistName : track.getPartecipanti())
    {
        auto *partLabel = new QLabel(QString::fromStdString(artistName), titleBar);
        partLabel->setObjectName("trackParticipantLabel");
        titleLayout->addWidget(partLabel);
    }

    titleLayout->addStretch();

    infoLayout->addWidget(titleBar);

    auto *durLabel = new QLabel(QString::fromStdString(track.getDurata().toString()), infoWidget);
    durLabel->setObjectName("trackDurationLabel");
    f = durLabel->font();
    f.setPointSize(10);
    durLabel->setFont(f);
    durLabel->setStyleSheet("color: #555555;");
    infoLayout->addWidget(durLabel);

    layout->addWidget(infoWidget, 1);

    if (!track.getTesto().empty()) {
        auto *testoBtn = new QPushButton("Testo", trackContainer);
        testoBtn->setObjectName("textTrackButton");
        testoBtn->setCursor(Qt::PointingHandCursor);
        layout->addWidget(testoBtn, 0, Qt::AlignVCenter);

        connect(testoBtn, &QPushButton::clicked, this, [track, trackContainer]()
                {
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
            dlg.exec(); });
    }

    return trackContainer;
}

QWidget *VisitorGUI::createDateTourWidget(const DataTour &dt)
{
    QFrame *container = new QFrame;
    container->setObjectName("tourDateContainer");
    container->setFrameShape(QFrame::StyledPanel);
    container->setFrameShadow(QFrame::Raised);

    auto *layout = new QHBoxLayout(container);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(12);

    QWidget *infoWidget = new QWidget(container);
    auto *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(4);

    QLabel *dateLabel = new QLabel(
        QString("<b>Data:</b> %1")
            .arg(QString::fromStdString(dt.getData().toString())),
        infoWidget);
    dateLabel->setObjectName("tourDateLabel");
    infoLayout->addWidget(dateLabel);

    QLabel *timeLabel = new QLabel(
        QString("<b>Ora:</b> %1")
            .arg(QString::fromStdString(dt.toStringFormat())),
        infoWidget);
    timeLabel->setObjectName("tourTimeLabel");
    infoLayout->addWidget(timeLabel);

    QLabel *placeLabel = new QLabel(
        QString("<b>Luogo:</b> %1")
            .arg(QString::fromStdString(dt.getLuogo())),
        infoWidget);
    placeLabel->setObjectName("tourPlaceLabel");
    infoLayout->addWidget(placeLabel);

    layout->addWidget(infoWidget, 1);

    return container;
}

void VisitorGUI::clearLayout()
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr)
    {
        if (child->widget())
        {
            child->widget()->deleteLater();
        }
        delete child;
    }
}

// ---------------- VISITORS ----------------

// ------------- merch ----------------
void VisitorGUI::visit(const TShirt *tshirt)
{
    clearLayout();

    QWidget *mainWrapper = new QWidget;
    auto *mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    QWidget *productCard = new QWidget;
    productCard->setObjectName("tshirtContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24, 30, 24, 12);
    productLayout->setSpacing(12);

    QSize imageSize(180, 180);
    auto *imageLabel = createImageLabel(
        tshirt->getImagePath(), 
        imageSize,
        /*isArtist=*/false,
        productCard
    );
    imageLabel->setObjectName("tshirtImageLabel");
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    QLabel *titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(tshirt->getTitle())),
        productCard);
    titleLabel->setObjectName("tshirtTitleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    productLayout->addWidget(titleLabel);

    QLabel *availabilityLabel = new QLabel(
        tshirt->getDisponibile() ? "Disponibile" : "Sold Out",
        productCard);
    availabilityLabel->setObjectName("tshirtAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignCenter);
    productLayout->addWidget(availabilityLabel);

    {
        QWidget *tagPriceRow = new QWidget();
        QHBoxLayout *tpLayout = new QHBoxLayout(tagPriceRow);
        tpLayout->setContentsMargins(0, 0, 0, 0);
        tpLayout->setSpacing(12);

        tpLayout->addWidget(new QLabel(
            "<b>Taglia:</b> " + QString::fromStdString(tshirt->getTaglia())));
        tpLayout->addWidget(new QLabel(
            "<b>Colore:</b> " + QString::fromStdString(tshirt->getColore())));
        QLabel *priceLabel = new QLabel(
            "<b>Prezzo:</b> " + QString("%1 €").arg(tshirt->getPrezzo()));
        priceLabel->setObjectName("tshirtPriceLabel");
        tpLayout->addWidget(priceLabel, 0, Qt::AlignRight);

        productLayout->addWidget(tagPriceRow, 0, Qt::AlignHCenter);
    }

    QLabel *desc = new QLabel(QString::fromStdString(tshirt->getDescription()), productCard);
    desc->setObjectName("tshirtDescLabel");
    desc->setWordWrap(true);
    productLayout->addWidget(desc);

    unsigned int artistId = tshirt->getArtistId();
    unsigned int thisProductId = tshirt->getId();

    auto it = m_artists->find(artistId);
    if (it != m_artists->end() && it->second->getProducts().size() > 1) {
        QLabel *correlatiLabel = new QLabel("Prodotti correlati");
        correlatiLabel->setStyleSheet(
            "font-weight: bold; font-size: 14pt;"
            "margin-top: 30px; margin-bottom: 20px;"
        );
        correlatiLabel->setAlignment(Qt::AlignLeft);
        productLayout->addWidget(correlatiLabel);

        QWidget *related = createRelatedProductsSection(artistId, thisProductId);
        productLayout->addWidget(related);
    }

    productLayout->addStretch();

    mainLayout->addWidget(productCard, /*stretch=*/1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(mainWrapper);
}

void VisitorGUI::visit(const CD *cd)
{
    clearLayout();

    QWidget *mainWrapper = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    QWidget *productCard = new QWidget();
    productCard->setObjectName("cdContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24, 30, 24, 12);
    productLayout->setSpacing(12);

    QSize imageSize(180, 180);
    auto *imageLabel = createImageLabel(
        cd->getImagePath(),
        imageSize,
        /*isArtist=*/false,
        productCard
    );
    imageLabel->setObjectName("cdImageLabel");
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    QLabel *titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(cd->getTitle())));
    titleLabel->setObjectName("cdTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    QLabel *availabilityLabel = new QLabel(
        cd->getDisponibile() ? "Disponibile" : "Sold Out");
    availabilityLabel->setObjectName("cdAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(availabilityLabel);

    {
        QWidget *row1 = new QWidget(productCard);
        QHBoxLayout *l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0, 0, 0, 0);
        l1->setSpacing(12);

        l1->addWidget(new QLabel(
            "<b>Tipo:</b> " + QString::fromStdString(cd->getTipoProdotto())));
        l1->addWidget(new QLabel(
            "<b>Produttore:</b> " + QString::fromStdString(cd->getProduttoreStampe())));

        productLayout->addWidget(row1, 0, Qt::AlignHCenter);
    }

    {
        QWidget *row2 = new QWidget(productCard);
        QHBoxLayout *l2 = new QHBoxLayout(row2);
        l2->setContentsMargins(0, 0, 0, 0);
        l2->setSpacing(12);

        l2->addWidget(new QLabel(
            "<b>Prezzo:</b> " + QString("%1 €").arg(cd->getPrezzo())));

        productLayout->addWidget(row2, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(8);
    QLabel *descLabel = new QLabel(QString::fromStdString(cd->getDescription()));
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    descLabel->setObjectName("cdDescLabel");
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(12);

    // Sezione correlati
    unsigned int artistId = cd->getArtistId();
    unsigned int thisProductId = cd->getId();

    auto it = m_artists->find(artistId);
    if (it != m_artists->end() && it->second->getProducts().size() > 1) {
        QLabel *correlatiLabel = new QLabel("Prodotti correlati");
        correlatiLabel->setStyleSheet(
            "font-weight: bold; font-size: 14pt;"
            "margin-top: 30px; margin-bottom: 20px;"
        );
        correlatiLabel->setAlignment(Qt::AlignLeft);
        productLayout->addWidget(correlatiLabel);

        QWidget *related = createRelatedProductsSection(artistId, thisProductId);
        productLayout->addWidget(related);
    }

    productLayout->addStretch();

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(mainWrapper);
}

void VisitorGUI::visit(const Vinile *vinile)
{
    clearLayout();

    QWidget *mainWrapper = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    QWidget *productCard = new QWidget();
    productCard->setObjectName("vinileContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24, 30, 24, 12);
    productLayout->setSpacing(12);

    QSize imageSize(180, 180);
    auto *imageLabel = createImageLabel(
        vinile->getImagePath(),
        imageSize,
        /*isArtist=*/false,
        productCard
    );
    imageLabel->setObjectName("vinileImageLabel");
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);


    QLabel *titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(vinile->getTitle())));
    titleLabel->setObjectName("vinileTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    QLabel *availabilityLabel = new QLabel(
        vinile->getDisponibile() ? "Disponibile" : "Non disponibile");
    availabilityLabel->setObjectName("vinileAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(availabilityLabel);

    {
        QWidget *row1 = new QWidget(productCard);
        QHBoxLayout *l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0, 0, 0, 0);
        l1->setSpacing(12);

        l1->addWidget(new QLabel(
            "<b>Tipo:</b> " + QString::fromStdString(vinile->getTipoProdotto())));
        l1->addWidget(new QLabel(
            "<b>Riconoscimento:</b> " + QString::fromStdString(vinile->getCodiceRiconoscimento())));
        l1->addWidget(new QLabel(
            "<b>Produttore:</b> " + QString::fromStdString(vinile->getProduttoreStampe())));

        productLayout->addWidget(row1, 0, Qt::AlignHCenter);
    }

    {
        QWidget *row2 = new QWidget(productCard);
        QHBoxLayout *l2 = new QHBoxLayout(row2);
        l2->setContentsMargins(0, 0, 0, 0);
        l2->setSpacing(12);

        l2->addWidget(new QLabel(
            "<b>Prezzo:</b> " + QString("%1 €").arg(vinile->getPrezzo())));

        productLayout->addWidget(row2, 0, Qt::AlignHCenter);
    }

    {
        QWidget *row3 = new QWidget(productCard);
        QHBoxLayout *l3 = new QHBoxLayout(row3);
        l3->setContentsMargins(0, 0, 0, 0);
        l3->setSpacing(12);

        l3->addWidget(new QLabel(
            "<b>Diametro:</b> " + QString::number(vinile->getDiametro()) + " cm"));
        l3->addWidget(new QLabel(
            "<b>RPM:</b> " + QString::number(vinile->getRpm())));

        productLayout->addWidget(row3, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(12);
    QLabel *descLabel = new QLabel(QString::fromStdString(vinile->getDescription()));
    descLabel->setObjectName("vinileDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(12);

    // Sezione correlati
    unsigned int artistId = vinile->getArtistId();
    unsigned int thisProductId = vinile->getId();

    auto it = m_artists->find(artistId);
    if (it != m_artists->end() && it->second->getProducts().size() > 1) {
        QLabel *correlatiLabel = new QLabel("Prodotti correlati");
        correlatiLabel->setStyleSheet(
            "font-weight: bold; font-size: 14pt;"
            "margin-top: 30px; margin-bottom: 20px;"
        );
        correlatiLabel->setAlignment(Qt::AlignLeft);
        productLayout->addWidget(correlatiLabel);

        QWidget *related = createRelatedProductsSection(artistId, thisProductId);
        productLayout->addWidget(related);
    }

    productLayout->addStretch();

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(mainWrapper);
}
// ------------------------------------

// ---------- musica ----------------
void VisitorGUI::visit(const Singolo *singolo)
{
    clearLayout();

    QWidget *mainWrapper = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    QWidget *productCard = new QWidget;
    productCard->setObjectName("singoloContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24, 30, 24, 12);
    productLayout->setSpacing(12);

    QSize imageSize(180, 180);
    auto *imageLabel = createImageLabel(
        singolo->getImagePath(),
        imageSize,
        /*isArtist=*/false,
        productCard
    );
    imageLabel->setObjectName("singoloImageLabel");
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    QLabel *titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(singolo->getTitle())));
    titleLabel->setObjectName("singoloTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    {
        QWidget *row1 = new QWidget(productCard);
        QHBoxLayout *l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0, 0, 0, 0);
        l1->setSpacing(12);

        QLabel *genereLabel = new QLabel("<b>Genere:</b> " + QString::fromStdString(singolo->getGenere()));
        l1->addWidget(genereLabel);

        QLabel *versioneLabel = new QLabel("<b>Versione:</b> " + QString(singolo->getIsRemix() ? "Remix" : "Original"));
        l1->addWidget(versioneLabel);

        QLabel *chartLabel = new QLabel("<b>Classifica:</b> " + QString::number(singolo->getChartPosition()));
        l1->addWidget(chartLabel);

        productLayout->addWidget(row1, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(12);

    QLabel *descLabel = new QLabel(QString::fromStdString(singolo->getDescription()));
    descLabel->setObjectName("singoloDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);

    productLayout->addSpacing(12);

    const Traccia &track = singolo->getMainTrack();
    QWidget *trackWidget = createTrackWidget(track);
    productLayout->addWidget(trackWidget);

    // Sezione correlati
    unsigned int artistId = singolo->getArtistId();
    unsigned int thisProductId = singolo->getId();

    auto it = m_artists->find(artistId);
    if (it != m_artists->end() && it->second->getProducts().size() > 1) {
        QLabel *correlatiLabel = new QLabel("Prodotti correlati");
        correlatiLabel->setStyleSheet(
            "font-weight: bold; font-size: 14pt;"
            "margin-top: 30px; margin-bottom: 20px;"
        );
        correlatiLabel->setAlignment(Qt::AlignLeft);
        productLayout->addWidget(correlatiLabel);

        QWidget *related = createRelatedProductsSection(artistId, thisProductId);
        productLayout->addWidget(related);
    }

    productLayout->addStretch();

    mainLayout->addWidget(productCard, 1);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(mainWrapper);
}

void VisitorGUI::visit(const Album *album)
{
    clearLayout();

    QWidget *mainWrapper = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    QWidget *productCard = new QWidget;
    productCard->setObjectName("albumContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24, 30, 24, 12);
    productLayout->setSpacing(12);

    QSize imageSize(180, 180);
    auto *imageLabel = createImageLabel(
        album->getImagePath(),
        imageSize,
        /*isArtist=*/false,
        productCard
    );
    imageLabel->setObjectName("albumImageLabel");
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    QLabel *titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(album->getTitle())));
    titleLabel->setObjectName("albumTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    {
        QWidget *infoRow = new QWidget(productCard);
        QHBoxLayout *infoLayout = new QHBoxLayout(infoRow);
        infoLayout->setContentsMargins(0, 0, 0, 0);
        infoLayout->setSpacing(12);
        infoLayout->addWidget(new QLabel(
            "<b>Data Uscita:</b> " +
            QString::fromStdString(album->getDataUscita().toString())));
        infoLayout->addWidget(new QLabel(
            "<b>Durata Totale:</b> " +
            QString::fromStdString(album->getDurata().toString())));
        productLayout->addWidget(infoRow, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(12);

    QLabel *desc = new QLabel(QString::fromStdString(album->getDescription()));
    desc->setObjectName("albumDescLabel");
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(desc);

    for (const auto &track : album->getTracce())
    {
        QWidget *trackWidget = createTrackWidget(track);
        productLayout->addWidget(trackWidget);
    }

    productLayout->addSpacing(12);

    QString labelText = "<b>Label:</b> " + QString::fromStdString(album->getLabel());

    QLabel *labelLabel = new QLabel(labelText);
    labelLabel->setObjectName("albumLabelLabel");
    labelLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(labelLabel);

    productLayout->addStretch();

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(mainWrapper);
}

// ------------------------------------

// ---------- Tour ----------------
void VisitorGUI::visit(const Tour *tour)
{
    clearLayout();

    QWidget *mainWrapper = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWrapper);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    QWidget *productCard = new QWidget();
    productCard->setObjectName("tourContainer");
    productCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *productLayout = new QVBoxLayout(productCard);
    productLayout->setContentsMargins(24, 30, 24, 12);
    productLayout->setSpacing(12);

    QSize imageSize(180, 180);
    auto *imageLabel = createImageLabel(
        tour->getImagePath(),
        imageSize,
        /*isArtist=*/false,
        productCard
    );
    imageLabel->setObjectName("tourImageLabel");
    productLayout->addWidget(imageLabel, 0, Qt::AlignHCenter);

    QLabel *titleLabel = new QLabel(
        QString("<h2>%1</h2>").arg(QString::fromStdString(tour->getTitle())));
    titleLabel->setObjectName("tourTitleLabel");
    titleLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(titleLabel);

    QLabel *availabilityLabel = new QLabel(
        tour->getDisponibile() ? "Disponibile" : "Non disponibile");
    availabilityLabel->setObjectName("tourAvailabilityLabel");
    availabilityLabel->setAlignment(Qt::AlignHCenter);
    productLayout->addWidget(availabilityLabel);

    {
        QWidget *row1 = new QWidget(productCard);
        QHBoxLayout *l1 = new QHBoxLayout(row1);
        l1->setContentsMargins(0, 0, 0, 0);
        l1->setSpacing(12);

        const auto &dateTour = tour->getDateTour();
        if (!dateTour.empty())
        {
            const Data inizio = dateTour.front().getData();
            const Data fine = dateTour.back().getData();
            l1->setContentsMargins(0, 0, 0, 0);
            l1->setSpacing(12);

            l1->addWidget(new QLabel(
                "<b>Periodo:</b> " +
                QString::fromStdString(inizio.toString()) + " → " +
                QString::fromStdString(fine.toString())));
        }

        l1->addWidget(new QLabel(
            "<b>Prezzo:</b> " + QString("%1 €").arg(tour->getPrezzo())));

        productLayout->addWidget(row1, 0, Qt::AlignHCenter);
    }

    productLayout->addSpacing(12);
    QLabel *descLabel = new QLabel(QString::fromStdString(tour->getDescription()));
    descLabel->setObjectName("tourDescLabel");
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignLeft);
    productLayout->addWidget(descLabel);
    productLayout->addSpacing(12);

    for (const auto &dt : tour->getDateTour())
    {
        QWidget *dateWidget = createDateTourWidget(dt);
        productLayout->addWidget(dateWidget);
    }

    // Sezione correlati
    unsigned int artistId = tour->getArtistId();
    unsigned int thisProductId = tour->getId();

    auto it = m_artists->find(artistId);
    if (it != m_artists->end() && it->second->getProducts().size() > 1) {
        QLabel *correlatiLabel = new QLabel("Prodotti correlati");
        correlatiLabel->setStyleSheet(
            "font-weight: bold; font-size: 14pt;"
            "margin-top: 30px; margin-bottom: 20px;"
        );
        correlatiLabel->setAlignment(Qt::AlignLeft);
        productLayout->addWidget(correlatiLabel);

        QWidget *related = createRelatedProductsSection(artistId, thisProductId);
        productLayout->addWidget(related);
    }

    productLayout->addStretch();

    mainLayout->addWidget(productCard, /*stretch=*/1);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(mainWrapper);
}
// ------------------------------------