#include "FilterDialog.h"
#include "../../model/artisti/Artista.h"
#include "../../model/core/ArtistProduct.h"
#include "../../model/musica/Album.h"
#include "../../model/musica/Singolo.h"
#include "../../model/merch/CD.h"
#include "../../model/merch/Vinile.h"
#include "../../model/merch/TShirt.h"
#include "../../model/tour/Tour.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QSet>

FilterDialog::FilterDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Filtra elementi");
    resize(400, 300);
    mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
}

void FilterDialog::setupForArtisti(const std::unordered_map<unsigned int, Artista*>& artists) {
    QLabel* label = new QLabel("Seleziona un genere:", this);
    genereCombo = new QComboBox(this);

    QSet<QString> generi;
    for (const auto& pair : artists) {
        generi.insert(QString::fromStdString(pair.second->getGenere()));
    }

    genereCombo->addItem("Tutti");
    for (const QString& g : generi) {
        genereCombo->addItem(g);
    }

    mainLayout->addWidget(label);
    mainLayout->addWidget(genereCombo);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FilterDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &FilterDialog::reject);

    mainLayout->addWidget(buttonBox);
}

void FilterDialog::setupForProdotti(const std::unordered_map<unsigned int, ArtistProduct*>& prodotti, const std::unordered_map<unsigned int, Artista*>& artisti) {
    // Sezione: artista
    QLabel* artistaLabel = new QLabel("Filtra per artista:", this);
    artistaCombo = new QComboBox(this);
    artistaCombo->addItem("Tutti");
    for (const auto& pair : artisti) {
        QString nome = QString::fromStdString(pair.second->getNome());
        artistaCombo->addItem(nome);
        artistaNomeToId[nome] = pair.first;
    }
    mainLayout->addWidget(artistaLabel);
    mainLayout->addWidget(artistaCombo);

    // Sezione: prodotti musicali
    QLabel* musicaLabel = new QLabel("Prodotti musicali:", this);
    QGroupBox* musicaGroup = new QGroupBox(this);
    QVBoxLayout* musicaLayout = new QVBoxLayout(musicaGroup);

    albumCheck = new QCheckBox("Album", this);
    singoloCheck = new QCheckBox("Singolo", this);

    QSet<QString> generi;
    for (const auto& pair : prodotti) {
        if (auto* m = dynamic_cast<Musica*>(pair.second)) {
            generi.insert(QString::fromStdString(m->getGenere()));
        }
    }

    genereMusicaCombo = new QComboBox(this);
    genereMusicaCombo->addItem("Tutti");
    for (const QString& g : generi)
        genereMusicaCombo->addItem(g);

    musicaLayout->addWidget(albumCheck);
    musicaLayout->addWidget(singoloCheck);
    musicaLayout->addWidget(new QLabel("Genere:", this));
    musicaLayout->addWidget(genereMusicaCombo);
    musicaGroup->setLayout(musicaLayout);

    mainLayout->addWidget(musicaLabel);
    mainLayout->addWidget(musicaGroup);

    // Sezione: prodotti NotMusica
    QLabel* notMusicaLabel = new QLabel("Prodotti non musicali:", this);
    QGroupBox* notMusicaGroup = new QGroupBox(this);
    QVBoxLayout* notMusicaLayout = new QVBoxLayout(notMusicaGroup);

    cdCheck = new QCheckBox("CD", this);
    vinileCheck = new QCheckBox("Vinile", this);
    tshirtCheck = new QCheckBox("TShirt", this);
    tourCheck = new QCheckBox("Tour", this);
    disponibileCheck = new QCheckBox("Disponibile", this);

    notMusicaLayout->addWidget(cdCheck);
    notMusicaLayout->addWidget(vinileCheck);
    notMusicaLayout->addWidget(tshirtCheck);
    notMusicaLayout->addWidget(tourCheck);
    notMusicaLayout->addWidget(disponibileCheck);
    notMusicaGroup->setLayout(notMusicaLayout);

    mainLayout->addWidget(notMusicaLabel);
    mainLayout->addWidget(notMusicaGroup);

    // Pulsanti OK / Cancel
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FilterDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &FilterDialog::reject);

    mainLayout->addWidget(buttonBox);
}

QString FilterDialog::getSelectedGenere() const {
    return genereCombo ? genereCombo->currentText() : "Tutti";
}

QString FilterDialog::getSelectedGenereMusica() const {
    return genereMusicaCombo ? genereMusicaCombo->currentText() : "Tutti";
}

std::vector<std::string> FilterDialog::getSelectedTipoProdotto() const {
    std::vector<std::string> tipi;

    if (albumCheck && albumCheck->isChecked())
        tipi.emplace_back("Album");
    if (singoloCheck && singoloCheck->isChecked())
        tipi.emplace_back("Singolo");
    if (cdCheck && cdCheck->isChecked())
        tipi.emplace_back("CD");
    if (vinileCheck && vinileCheck->isChecked())
        tipi.emplace_back("Vinile");
    if (tshirtCheck && tshirtCheck->isChecked())
        tipi.emplace_back("TShirt");
    if (tourCheck && tourCheck->isChecked())
        tipi.emplace_back("Tour");

    return tipi;
}

bool FilterDialog::isDisponibileChecked() const {
    return disponibileCheck ? disponibileCheck->isChecked() : false;
}

QString FilterDialog::getSelectedArtistaId() const {
    if (!artistaCombo || artistaCombo->currentText() == "Tutti") return "Tutti";
    auto it = artistaNomeToId.find(artistaCombo->currentText());
    return QString::fromStdString(it != artistaNomeToId.end() ? it->second : "");
}