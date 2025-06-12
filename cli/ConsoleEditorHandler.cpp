#include "ConsoleEditorHandler.h"
#include "SafeInput.h"
#include "../view/ErrorManager.h"
#include <iostream>
#include <limits>
#include <unordered_map>

#include "../model/artisti/Artista.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/merch/TShirt.h"
#include "../model/tour/Tour.h"
#include "../model/musica/Traccia.h"
#include "../model/tour/DataTour.h"

#include "../visitors/VisitorConsoleEditor.h"

// ----- ARTISTA -----
Artista* ConsoleEditorHandler::creaArtista(const std::string& nome) {
    std::string genere = SafeInput::read("Inserisci il genere musicale dell'artista: ");
    std::string info = SafeInput::read("Inserisci una breve descrizione/info: ");
    std::string imagePath = SafeInput::read("Inserisci il path dell'immagine: ");
    return new Artista(nome, genere, info, imagePath);
}

void ConsoleEditorHandler::editNomeArtista(Artista* a, const std::string& nuovoNome) {
    a -> setNome(nuovoNome);
}

void ConsoleEditorHandler::editGenereArtista(Artista* a) {
    std::string nuovo = SafeInput::read("Nuovo genere: ");
    a->setGenere(nuovo);
}

void ConsoleEditorHandler::editInfoArtista(Artista* a) {
    std::string nuova = SafeInput::read("Nuova descrizione/biografia: ");
    a->setInfo(nuova);
}

void ConsoleEditorHandler::editImagePathArtista(Artista* a) {
    std::string nuovo = SafeInput::read("Nuovo path immagine: ");
    a->setImagePath(nuovo);
}

