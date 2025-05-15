#ifndef PRODOTTOINSERTDIALOG_H
#define PRODOTTOINSERTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDateEdit>
#include <QTimeEdit>
#include <QScrollArea>
#include <QListWidget>
#include <unordered_map>
#include <vector>

#include "../../model/artisti/Artista.h"
#include "../../model/core/ArtistProduct.h"
#include "../../model/musica/Traccia.h"
#include "../../model/musica/Album.h"
#include "../../model/musica/Singolo.h"
#include "../../model/merch/TShirt.h"
#include "../../model/merch/CD.h"
#include "../../model/merch/Vinile.h"
#include "../../model/tour/Tour.h"
#include "../../model/util/Data.h"
#include "../../model/util/Orario.h"
#include "../../model/util/Durata.h"

class ProdottoInsertDialog : public QDialog {
    Q_OBJECT

public:
    ProdottoInsertDialog(std::unordered_map<unsigned int, Artista*>& artistiRef,
                         std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
                         QWidget* parent = nullptr);

private slots:
    void selezionaImmagine();
    void aggiornaCampiSpecifici();
    void confermaInserimento();

private:
    // === Riferimenti ===
    std::unordered_map<unsigned int, Artista*>& artisti;
    std::unordered_map<unsigned int, ArtistProduct*>& prodotti;
    Artista* artistaSelezionato;

    // === Layout ===
    QVBoxLayout* scrollLayout;
    QComboBox* artistaComboBox;
    QLineEdit* titoloEdit;
    QTextEdit* descrizioneEdit;
    QLineEdit* imagePathEdit;
    QComboBox* tipoComboBox;
    QWidget* campiSpecificiContainer;
    QVBoxLayout* campiSpecificiLayout;

    // === Campi comuni ===
    // (già definiti sopra)

    // === Campi NotMusica (comuni a TShirt, CD, Vinile, Tour) ===
    QDoubleSpinBox* prezzoSpin;
    QSpinBox* quantitaSpin;
    QCheckBox* disponibileCheck;
    QLineEdit* codiceProdottoEdit;

    // === TShirt ===
    QLineEdit* tagliaEdit;
    QLineEdit* coloreEdit;

    // === Disco (CD + Vinile) ===
    QLineEdit* produttoreStampeEdit;
    QLineEdit* codiceRiconoscimentoEdit;
    QLineEdit* tipoProdottoEdit;

    // === CD ===
    QLineEdit* formatoEdit;

    // === Vinile ===
    QSpinBox* rpmSpin;
    QSpinBox* diametroSpin;

    // === Tour ===
    QWidget* dateTourWidget;
    QVBoxLayout* dateTourLayout;
    QPushButton* aggiungiDataTourBtn;
    struct TourDataWidget {
        QDateEdit* dataEdit;
        QTimeEdit* orarioEdit;
        QLineEdit* luogoEdit;
    };
    std::vector<TourDataWidget> dateTourWidgets;

    // === Musica ===
    QDateEdit* dataUscitaEdit;
    QLineEdit* genereEdit;

    // === Album ===
    QLineEdit* labelEdit;
    QWidget* tracceWidget;
    QVBoxLayout* tracceLayout;
    QPushButton* aggiungiTracciaBtn;

    struct TracciaEditor {
        QLineEdit* nomeEdit;
        QSpinBox* oreDurata;
        QSpinBox* minutiDurata;
        QSpinBox* secondiDurata;
        QCheckBox* hasTestoCheck;
        QTextEdit* testoEdit;
        QWidget* partecipantiWidget;
        QVBoxLayout* partecipantiLayout;
        QPushButton* aggiungiPartecipanteBtn;
        std::vector<QLineEdit*> partecipantiLines;
    };
    std::vector<TracciaEditor> tracceEditors;

    // === Singolo ===
    QLineEdit* nomeTracciaEdit;
    QSpinBox* oreTraccia;
    QSpinBox* minutiTraccia;
    QSpinBox* secondiTraccia;
    QCheckBox* hasTestoTracciaCheck;
    QTextEdit* testoTracciaEdit;
    QWidget* partecipantiSingoloWidget;
    QVBoxLayout* partecipantiSingoloLayout;
    QPushButton* aggiungiPartecipanteSingoloBtn;
    std::vector<QLineEdit*> partecipantiSingoloLines;
    QCheckBox* remixCheck;
    QSpinBox* chartPosSpin;

    // === Funzioni di costruzione campi ===
    void setupUI();
    void buildArtistProduct();
    void buildMusica();
    void buildNotMusica();

    void buildMerch();
    void buildDisco();

    void buildTShirt();
    void buildCD();
    void buildVinile();
    void buildTour();
    void buildAlbum();
    void buildSingolo();
};

#endif // PRODOTTOINSERTDIALOG_H