#include "ConsoleEditorHandler.h"
#include "../view/ErrorManager.h"
#include <iostream>
#include <limits>

#include "../model/artisti/Artista.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/merch/TShirt.h"
#include "../model/tour/Tour.h"
#include "../model/musica/Traccia.h"
#include "../model/tour/DataTour.h"

#include "VisitorConsoleEditor.h"


// ----- ARTISTA -----

Artista* ConsoleEditorHandler::creaArtista() {
    std::string nome, info, imagePath, genere;

    std::cout << "Inserisci il nome dell'artista: ";
    std::getline(std::cin, nome);

    std::cout << "Inserisci il genere musicale dell'artista: ";
    std::getline(std::cin, genere);

    std::cout << "Inserisci una breve descrizione/info: ";
    std::getline(std::cin, info);

    std::cout << "Inserisci il path dell'immagine: ";
    std::getline(std::cin, imagePath);

    return new Artista(nome, genere, info, imagePath);
}

void ConsoleEditorHandler::editNomeArtista(Artista* a) {
    std::cout << "Nuovo nome: ";
    std::string nuovo;
    std::getline(std::cin, nuovo);
    if (!nuovo.empty())
        a->setNome(nuovo);
}

void ConsoleEditorHandler::editGenereArtista(Artista* a) {
    std::cout << "Nuovo genere: ";
    std::string nuovo;
    std::getline(std::cin, nuovo);
    a->setGenere(nuovo);
}

void ConsoleEditorHandler::editInfoArtista(Artista* a) {
    std::cout << "Nuova descrizione/biografia: ";
    std::string nuova;
    std::getline(std::cin, nuova);
    a->setInfo(nuova);
}

void ConsoleEditorHandler::editImagePathArtista(Artista* a) {
    std::cout << "Nuovo path immagine: ";
    std::string nuovo;
    std::getline(std::cin, nuovo);
    a->setImagePath(nuovo);
}