void ConsoleEditorHandler::aggiungiProdottoArtista(Artista* artista) {
    std::cout << "\n--- Seleziona il tipo di prodotto da aggiungere ---\n"
              << "1. TShirt\n"
              << "2. CD\n"
              << "3. Vinile\n"
              << "4. Album\n"
              << "5. Singolo\n"
              << "6. Tour\n"
              << "0. Annulla\n";

    int scelta = SafeInput::read<int>("Scelta: ");
    if (scelta == 0) {
        return;
    }

    if (scelta < 1 || scelta > 6) {
        std::cout << "Scelta non valida.\n";
        return;
    }

    std::string title = SafeInput::read("Titolo: ");
    if (title.empty()) {
        std::cout << "Titolo non valido.\n";
        return;
    }

    for (const auto& [id, p] : artista->getProducts()) {
        if (p->getTitle() == title) {
            std::cout << "Titolo già in uso da un altro prodotto.\n";
            return;
        }
    }

    std::string description = SafeInput::read("Descrizione: ");

    if (scelta == 1) {
        double prezzo = SafeInput::read<double>("Prezzo: ");
        bool disponibile = SafeInput::readBool("Disponibile (1=sì, 0=no): ");
        unsigned int quantita = SafeInput::read<unsigned int>("Quantita: ");
        std::string codiceProdotto = SafeInput::read("Codice prodotto: ");
        std::string taglia = SafeInput::read("Taglia: ");
        std::string colore = SafeInput::read("Colore: ");

        artista->addProduct(new TShirt(
            artista,
            title,
            description,
            prezzo,
            disponibile,
            quantita,
            codiceProdotto,
            taglia,
            colore
        ));
    }
    else if (scelta == 2) {
        double prezzo = SafeInput::read<double>("Prezzo: ");
        bool disponibile = SafeInput::readBool("Disponibile (1=sì, 0=no): ");
        unsigned int quantita = SafeInput::read<unsigned int>("Quantita: ");
        std::string codiceProdotto = SafeInput::read("Codice prodotto: ");
        std::string produttoreStampe = SafeInput::read("Produttore stampe: ");
        std::string codiceRiconoscimento = SafeInput::read("Codice riconoscimento: ");
        std::string tipoProdotto = SafeInput::read("Tipo prodotto: ");
        std::string formato = SafeInput::read("Formato: ");

        artista->addProduct(new CD(
            artista,
            title,
            description,
            prezzo,
            disponibile,
            quantita,
            codiceProdotto,
            produttoreStampe,
            codiceRiconoscimento,
            tipoProdotto,
            formato
        ));
    }
    else if (scelta == 3) {
        double prezzo = SafeInput::read<double>("Prezzo: ");
        bool disponibile = SafeInput::readBool("Disponibile (1=sì, 0=no): ");
        unsigned int quantita = SafeInput::read<unsigned int>("Quantita: ");
        std::string codiceProdotto = SafeInput::read("Codice prodotto: ");
        std::string produttoreStampe = SafeInput::read("Produttore stampe: ");
        std::string codiceRiconoscimento = SafeInput::read("Codice riconoscimento: ");
        std::string tipoProdotto = SafeInput::read("Tipo prodotto: ");
        unsigned int rpm = SafeInput::read<unsigned int>("RPM: ");
        unsigned int diametro = SafeInput::read<unsigned int>("Diametro: ");

        artista->addProduct(new Vinile(
            artista,
            title,
            description,
            prezzo,
            disponibile,
            quantita,
            codiceProdotto,
            produttoreStampe,
            codiceRiconoscimento,
            tipoProdotto,
            rpm,
            diametro
        ));
    }
    else if (scelta == 4) {
        unsigned int giorno = SafeInput::read<unsigned int>("Data di uscita - giorno: ");
        unsigned int mese = SafeInput::read<unsigned int>("Data di uscita - mese: ");
        unsigned int anno = SafeInput::read<unsigned int>("Data di uscita - anno: ");
        Data dataUscita(giorno, mese, anno);

        std::string genere = SafeInput::read("Genere: ");
        std::string label = SafeInput::read("Label discografica: ");
        unsigned int numTracce = SafeInput::read<unsigned int>("Numero tracce: ");
        std::vector<Traccia> tracce;
        for (unsigned int i = 0; i < numTracce; ++i) {
            unsigned int ore = SafeInput::read<unsigned int>("Durata traccia - ore: ");
            unsigned int minuti = SafeInput::read<unsigned int>("Durata traccia - minuti: ");
            unsigned int sec = SafeInput::read<unsigned int>("Durata traccia - secondi: ");
            std::string nome = SafeInput::read("Nome traccia " + std::to_string(i+1) + ": ");
            bool hasTesto = SafeInput::readBool("Ha testo (1=sì, 0=no): ");
            std::string testo;
            if (hasTesto) {
                testo = SafeInput::read("Testo: ");
            }
            unsigned int numPartecipanti = SafeInput::read<unsigned int>("Partecipanti: ");
            std::vector<std::string> partecipanti;
            for (unsigned int j = 0; j < numPartecipanti; ++j) {
                partecipanti.push_back(SafeInput::read("Partecipante " + std::to_string(j+1) + ": "));
            }
            tracce.emplace_back(nome, partecipanti, Durata(ore, minuti, sec), testo, hasTesto);
        }
        Album* album = new Album(
            artista,
            title,
            description,
            dataUscita,
            Durata(0,0,0),
            genere,
            tracce,
            label
        );
        album->updateDurata();
        artista->addProduct(album);
    }
    else if (scelta == 5) {
        unsigned int giorno = SafeInput::read<unsigned int>("Data di uscita - giorno: ");
        unsigned int mese = SafeInput::read<unsigned int>("Data di uscita - mese: ");
        unsigned int anno = SafeInput::read<unsigned int>("Data di uscita - anno: ");
        Data dataUscita(giorno, mese, anno);

        std::string genere = SafeInput::read("Genere: ");
        unsigned int ore = SafeInput::read<unsigned int>("Durata principale - ore: ");
        unsigned int minuti = SafeInput::read<unsigned int>("Durata principale - minuti: ");
        unsigned int secondi = SafeInput::read<unsigned int>("Durata principale - secondi: ");
        std::string nome = SafeInput::read("Nome traccia principale: ");
        bool hasTesto = SafeInput::readBool("Ha testo (1=sì, 0=no): ");
        std::string testo;
        if (hasTesto) {
            testo = SafeInput::read("Testo: ");
        }
        unsigned int numP = SafeInput::read<unsigned int>("Partecipanti: ");
        std::vector<std::string> partecipanti;
        for (unsigned int j = 0; j < numP; ++j) {
            partecipanti.push_back(SafeInput::read("Partecipante " + std::to_string(j+1) + ": "));
        }
        Traccia mainTrack(nome, partecipanti, Durata(ore, minuti, secondi), testo, hasTesto);
        bool isRemix = SafeInput::readBool("E' un remix (1=sì, 0=no): ");
        int chartPosition = SafeInput::read<int>("Posizione in classifica: ");

        Singolo* singolo = new Singolo(
            artista,
            title,
            description,
            dataUscita,
            Durata(0,0,0),
            genere,
            mainTrack,
            isRemix,
            chartPosition
        );
        singolo->updateDurata();
        artista->addProduct(singolo);
    }
    else {
        unsigned int numDate = SafeInput::read<unsigned int>("Date tour: ");
        double prezzo = SafeInput::read<double>("Prezzo: ");
        bool disponibile = SafeInput::readBool("Disponibile (1=sì, 0=no): ");
        unsigned int quantita = SafeInput::read<unsigned int>("Quantita: ");

        Tour* tour = new Tour(
            artista,
            title,
            description,
            prezzo,
            disponibile,
            quantita
        );
        for (unsigned int i = 0; i < numDate; ++i) {
            unsigned int g = SafeInput::read<unsigned int>("Data - giorno: ");
            unsigned int m = SafeInput::read<unsigned int>("Data - mese: ");
            unsigned int a = SafeInput::read<unsigned int>("Data - anno: ");
            unsigned int h = SafeInput::read<unsigned int>("Orario - ore: ");
            unsigned int mi = SafeInput::read<unsigned int>("Orario - minuti: ");
            unsigned int s = SafeInput::read<unsigned int>("Orario - secondi: ");
            std::string luogo = SafeInput::read("Luogo: ");
            tour->addDataTour(DataTour(g, m, a, h, mi, s, luogo));
        }
        artista->addProduct(tour);
    }
}

