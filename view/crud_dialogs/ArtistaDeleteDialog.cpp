#include "ArtistaDeleteDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFile>

#include "../../data/DataManager.h"

ArtistaDeleteDialog::ArtistaDeleteDialog(
    std::unordered_map<unsigned int, Artista*>& artistiRef,
    std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
    QWidget* parent
) : QDialog(parent), artisti(artistiRef), prodotti(prodottiRef)
{
    setWindowTitle("Elimina artista");

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Seleziona l'artista da eliminare:"));

    artistaComboBox = new QComboBox(this);
    for (const auto& [id, artista] : artisti) {
        artistaComboBox->addItem(QString::fromStdString(artista->getNome()), QVariant::fromValue(id));
    }
    layout->addWidget(artistaComboBox);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
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