void ConsoleEditorHandler::aggiungiProdottoArtista(Artista* artista) {
    std::cout << "Seleziona il tipo di prodotto da aggiungere:\n";
    std::cout << "1. TShirt\n";
    std::cout << "2. CD\n";
    std::cout << "3. Vinile\n";
    std::cout << "4. Album\n";
    std::cout << "5. Singolo\n";
    std::cout << "6. Tour\n";
    int scelta;
    std::cin >> scelta;

    std::string title, description;
    std::cout << "Titolo: ";
    std::getline(std::cin >> std::ws, title);
    std::cout << "Descrizione: ";
    std::getline(std::cin >> std::ws, description);

    if (scelta == 1) { // TShirt
        double prezzo;
        bool disponibile;
        unsigned int quantita;
        std::string codiceProdotto, taglia, colore;

        std::cout << "Prezzo: ";
        std::cin >> prezzo;
        std::cout << "Disponibile (1 = si, 0 = no): ";
        std::cin >> disponibile;
        std::cout << "Quantita: ";
        std::cin >> quantita;

        std::cout << "Codice prodotto: ";
        std::getline(std::cin >> std::ws, codiceProdotto);
        std::cout << "Taglia: ";
        std::getline(std::cin >> std::ws, taglia);
        std::cout << "Colore: ";
        std::getline(std::cin >> std::ws, colore);

        artista->addProduct(new TShirt(artista, title, description, prezzo, disponibile, quantita, codiceProdotto, taglia, colore));
    }
    else if (scelta == 2) { // CD
        double prezzo;
        bool disponibile;
        unsigned int quantita;
        std::string codiceProdotto, produttoreStampe, codiceRiconoscimento, tipoProdotto, formato;

        std::cout << "Prezzo: ";
        std::cin >> prezzo;
        std::cout << "Disponibile (1 = si, 0 = no): ";
        std::cin >> disponibile;
        std::cout << "Quantita: ";
        std::cin >> quantita;

        std::cout << "Codice prodotto: ";
        std::getline(std::cin >> std::ws, codiceProdotto);
        std::cout << "Produttore stampe: ";
        std::getline(std::cin >> std::ws, produttoreStampe);
        std::cout << "Codice riconoscimento: ";
        std::getline(std::cin >> std::ws, codiceRiconoscimento);
        std::cout << "Tipo prodotto: ";
        std::getline(std::cin >> std::ws, tipoProdotto);
        std::cout << "Formato: ";
        std::getline(std::cin >> std::ws, formato);

        artista->addProduct(new CD(artista, title, description, prezzo, disponibile, quantita, codiceProdotto, produttoreStampe, codiceRiconoscimento, tipoProdotto, formato));
    }
    else if (scelta == 3) { // Vinile
        double prezzo;
        bool disponibile;
        unsigned int quantita;
        std::string codiceProdotto, produttoreStampe, codiceRiconoscimento, tipoProdotto;
        unsigned int rpm, diametro;

        std::cout << "Prezzo: ";
        std::cin >> prezzo;
        std::cout << "Disponibile (1 = si, 0 = no): ";
        std::cin >> disponibile;
        std::cout << "Quantita: ";
        std::cin >> quantita;

        std::cout << "Codice prodotto: ";
        std::getline(std::cin >> std::ws, codiceProdotto);
        std::cout << "Produttore stampe: ";
        std::getline(std::cin >> std::ws, produttoreStampe);
        std::cout << "Codice riconoscimento: ";
        std::getline(std::cin >> std::ws, codiceRiconoscimento);
        std::cout << "Tipo prodotto: ";
        std::getline(std::cin >> std::ws, tipoProdotto);

        std::cout << "RPM: ";
        std::cin >> rpm;
        std::cout << "Diametro: ";
        std::cin >> diametro;

        artista->addProduct(new Vinile(artista, title, description, prezzo, disponibile, quantita, codiceProdotto, produttoreStampe, codiceRiconoscimento, tipoProdotto, rpm, diametro));
    }
    else if (scelta == 4) { // Album
        unsigned int giorno, mese, anno;
        std::string genere, label;

        std::cout << "Data di uscita (giorno mese anno): ";
        std::cin >> giorno >> mese >> anno;
        Data dataUscita(giorno, mese, anno);

        std::cout << "Genere: ";
        std::getline(std::cin >> std::ws, genere);
        std::cout << "Label discografica: ";
        std::getline(std::cin >> std::ws, label);

        std::vector<Traccia> tracce;
        unsigned int numTracce;
        std::cout << "Numero tracce: ";
        std::cin >> numTracce;

        for (unsigned int i = 0; i < numTracce; ++i) {
            std::string nome, testo;
            bool hasTesto;
            unsigned int ore, minuti, secondi;

            std::cout << "Nome traccia " << i+1 << ": ";
            std::getline(std::cin >> std::ws, nome);

            std::cout << "Durata (ore minuti secondi): ";
            std::cin >> ore >> minuti >> secondi;

            std::cout << "Ha testo? (1 = si, 0 = no): ";
            std::cin >> hasTesto;

            if (hasTesto) {
                std::cout << "Testo: ";
                std::getline(std::cin >> std::ws, testo);
            } else {
                testo = "";
            }

            std::vector<std::string> partecipanti;
            unsigned int numPartecipanti;
            std::cout << "Numero partecipanti: ";
            std::cin >> numPartecipanti;

            for (unsigned int j = 0; j < numPartecipanti; ++j) {
                std::string partecipante;
                std::cout << "Nome partecipante " << j+1 << ": ";
                std::getline(std::cin >> std::ws, partecipante);
                partecipanti.push_back(partecipante);
            }

            tracce.emplace_back(nome, partecipanti, Durata(ore, minuti, secondi), testo, hasTesto);
        }
        Album* album = new Album(artista, title, description, dataUscita, Durata(0,0,0), genere, tracce, label);
        album->updateDurata();
        artista->addProduct(album);
    }
    else if (scelta == 5) { // Singolo
        unsigned int giorno, mese, anno;
        std::string genere;

        std::cout << "Data di uscita (giorno mese anno): ";
        std::cin >> giorno >> mese >> anno;
        Data dataUscita(giorno, mese, anno);

        std::cout << "Genere: ";
        std::getline(std::cin >> std::ws, genere);

        std::string nome, testo;
        bool hasTesto;
        unsigned int ore, minuti, secondi;

        std::cout << "Nome traccia principale: ";
        std::getline(std::cin >> std::ws, nome);

        std::cout << "Durata (ore minuti secondi): ";
        std::cin >> ore >> minuti >> secondi;

        std::cout << "Ha testo? (1 = si, 0 = no): ";
        std::cin >> hasTesto;

        if (hasTesto) {
            std::cout << "Testo: ";
            std::getline(std::cin >> std::ws, testo);
        } else {
            testo = "";
        }

        std::vector<std::string> partecipanti;
        unsigned int numPartecipanti;
        std::cout << "Numero partecipanti: ";
        std::cin >> numPartecipanti;

        for (unsigned int j = 0; j < numPartecipanti; ++j) {
            std::string partecipante;
            std::cout << "Nome partecipante " << j+1 << ": ";
            std::getline(std::cin >> std::ws, partecipante);
            partecipanti.push_back(partecipante);
        }

        Traccia mainTrack(nome, partecipanti, Durata(ore, minuti, secondi), testo, hasTesto);

        bool isRemix;
        int chartPosition;

        std::cout << "E' un remix? (1 = si, 0 = no): ";
        std::cin >> isRemix;

        std::cout << "Posizione in classifica: ";
        std::cin >> chartPosition;

        Singolo* singolo = new Singolo(artista, title, description, dataUscita, Durata(0,0,0), genere, mainTrack, isRemix, chartPosition);
        singolo -> updateDurata();
        artista->addProduct(singolo);
    }
    else if (scelta == 6) { // Tour
        unsigned int numDate;
        std::vector<DataTour> dateTour;
        double prezzo;
        bool disponibile;
        unsigned int quantita;

        std::cout << "Prezzo: ";
        std::cin >> prezzo;
        std::cout << "Disponibile (1 = si, 0 = no): ";
        std::cin >> disponibile;
        std::cout << "Quantita: ";
        std::cin >> quantita;

        std::cout << "Numero date tour: ";
        std::cin >> numDate;

        for (unsigned int i = 0; i < numDate; ++i) {
            unsigned int giorno, mese, anno, ore, minuti, secondi;
            std::string luogo;

            std::cout << "Data (giorno mese anno): ";
            std::cin >> giorno >> mese >> anno;

            std::cout << "Orario (ore minuti secondi): ";
            std::cin >> ore >> minuti >> secondi;

            std::cout << "Luogo: ";
            std::getline(std::cin >> std::ws, luogo);

            dateTour.emplace_back(giorno, mese, anno, ore, minuti, secondi, luogo);
        }
        Tour* tour = new Tour(artista, title, description, prezzo, disponibile, quantita);
        for (const auto& date : dateTour) {
            tour->addDataTour(date);
        }
        artista->addProduct(tour);

    } 
    else {
        std::cout << "Scelta non valida.\n";
    }
}