// ----- ARTIST PRODUCT -----
void ConsoleEditorHandler::editDescrizioneProduct(ArtistProduct* p) {
    std::string descrizione = SafeInput::read("Nuova descrizione: ");
    p->setDescription(descrizione);
}

void ConsoleEditorHandler::editImagePathProduct(ArtistProduct* p) {
    std::string path = SafeInput::read("Nuovo path immagine: ");
    p->setImagePath(path);
}

// ----- MUSICA -----
void ConsoleEditorHandler::editGenereMusica(Musica* m) {
    std::string genere = SafeInput::read("Nuovo genere: ");
    m->setGenere(genere);
}

void ConsoleEditorHandler::editDataUscitaMusica(Musica* m) {
    unsigned int giorno = SafeInput::read<unsigned int>("Nuova data uscita - giorno: ");
    unsigned int mese   = SafeInput::read<unsigned int>("Nuova data uscita - mese: ");
    unsigned int anno   = SafeInput::read<unsigned int>("Nuova data uscita - anno: ");
    m->setDataUscita(Data(giorno, mese, anno));
}

// ----- MERCH -----
void ConsoleEditorHandler::editPrezzoMerch(Merch* m) {
    double prezzo = SafeInput::read<double>("Nuovo prezzo: ");
    m->setPrezzo(prezzo);
}

void ConsoleEditorHandler::editDisponibileMerch(Merch* m) {
    bool disponibile = SafeInput::readBool("Disponibile (1=sì, 0=no): ");
    m->setDisponibile(disponibile);
}

void ConsoleEditorHandler::editQuantitaMerch(Merch* m) {
    unsigned int quantita = SafeInput::read<unsigned int>("Nuova quantita: ");
    m->setQuantita(quantita);
}

