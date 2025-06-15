#ifndef PRODOTTODELETEDIALOG_H
#define PRODOTTODELETEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <unordered_map>
#include "../../model/artisti/Artista.h"
#include "../../model/core/ArtistProduct.h"

class ProdottoDeleteDialog : public QDialog {
    Q_OBJECT

public:
    ProdottoDeleteDialog(std::unordered_map<unsigned int, Artista*>& artistiRef,
                         std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
                         QWidget* parent = nullptr);

    // Avvia il dialogo e, se confermato, esegue l'eliminazione del prodotto
    bool esegui();

private:
    std::unordered_map<unsigned int, Artista*>& artisti;
    std::unordered_map<unsigned int, ArtistProduct*>& prodotti;

    QComboBox* artistaComboBox;
    QComboBox* prodottoComboBox;
    QDialogButtonBox* buttonBox;

    void aggiornaProdotti();
    void eliminaProdotto(ArtistProduct* prodotto);
};

#endif // PRODOTTODELETEDIALOG_H