// ----- ARTIST PRODUCT -----

void ConsoleEditorHandler::editTitoloProduct(ArtistProduct* p) {
    std::cout << "Nuovo titolo: ";
    std::string nuovo;
    std::getline(std::cin, nuovo);
    if (!nuovo.empty())
        p->setTitle(nuovo);
}

void ConsoleEditorHandler::editDescrizioneProduct(ArtistProduct* p) {
    std::cout << "Nuova descrizione: ";
    std::string nuova;
    std::getline(std::cin, nuova);
    p->setDescription(nuova);
}

void ConsoleEditorHandler::editImagePathProduct(ArtistProduct* p) {
    std::cout << "Nuovo path immagine: ";
    std::string nuovo;
    std::getline(std::cin, nuovo);
    p->setImagePath(nuovo);
}

// ----- MUSICA -----

void ConsoleEditorHandler::editGenereMusica(Musica* m) {
    std::cout << "Nuovo genere: ";
    std::string nuovo;
    std::getline(std::cin, nuovo);
    m->setGenere(nuovo);
}

void ConsoleEditorHandler::editDataUscitaMusica(Musica* m) {
    int g, mth, a;
    std::cout << "Nuova data uscita (gg mm aaaa): ";
    std::cin >> g >> mth >> a;
    std::cin.ignore();
    m->setDataUscita(Data(g, mth, a));
}

