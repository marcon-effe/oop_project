#ifndef ARTISTWIDGET_H
#define ARTISTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QListWidget>
#include <unordered_map>

// Includo le utility per le immagini
#include "ImageHelper.h"

// Forward declaration delle classi di modello
class Artista;
class ArtistProduct;
class Traccia;
class DataTour;

///
/// ArtistWidget si occupa di creare dinamicamente i widget per:
///   – Dettagli di un artista (immagine, nome, genere, descrizione, prodotti, ecc.)
///   – Emette un segnale quando l’utente clicca “Dettagli” su un prodotto
///   – Al click del segnale, MainWindow creerà il VisitorGUI per quel prodotto
///
class ArtistWidget : public QObject
{
    Q_OBJECT                   // <— Assicurati che questa riga sia presente

public:
    /// Costruttore: riceve subito la mappa di tutti gli Artista*
    explicit ArtistWidget(const std::unordered_map<unsigned int, Artista*>* artistsMap,
                          QObject* parent = nullptr);
    ~ArtistWidget() override = default;

    /// Restituisce il widget principale (una QScrollArea) da inserire nella UI
    QWidget* getWidget() const;

    /// Popola il contenitore interno con i dettagli di “artista”.
    /// Non passare qui il parent di MainWindow: container verrà figlio di scrollContent.
    void showArtista(const Artista* artista, QWidget* parent);

signals:
    /// Emesso quando l’utente clicca “Dettagli” su un prodotto.
    /// Informa il MainWindow quale ArtistProduct è stato selezionato.
    void prodottoSelezionato(ArtistProduct* prodotto);

private:
    const std::unordered_map<unsigned int, Artista*>* m_artists;

    QWidget*       scrollContent;  ///< QWidget che contiene i child dinamici
    QVBoxLayout*   layout;         ///< Layout verticale interno di scrollContent
    QScrollArea*   scrollArea;     ///< QScrollArea che avvolge scrollContent
    QWidget*       widget;         ///< Alias a scrollArea, ritornato da getWidget()

    /// Svuota il “layout” deallocando tutti i widget figli
    void clearLayout();
};

#endif // ARTISTWIDGET_H
