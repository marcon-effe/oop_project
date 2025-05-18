#include "ProdottoDeleteDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include "../../data/DataManager.h"

ProdottoDeleteDialog::ProdottoDeleteDialog(
    std::unordered_map<unsigned int, Artista*>& artistiRef,
    std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
    QWidget* parent
)
    : QDialog(parent), artisti(artistiRef), prodotti(prodottiRef)
{
    setWindowTitle("Elimina prodotto");

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Seleziona l'artista:"));
    artistaComboBox = new QComboBox(this);
    for (const auto& [id, artista] : artisti)
        artistaComboBox->addItem(QString::fromStdString(artista->getNome()), QVariant::fromValue(id));
    layout->addWidget(artistaComboBox);

    layout->addWidget(new QLabel("Seleziona il prodotto:"));
    prodottoComboBox = new QComboBox(this);
    layout->addWidget(prodottoComboBox);

    connect(artistaComboBox, &QComboBox::currentIndexChanged, this, &ProdottoDeleteDialog::aggiornaProdotti);
    artistaComboBox->setCurrentIndex(0);  // forza aggiornamento
    aggiornaProdotti();

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ProdottoDeleteDialog::aggiornaProdotti() {
    prodottoComboBox->clear();
    if (artistaComboBox->currentIndex() < 0) return;

    unsigned int artistId = artistaComboBox->currentData().toUInt();
    Artista* artista = artisti.at(artistId);
    for (const auto& [id, prodotto] : artista->getProducts()) {
        prodottoComboBox->addItem(QString::fromStdString(prodotto->getTitle()), QVariant::fromValue(id));
    }
}

bool ProdottoDeleteDialog::esegui() {
    if (exec() != QDialog::Accepted)
        return false;

    if (prodottoComboBox->currentIndex() < 0)
        return false;

    QVariant data = prodottoComboBox->currentData();
    if (!data.isValid())
        return false;

    unsigned int prodottoId = data.toUInt();
    auto it = prodotti.find(prodottoId);
    if (it == prodotti.end())
        return false;

    ArtistProduct* prodotto = it->second;
    if (!prodotto)
        return false;

    QString msg = "Sei sicuro di voler eliminare il prodotto \"" +
                  QString::fromStdString(prodotto->getTitle()) + "\"?";
    if (QMessageBox::question(this, "Conferma eliminazione", msg) != QMessageBox::Yes)
        return false;

    eliminaProdotto(prodotto);
    return true;
}

void ProdottoDeleteDialog::eliminaProdotto(ArtistProduct* prodotto) {
    if (!prodotto) return;

    unsigned int id = prodotto->getId();
    unsigned int artistId = prodotto->getArtistId();

    auto itArtista = artisti.find(artistId);
    if (itArtista != artisti.end()) {
        itArtista->second->removeProduct(id);
    }

    prodotti.erase(id);
}