// ----- MERCH -----

void ConsoleEditorHandler::editPrezzoMerch(Merch* m) {
    double prezzo;
    std::cout << "Nuovo prezzo: ";
    std::cin >> prezzo;
    std::cin.ignore();
    m->setPrezzo(prezzo);
}

void ConsoleEditorHandler::editDisponibileMerch(Merch* m) {
    bool disp;
    std::cout << "Disponibile? (1=sì, 0=no): ";
    std::cin >> disp;
    std::cin.ignore();
    m->setDisponibile(disp);
}

void ConsoleEditorHandler::editQuantitaMerch(Merch* m) {
    unsigned int q;
    std::cout << "Nuova quantità: ";
    std::cin >> q;
    std::cin.ignore();
    m->setQuantita(q);
}

// ----- ALBUM -----

void ConsoleEditorHandler::editEtichettaAlbum(Album* album) {
    std::cout << "Nuova etichetta: ";
    std::string et;
    std::getline(std::cin, et);
    album->setLabel(et);
}

void ConsoleEditorHandler::aggiungiTracciaAlbum(Album* album) {
    Traccia t = creaTraccia();
    album->addTraccia(t);
    album->updateDurata();
}

void ConsoleEditorHandler::rimuoviTracciaAlbum(Album* album, unsigned int index) {
    const auto& tracce = album->getTracce();
    if (index < tracce.size()) {
        album->removeTraccia(tracce[index].getNome());
        album->updateDurata();
    }
}

void ConsoleEditorHandler::editTracciaAlbum(Album* album, unsigned int index) {
    const auto& tracce = album->getTracce();
    if (index < tracce.size()) {
        Traccia& t = album->getTracciaModificabile(index);
        VisitorConsoleEditor v;
        t.accept(&v);
        album->updateDurata();
    }
}

// ----- SINGOLO -----

void ConsoleEditorHandler::editIsRemixSingolo(Singolo* s) {
    bool remix;
    std::cout << "È un remix? (1=sì, 0=no): ";
    std::cin >> remix;
    std::cin.ignore();
    s->setIsRemix(remix);
}

void ConsoleEditorHandler::editChartPositionSingolo(Singolo* s) {
    int pos;
    std::cout << "Nuova posizione classifica: ";
    std::cin >> pos;
    std::cin.ignore();
    s->setChartPosition(pos);
}

void ConsoleEditorHandler::editMainTrackSingolo(Singolo* s) {
    VisitorConsoleEditor v;
    Traccia& t = s->getTracciaModificabile();
    t.accept(&v);
    s->setDurata(t.getDurata());
}

// ----- CD -----

void ConsoleEditorHandler::editFormatoCD(CD* cd) {
    std::cout << "Nuovo formato: ";
    std::string f;
    std::getline(std::cin, f);
    cd->setFormato(f);
}

// ----- VINILE -----

 void ConsoleEditorHandler::editRPMVinile(Vinile* v) {
    int rpm;
    std::cout << "Nuovi RPM: ";
    std::cin >> rpm;
    std::cin.ignore();
    v->setRpm(rpm);
}

void ConsoleEditorHandler::editDiametroVinile(Vinile* v) {
    int d;
    std::cout << "Nuovo diametro (pollici): ";
    std::cin >> d;
    std::cin.ignore();
    v->setDiametro(d);
}

// ----- TSHIRT -----

void ConsoleEditorHandler::editTagliaTShirt(TShirt* tshirt) {
    std::cout << "Nuova taglia: ";
    std::string taglia;
    std::getline(std::cin, taglia);
    tshirt->setTaglia(taglia);
}

void ConsoleEditorHandler::editColoreTShirt(TShirt* tshirt) {
    std::cout << "Nuovo colore: ";
    std::string colore;
    std::getline(std::cin, colore);
    tshirt->setColore(colore);
}

// ----- TOUR -----

void ConsoleEditorHandler::aggiungiDataTour(Tour* tour) {
    DataTour dt = creaDataTour();
    tour->addDataTour(dt);
}

void ConsoleEditorHandler::rimuoviDataTour(Tour* tour, unsigned int index) {
    tour->removeDataTour(index);
}

