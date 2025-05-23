#include "ArtistaDeleteDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include "../../data/DataManager.h"

ArtistaDeleteDialog::ArtistaDeleteDialog(
    std::unordered_map<unsigned int, Artista*>& artistiRef,
    std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
    QWidget* parent
) : QDialog(parent), artisti(artistiRef), prodotti(prodottiRef)
{
    this->setObjectName("artistaDeleteDialog");

    setWindowTitle("Eliminazione");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20,20,20,20);
    layout->setSpacing(12);

    QLabel* info = new QLabel("Seleziona l'artista da eliminare:", this);
    layout->addWidget(info);

    artistaComboBox = new QComboBox(this);
    artistaComboBox->setIconSize(QSize(32,32));
    for (const auto& [id, artista] : artisti) {
        artistaComboBox->addItem(
            QString::fromStdString(artista->getNome()),
            QVariant::fromValue(id)
        );
    }
    layout->addWidget(artistaComboBox);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QPushButton* ok = buttonBox->button(QDialogButtonBox::Ok);
    ok->setObjectName("okButton");
    QPushButton* cancel = buttonBox->button(QDialogButtonBox::Cancel);
    cancel->setObjectName("cancelButton");

    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool ArtistaDeleteDialog::esegui() {
    if (exec() != QDialog::Accepted)
        return false;

    if (artistaComboBox->currentIndex() < 0)
        return false;

    unsigned int id = artistaComboBox->currentData().toUInt();
    auto it = artisti.find(id);
    if (it == artisti.end())
        return false;

    Artista* artista = it->second;

    QString msg = "Sei sicuro di voler eliminare l'artista \"" +
                  QString::fromStdString(artista->getNome()) + "\"?";
    if (QMessageBox::question(this, "Conferma eliminazione", msg) != QMessageBox::Yes)
        return false;

    eliminaArtista(artista);
    return true;
}

void ArtistaDeleteDialog::eliminaArtista(Artista* artista) {
    // Rimuove e dealloca tutti i prodotti tramite il metodo proprietario
    for (const auto& [id, _] : artista->getProducts()) {
        artista->removeProduct(id);
        prodotti.erase(id);
    }

    // Elimina la directory dell'artista (solo se esiste)
    QString dirPath = "view/icons/" + QString::fromStdString(DataManager::sanitizeForPath(artista->getNome()));
    QDir dir(dirPath);
    if (dir.exists()) {
        dir.removeRecursively();
    }

    // Rimuove artista dalla mappa globale e libera la memoria
    artisti.erase(artista->getId());
    delete artista;
}