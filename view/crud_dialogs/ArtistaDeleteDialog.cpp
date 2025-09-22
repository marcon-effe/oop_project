// ---------------
// Implementa una finestra di dialogo (QDialog) che consente di selezionare un artista
// e confermarne l’eliminazione. L’operazione rimuove:
//   - tutti i prodotti associati all’artista sia dalla relazione interna dell’artista
//     sia dalla mappa globale dei prodotti;
//   - la cartella immagini/icone dell’artista sul filesystem (se presente);
//   - l’artista dalla mappa globale, deallocandone la memoria.
//
// Note su proprietà e responsabilità:
// - Le mappe `artisti` e `prodotti` sono passate per riferimento nel costruttore e
//   rappresentano lo stato “globale” del modello gestito dall’applicazione.
// - La memoria dell’oggetto Artista selezionato viene deallocata in questa unità,
//   dopo averlo rimosso dalle strutture dati.
// - Le widget Qt create nel costruttore hanno come parent `this`, quindi la loro
//   memoria è gestita automaticamente dal sistema di parentela Qt.
//
// Sicurezza dell’operazione di cancellazione:
// - La rimozione dei prodotti è fatta in due fasi (snapshot degli ID -> rimozione)
//   per evitare invalidazione degli iteratori e potenziali crash.
// - Il nome dell’artista viene sanitizzato prima di costruire il path sul filesystem,
//   in modo da evitare caratteri non validi nella directory.
//
// Thread-safety:
// - Le operazioni qui svolte sono pensate per essere eseguite nel thread GUI.
//   Non ci sono sincronizzazioni per l’accesso concorrente alle mappe.
//

#include "ArtistaDeleteDialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QSize>
#include <QVariant>

#include "../../data/DataManager.h"
// Se le definizioni complete non sono già visibili al linkatore, includere le rispettive intestazioni:
// #include "../../model/artisti/Artista.h"
// #include "../../model/core/ArtistProduct.h"

#include <vector>
#include <unordered_map>

// Funzione di utilità interna al file (non esposta nell'header).
// Esegue davvero la cancellazione dell’artista e delle sue risorse.
//
// Precondizioni:
//  - `artista` deve puntare a un elemento presente in `artisti`.
// Postcondizioni:
//  - tutti i prodotti dell’artista sono rimossi dalla relazione dell’artista e dalla
//    mappa globale `prodotti`;
//  - la directory "view/icons/<nome-sanitizzato>" viene rimossa se esiste;
//  - l’artista viene tolto da `artisti` e deallocato.
//
// Motivazione dell’algoritmo:
//  - Si effettua prima uno snapshot degli ID dei prodotti per non iterare e cancellare
//    contemporaneamente sulla stessa struttura (evitando invalidazioni/segfault).
namespace {
void eliminaArtistaImpl(std::unordered_map<unsigned int, Artista*>& artisti,
                        std::unordered_map<unsigned int, ArtistProduct*>& prodotti,
                        Artista* artista)
{
    if (!artista) return;

    const unsigned int artistId = artista->getId();

    // 1) Snapshot degli ID dei prodotti dell’artista.
    std::vector<unsigned int> productIds;
    productIds.reserve(artista->getProducts().size());
    for (const auto& kv : artista->getProducts()) {
        productIds.push_back(kv.first);
    }

    // 2) Rimozione dei prodotti (dalla relazione dell’artista e dalla mappa globale).
    for (unsigned int pid : productIds) {
        artista->removeProduct(pid);
        prodotti.erase(pid);
    }

    // 3) Pulizia risorse su disco (cartella icone/immagini dedicata all’artista).
    const QString dirPath =
        "view/icons/" + QString::fromStdString(DataManager::sanitizeForPath(artista->getNome()));
    QDir dir(dirPath);
    if (dir.exists()) {
        dir.removeRecursively();
    }

    // 4) Rimozione dalla mappa globale e deallocazione della memoria.
    artisti.erase(artistId);
    delete artista;
}
} // namespace

// Costruisce la dialog, popola la combo con (nome, id) di ciascun artista e
// prepara i pulsanti Ok/Cancel.
ArtistaDeleteDialog::ArtistaDeleteDialog(
    std::unordered_map<unsigned int, Artista*>& artistiRef,
    std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
    QWidget* parent
) : QDialog(parent), artisti(artistiRef), prodotti(prodottiRef)
{
    this->setObjectName("artistaDeleteDialog");
    setWindowTitle("Eliminazione");

    // Layout verticale principale.
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    // Istruzioni per l’utente.
    auto* info = new QLabel("Seleziona l'artista da eliminare:", this);
    layout->addWidget(info);

    // Selettore artista: mostra i nomi ma conserva l’ID come userData.
    artistaComboBox = new QComboBox(this);
    artistaComboBox->setIconSize(QSize(32, 32));
    for (const auto& [id, artista] : artisti) {
        // Nota: QVariant::fromValue(unsigned int) -> toUInt() in lettura.
        artistaComboBox->addItem(
            QString::fromStdString(artista->getNome()),
            QVariant::fromValue(id)
        );
    }
    layout->addWidget(artistaComboBox);

    // Box pulsanti standard Ok/Cancel.
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    // Accesso ai pulsanti per eventuale styling/test.
    auto* ok = buttonBox->button(QDialogButtonBox::Ok);
    ok->setObjectName("okButton");
    auto* cancel = buttonBox->button(QDialogButtonBox::Cancel);
    cancel->setObjectName("cancelButton");

    layout->addWidget(buttonBox);

    // Connessioni standard: Ok => accept(), Cancel => reject().
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

// Mostra la dialog in modo modale, valida la selezione, chiede conferma e,
// in caso affermativo, esegue l’eliminazione. Ritorna true se l’operazione
// è stata effettuata, false in caso di annullamento o condizioni non valide.
bool ArtistaDeleteDialog::esegui() {
    // L’utente ha annullato o chiuso la dialog?
    if (exec() != QDialog::Accepted)
        return false;

    // Nessuna selezione valida in combo.
    if (artistaComboBox->currentIndex() < 0)
        return false;

    // Recupero dell’ID dall’userData della voce selezionata.
    const unsigned int id = artistaComboBox->currentData().toUInt();

    // Verifica che l’artista esista ancora nella mappa.
    auto it = artisti.find(id);
    if (it == artisti.end())
        return false;

    Artista* artista = it->second;

    // Conferma esplicita da parte dell’utente.
    const QString msg = "Sei sicuro di voler eliminare l'artista \""
                      + QString::fromStdString(artista->getNome()) + "\"?";
    if (QMessageBox::question(this, "Conferma eliminazione", msg) != QMessageBox::Yes)
        return false;

    // Esecuzione effettiva della cancellazione (vedi note in testa al file).
    eliminaArtistaImpl(artisti, prodotti, artista);
    return true;
}