void ConsoleEditorHandler::editDataTour(Tour* tour, unsigned int index) {
    auto& date = tour->getDateTourModificabile();
    if (index < date.size()) {
        VisitorConsoleEditor v;
        date[index].accept(&v);
    }
}

// ----- TRACCIA -----

void ConsoleEditorHandler::editNomeTraccia(Traccia* t) {
    std::cout << "Nuovo nome: ";
    std::string nuovo;
    std::getline(std::cin, nuovo);
    t->setNome(nuovo);
}

void ConsoleEditorHandler::editDurataTraccia(Traccia* t) {
    int min, sec;
    std::cout << "Durata (minuti secondi): ";
    std::cin >> min >> sec;
    std::cin.ignore();
    t->setDurata(Durata(0, min, sec));
}

void ConsoleEditorHandler::editTestoTraccia(Traccia* t) {
    std::cout << "Nuovo testo (vuoto se strumentale): ";
    std::string testo;
    std::getline(std::cin, testo);
    t->setTesto(testo);
    t->setHasTesto(!testo.empty());
}

void ConsoleEditorHandler::aggiungiPartecipanteTraccia(Traccia* t) {
    std::string p;
    std::cout << "Nome partecipante: ";
    std::getline(std::cin, p);
    t->addPartecipante(p);
}

void ConsoleEditorHandler::rimuoviPartecipanteTraccia(Traccia* t, unsigned int index) {
    t->removePartecipante(index);
}

void ConsoleEditorHandler::editPartecipanteTraccia(Traccia* t, unsigned int index) {
    std::string nuovo;
    std::cout << "Nuovo nome partecipante: ";
    std::getline(std::cin, nuovo);
    t->editPartecipante(index, nuovo);
}

void ConsoleEditorHandler::gestisciPartecipantiTraccia(Traccia* t) {
    // Implementazione da fare o almeno vuota:
}

// ----- DATATOUR -----

void ConsoleEditorHandler::editDataDataTour(DataTour* d) {
    int g, m, a;
    std::cout << "Nuova data (gg mm aaaa): ";
    std::cin >> g >> m >> a;
    std::cin.ignore();
    d->setData(Data(g, m, a));
}

void ConsoleEditorHandler::editOrarioDataTour(DataTour* d) {
    int h, min, sec;
    std::cout << "Nuovo orario (hh mm ss): ";
    std::cin >> h >> min >> sec;
    std::cin.ignore();
    d->setOrario(Orario(h, min, sec));
}

void ConsoleEditorHandler::editLuogoDataTour(DataTour* d) {
    std::cout << "Nuovo luogo: ";
    std::string l;
    std::getline(std::cin, l);
    d->setLuogo(l);
}


// ----- CREAZIONE OGGETTI -----

Traccia ConsoleEditorHandler::creaTraccia() {
    std::string nome, testo;
    int minuti, secondi;

    std::cout << "Nome traccia: ";
    std::getline(std::cin, nome);

    std::cout << "Durata (minuti secondi): ";
    std::cin >> minuti >> secondi;
    std::cin.ignore();

    std::cout << "Testo (vuoto se strumentale): ";
    std::getline(std::cin, testo);

    bool hasTesto = !testo.empty();

    std::vector<std::string> partecipanti;
    bool aggiungi = true;
    while (aggiungi) {
        std::string partecipante;
        std::cout << "Aggiungi partecipante (vuoto per terminare): ";
        std::getline(std::cin, partecipante);
        if (partecipante.empty())
            aggiungi = false;
        else
            partecipanti.push_back(partecipante);
    }

    return Traccia(nome, partecipanti, Durata(0, minuti, secondi), testo, hasTesto);
}

DataTour ConsoleEditorHandler::creaDataTour() {
    int g, m, a, ore, min, sec;
    std::string luogo;

    std::cout << "Data (gg mm aaaa): ";
    std::cin >> g >> m >> a;
    std::cin.ignore();

    std::cout << "Orario (hh mm ss): ";
    std::cin >> ore >> min >> sec;
    std::cin.ignore();

    std::cout << "Luogo: ";
    std::getline(std::cin, luogo);

    return DataTour(g, m, a, ore, min, sec, luogo);
}