#ifndef PRODOTTOEDITORDIALOG_H
#define PRODOTTOEDITORDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QTimeEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QGroupBox>
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
#include "../../data/DataManager.h"

class ProdottoEditorDialog : public QDialog {
    Q_OBJECT

public:
    ProdottoEditorDialog(ArtistProduct* prodottoOriginale,
                       std::unordered_map<unsigned int, Artista*>& artistiRef,
                       std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
                       QWidget* parent = nullptr);

private:
    ArtistProduct* prodotto;
    Artista* artistaSelezionato;
    std::unordered_map<unsigned int, Artista*>& artisti;
    std::unordered_map<unsigned int, ArtistProduct*>& prodotti;

    // UI
    QVBoxLayout* scrollLayout;
    QWidget* campiSpecificiContainer;
    QVBoxLayout* campiSpecificiLayout;

    // Campi comuni
    QLineEdit* titoloEdit;
    QTextEdit* descrizioneEdit;
    QLineEdit* imagePathEdit;
    QDateEdit* dataUscitaEdit;
    QLineEdit* genereEdit;

    // Campi NotMusica/Merch/Disco
    QDoubleSpinBox* prezzoSpin;
    QCheckBox* disponibileCheck;
    QSpinBox* quantitaSpin;
    QLineEdit* codiceProdottoEdit;
    QLineEdit* produttoreStampeEdit;
    QLineEdit* codiceRiconoscimentoEdit;
    QLineEdit* tipoProdottoEdit;
    QLineEdit* formatoEdit;
    QSpinBox* rpmSpin;
    QSpinBox* diametroSpin;
    QLineEdit* tagliaEdit;
    QLineEdit* coloreEdit;

    // Tour
    struct TourDataWidget {
        QDateEdit* dataEdit;
        QTimeEdit* orarioEdit;
        QLineEdit* luogoEdit;
    };
    QWidget* dateTourWidget;
    QVBoxLayout* dateTourLayout;
    QPushButton* aggiungiDataTourBtn;
    std::vector<TourDataWidget> dateTourWidgets;

    // Singolo
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

    // Album
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
    QLineEdit* labelEdit;
    QWidget* tracceWidget;
    QVBoxLayout* tracceLayout;
    QPushButton* aggiungiTracciaBtn;
    std::vector<TracciaEditor*> tracceEditors;

    void setupUI();
    void selezionaImmagine();
    void buildArtistProduct();
    void buildMusica();
    void buildNotMusica();
    void buildMerch();
    void buildDisco();
    void buildCD();
    void buildVinile();
    void buildTShirt();
    void buildTour();
    void buildSingolo();
    void buildAlbum();
    void confermaModifica();
};

#endif