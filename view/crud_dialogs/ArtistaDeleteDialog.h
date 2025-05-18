#ifndef ARTISTADELETEDIALOG_H
#define ARTISTADELETEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>

#include <unordered_map>

#include "../../model/artisti/Artista.h"
#include "../../model/core/ArtistProduct.h"

class ArtistaDeleteDialog : public QDialog {
    Q_OBJECT

public:
    ArtistaDeleteDialog(std::unordered_map<unsigned int, Artista*>& artistiRef,
                        std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
                        QWidget* parent = nullptr);

    // Avvia il dialogo e, se confermato, esegue l'eliminazione dell'artista
    bool esegui();

private:
    std::unordered_map<unsigned int, Artista*>& artisti;
    std::unordered_map<unsigned int, ArtistProduct*>& prodotti;

    QComboBox* artistaComboBox;
    QDialogButtonBox* buttonBox;

    void eliminaArtista(Artista* artista);
};

#endif // ARTISTADELETEDIALOG_H