// ----- ALBUM -----
void ConsoleEditorHandler::editEtichettaAlbum(Album* album) {
    std::string label = SafeInput::read("Nuova etichetta: ");
    album->setLabel(label);
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
        VisitorConsoleEditor visitor;
        t.accept(&visitor);
        album->updateDurata();
    }
}

// ----- SINGOLO -----
void ConsoleEditorHandler::editIsRemixSingolo(Singolo* s) {
    bool remix = SafeInput::readBool("E' un remix (1=sì, 0=no): ");
    s->setIsRemix(remix);
}

void ConsoleEditorHandler::editChartPositionSingolo(Singolo* s) {
    int pos = SafeInput::read<int>("Nuova posizione classifica: ");
    s->setChartPosition(pos);
}

void ConsoleEditorHandler::editMainTrackSingolo(Singolo* s) {
    VisitorConsoleEditor visitor;
    Traccia& t = s->getTracciaModificabile();
    t.accept(&visitor);
    s->setDurata(t.getDurata());
}

// ----- CD -----
void ConsoleEditorHandler::editFormatoCD(CD* cd) {
    std::string formato = SafeInput::read("Nuovo formato: ");
    cd->setFormato(formato);
}

// ----- VINILE -----
void ConsoleEditorHandler::editRPMVinile(Vinile* v) {
    int rpm = SafeInput::read<int>("Nuovi RPM: ");
    v->setRpm(rpm);
}

void ConsoleEditorHandler::editDiametroVinile(Vinile* v) {
    int diametro = SafeInput::read<int>("Nuovo diametro (pollici): ");
    v->setDiametro(diametro);
}

// ----- TSHIRT -----
void ConsoleEditorHandler::editTagliaTShirt(TShirt* tshirt) {
    std::string taglia = SafeInput::read("Nuova taglia: ");
    tshirt->setTaglia(taglia);
}

void ConsoleEditorHandler::editColoreTShirt(TShirt* tshirt) {
    std::string colore = SafeInput::read("Nuovo colore: ");
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
    std::string nuovo = SafeInput::read("Nuovo nome: ");
    t->setNome(nuovo);
}

void ConsoleEditorHandler::editDurataTraccia(Traccia* t) {
    int minuti = SafeInput::read<int>("Durata (minuti): ");
    int secondi = SafeInput::read<int>("Durata (secondi): ");
    t->setDurata(Durata(0, minuti, secondi));
}

void ConsoleEditorHandler::editTestoTraccia(Traccia* t) {
    std::string testo = SafeInput::read("Nuovo testo (vuoto se strumentale): ");
    t->setTesto(testo);
    t->setHasTesto(!testo.empty());
}

void ConsoleEditorHandler::aggiungiPartecipanteTraccia(Traccia* t) {
    std::string p = SafeInput::read("Nome partecipante: ");
    t->addPartecipante(p);
}

void ConsoleEditorHandler::rimuoviPartecipanteTraccia(Traccia* t, unsigned int index) {
    t->removePartecipante(index);
}

void ConsoleEditorHandler::editPartecipanteTraccia(Traccia* t, unsigned int index) {
    std::string nuovo = SafeInput::read("Nuovo nome partecipante: ");
    t->editPartecipante(index, nuovo);
}

