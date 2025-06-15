#ifndef PRODOTTOFORMBUILDER_H
#define PRODOTTOFORMBUILDER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDateEdit>
#include <QTimeEdit>
#include <QScrollArea>
#include <QListWidget>
#include <unordered_map>
#include <vector>

#include "../../visitors/VisitorInterfaceNotConst.h"
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
#include "../../model/tour/DataTour.h"
#include "../../model/musica/Traccia.h"


class ProdottoFormBuilder : public QWidget, public VisitorInterfaceNotConst {
    Q_OBJECT

public:
    ProdottoFormBuilder(
        std::unordered_map<unsigned int, Artista*>& artistiRef,
        std::unordered_map<unsigned int, ArtistProduct*>& prodottiRef,
        const ArtistProduct* prodottoEsistente,
        QWidget* parent = nullptr
    );

signals:
    void prodottoSalvato();

private slots:
    void onConfermaArtista();
    void onTipoChanged(const QString&);
    void onSelezionaImmagine();
    void onConfermaSalvataggio();

private:
    // Riferimenti
    std::unordered_map<unsigned int, Artista*>& artisti;
    std::unordered_map<unsigned int, ArtistProduct*>& prodotti;
    const ArtistProduct* prodottoCorrente;
    Artista* artistaSelezionato;

    // Layout principale
    QVBoxLayout* mainLayout;

    // Per inserimento
    QWidget* selezioneContainer;
    QComboBox* artistaComboBox;
    QPushButton* confermaArtistaBtn;

    // Area scrollabile
    QScrollArea* scrollArea;
    QWidget* scrollContent;
    QVBoxLayout* scrollLayout;

    // Campi comuni
    QLineEdit* titoloEdit;
    QTextEdit* descrizioneEdit;
    QLineEdit* imagePathEdit;

    // Dati vendita
    QDoubleSpinBox* prezzoSpin;
    QCheckBox* disponibileCheck;
    QSpinBox* quantitaSpin;
    QLineEdit* codiceProdottoEdit;

    // Contenitore dinamico
    QWidget* campiSpecificiContainer;
    QVBoxLayout* campiSpecificiLayout;

    // Combo tipo (solo inserimento)
    QGroupBox* tipoBox;
    QComboBox* tipoComboBox;

    // Campi TShirt
    QLineEdit* tagliaEdit;
    QLineEdit* coloreEdit;

    // Campi Disco (CD + Vinile)
    QLineEdit* produttoreStampeEdit;
    QLineEdit* codiceRiconoscimentoEdit;
    QLineEdit* tipoProdottoEdit;

    // Campo CD
    QLineEdit* formatoEdit;

    // Campi Vinile
    QSpinBox* rpmSpin;
    QSpinBox* diametroSpin;

    // Campi Tour
    QWidget* dateTourWidget;
    QVBoxLayout* dateTourLayout;
    QPushButton* aggiungiDataTourBtn;
    struct TourDataWidget {
        QDateEdit* dataEdit;
        QTimeEdit* orarioEdit;
        QLineEdit* luogoEdit;
    };
    std::vector<TourDataWidget> dateTourWidgets;

    // Campi Musica (Singolo + Album)
    QDateEdit* dataUscitaEdit;
    QLineEdit* genereEdit;

    // Campi Singolo
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

    // Campi Album
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
    std::vector<TracciaEditor*> tracceEditors;

    // Bottoni
    QPushButton* annullaBtn;
    QPushButton* salvaBtn;

    // Metodi di setup
    void setupInserimento();
    void setupModifica();
    void setupScrollArea();
    void buildCampiBase();
    void buildCampoVendita();
    void buildCampoMerch();
    void buildCampoDisco();
    void buildCampoTShirt();
    void buildCampoCD();
    void buildCampoVinile();
    void buildCampoTour();
    void buildCampoMusica();
    void buildCampoSingolo();
    void buildCampoAlbum();

    void popolaCampiDaProdotto();

    // Applicazione modifiche (confronto e setter)
    void applicaModificheTShirt(TShirt*);
    void applicaModificheCD(CD*);
    void applicaModificheVinile(Vinile*);
    void applicaModificheTour(Tour*);
    void applicaModificheSingolo(Singolo*);
    void applicaModificheAlbum(Album*);

    // VisitorInterfaceNotConst overrides
    void visit(TShirt*   ) override;
    void visit(CD*       ) override;
    void visit(Vinile*   ) override;
    void visit(Tour*     ) override;
    void visit(Singolo*  ) override;
    void visit(Album*    ) override;
    void visit(DataTour* ) override;
    void visit(Traccia*  ) override;

};

#endif // PRODOTTOFORMBUILDER_H