void ConsoleEditorHandler::gestisciPartecipantiTraccia(Traccia* t) {
    int scelta = -1;
    while (scelta != 0) {
        std::cout << "\n--- Gestione Partecipanti ---\n";
        const auto& partecipanti = t->getPartecipanti();
        if (partecipanti.empty()) {
            std::cout << "Nessun partecipante.\n";
        }
        else {
            for (size_t i = 0; i < partecipanti.size(); ++i) {
                std::cout << i + 1 << ") " << partecipanti[i] << "\n";
            }
        }
        std::cout << "\nOpzioni:\n"
                  << "1. Aggiungi\n"
                  << "2. Modifica\n"
                  << "3. Rimuovi\n"
                  << "0. Torna indietro\n";
        scelta = SafeInput::read<int>("Scelta: ");

        switch (scelta) {
            case 1:
                aggiungiPartecipanteTraccia(t);
                break;
            case 2: {
                unsigned int idx = SafeInput::read<unsigned int>("Indice da modificare: ");
                if (idx >= 1 && idx <= partecipanti.size()) {
                    editPartecipanteTraccia(t, idx - 1);
                }
                else {
                    std::cout << "Indice non valido.\n";
                }
                break;
            }
            case 3: {
                unsigned int idx = SafeInput::read<unsigned int>("Indice da rimuovere: ");
                if (idx >= 1 && idx <= partecipanti.size()) {
                    rimuoviPartecipanteTraccia(t, idx - 1);
                }
                else {
                    std::cout << "Indice non valido.\n";
                }
                break;
            }
            case 0:
                std::cout << "Uscita gestione partecipanti.\n";
                break;
            default:
                std::cout << "Scelta non valida.\n";
                break;
        }
    }
}

// ----- DATATOUR -----
void ConsoleEditorHandler::editDataDataTour(DataTour* d) {
    unsigned int g = SafeInput::read<unsigned int>("Nuova data - giorno: ");
    unsigned int m = SafeInput::read<unsigned int>("Nuova data - mese: ");
    unsigned int a = SafeInput::read<unsigned int>("Nuova data - anno: ");
    d->setData(Data(g, m, a));
}

void ConsoleEditorHandler::editOrarioDataTour(DataTour* d) {
    unsigned int h = SafeInput::read<unsigned int>("Nuovo orario - ore: ");
    unsigned int mi= SafeInput::read<unsigned int>("Minuti: ");
    unsigned int s = SafeInput::read<unsigned int>("Secondi: ");
    d->setOrario(Orario(h, mi, s));
}

void ConsoleEditorHandler::editLuogoDataTour(DataTour* d) {
    std::string luogo = SafeInput::read("Nuovo luogo: ");
    if(luogo.empty()) {
        std::cout << "Luogo non valido, non può essere vuoto.\n";
        return;
    }
    else
        d->setLuogo(luogo);
}


// ----- CREAZIONE OGGETTI -----

Traccia ConsoleEditorHandler::creaTraccia() {
    unsigned int minuti = SafeInput::read<unsigned int>("Durata - minuti: ");
    unsigned int secondi = SafeInput::read<unsigned int>("Durata - secondi: ");
    std::string nome;
    do{
        nome = SafeInput::read("Nome traccia: ");
        if(nome.empty())
            std::cout << "Il nome della traccia non può essere vuoto. Riprova.\n";
    }while(nome.empty());
    
    std::string testo = SafeInput::read("Testo (vuoto se strumentale): ");
    bool hasTesto = !testo.empty();
    std::vector<std::string> partecipanti;
    while (true) {
        std::string p = SafeInput::read("Aggiungi partecipante (vuoto fine): ");
        if (p.empty()) break;
        partecipanti.push_back(p);
    }
    return Traccia(nome, partecipanti, Durata(0, minuti, secondi), testo, hasTesto);
}

DataTour ConsoleEditorHandler::creaDataTour() {
    unsigned int g = SafeInput::read<unsigned int>("Data - giorno: ");
    unsigned int m = SafeInput::read<unsigned int>("Data - mese: ");
    unsigned int a = SafeInput::read<unsigned int>("Data - anno: ");
    unsigned int h = SafeInput::read<unsigned int>("Orario - ore: ");
    unsigned int mi = SafeInput::read<unsigned int>("Orario - minuti: ");
    unsigned int s = SafeInput::read<unsigned int>("Orario - secondi: ");
    std::string luogo;
    do{
        luogo = SafeInput::read("Luogo: ");
        if(luogo.empty())
            std::cout << "Il luogo non può essere vuoto. Riprova.\n";
    }while(luogo.empty());
    return DataTour(g, m, a, h, mi, s, luogo);
}