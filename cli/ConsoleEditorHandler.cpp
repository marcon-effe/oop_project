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
    std::string info, imagePath, genere;

    std::cout << "Inserisci il genere musicale dell'artista: ";
    std::getline(std::cin, genere);

    std::cout << "Inserisci una breve descrizione/info: ";
    std::getline(std::cin, info);

    std::cout << "Inserisci il path dell'immagine: ";
    std::getline(std::cin, imagePath);

    return new Artista(nome, genere, info, imagePath);
}

void ConsoleEditorHandler::editNomeArtista(Artista* a, const std::string& nuovoNome) {
    a -> setNome(nuovoNome);
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
    std::cout << "0. Annulla\n";

    int scelta = SafeInput::read<int>("Scelta: ");
    if (scelta < 1 || scelta > 6) { std::cout << "Scelta non valida." << std::endl; return; }
    if(scelta == 0) return;

    std::string title, description;
    std::cout << "Titolo: ";
    std::getline(std::cin >> std::ws, title);

    if(title.empty()) {
        std::cout << "Titolo non valido." << std::endl;
        return;
    }

    //controlliamo nella mappa che il titolo non sia già in uso
    for (const auto& [id, p] : artista->getProducts()) {
        if (p->getTitle() == title) {
            std::cout << "Titolo già in uso da un altro prodotto." << std::endl;
            return;
        }
    }

    std::cout << "Descrizione: ";
    std::getline(std::cin >> std::ws, description);

    if (scelta == 1) {
        double prezzo = SafeInput::read<double>("Prezzo: ");
        bool disponibile = SafeInput::readBool("Disponibile");
        unsigned int quantita = SafeInput::read<unsigned int>("Quantita: ");

        std::string codiceProdotto, taglia, colore;
        std::cout << "Codice prodotto: "; std::getline(std::cin >> std::ws, codiceProdotto);
        std::cout << "Taglia: "; std::getline(std::cin >> std::ws, taglia);
        std::cout << "Colore: "; std::getline(std::cin >> std::ws, colore);

        artista->addProduct(new TShirt(artista, title, description, prezzo, disponibile, quantita, codiceProdotto, taglia, colore));
    
    } else if (scelta == 2) {
        double prezzo = SafeInput::read<double>("Prezzo: ");
        bool disponibile = SafeInput::readBool("Disponibile");
        unsigned int quantita = SafeInput::read<unsigned int>("Quantita: ");

        std::string codiceProdotto, produttoreStampe, codiceRiconoscimento, tipoProdotto, formato;
        std::cout << "Codice prodotto: "; std::getline(std::cin >> std::ws, codiceProdotto);
        std::cout << "Produttore stampe: "; std::getline(std::cin >> std::ws, produttoreStampe);
        std::cout << "Codice riconoscimento: "; std::getline(std::cin >> std::ws, codiceRiconoscimento);
        std::cout << "Tipo prodotto: "; std::getline(std::cin >> std::ws, tipoProdotto);
        std::cout << "Formato: "; std::getline(std::cin >> std::ws, formato);

        artista->addProduct(new CD(artista, title, description, prezzo, disponibile, quantita, codiceProdotto, produttoreStampe, codiceRiconoscimento, tipoProdotto, formato));
    
    } else if (scelta == 3) {
        double prezzo = SafeInput::read<double>("Prezzo: ");
        bool disponibile = SafeInput::readBool("Disponibile");
        unsigned int quantita = SafeInput::read<unsigned int>("Quantita: ");

        std::string codiceProdotto, produttoreStampe, codiceRiconoscimento, tipoProdotto;
        std::cout << "Codice prodotto: "; std::getline(std::cin >> std::ws, codiceProdotto);
        std::cout << "Produttore stampe: "; std::getline(std::cin >> std::ws, produttoreStampe);
        std::cout << "Codice riconoscimento: "; std::getline(std::cin >> std::ws, codiceRiconoscimento);
        std::cout << "Tipo prodotto: "; std::getline(std::cin >> std::ws, tipoProdotto);

        unsigned int rpm = SafeInput::read<unsigned int>("RPM: ");
        unsigned int diametro = SafeInput::read<unsigned int>("Diametro: ");
        artista->addProduct(new Vinile(artista, title, description, prezzo, disponibile, quantita, codiceProdotto, produttoreStampe, codiceRiconoscimento, tipoProdotto, rpm, diametro));
    
    } else if (scelta == 4) {
        unsigned int giorno = SafeInput::read<unsigned int>("Data di uscita - giorno: ");
        unsigned int mese   = SafeInput::read<unsigned int>("Data di uscita - mese: ");
        unsigned int anno   = SafeInput::read<unsigned int>("Data di uscita - anno: ");

        Data dataUscita(giorno, mese, anno);
        std::string genere, label;
        std::cout << "Genere: "; std::getline(std::cin >> std::ws, genere);
        std::cout << "Label discografica: "; std::getline(std::cin >> std::ws, label);
        std::vector<Traccia> tracce;
        unsigned int numTracce = SafeInput::read<unsigned int>("Numero tracce: ");
        for (unsigned int i=0; i<numTracce; ++i) {
            std::string nome, testo;
            bool hasTesto;
            unsigned int ore = SafeInput::read<unsigned int>("Durata traccia - ore: ");
            unsigned int minuti = SafeInput::read<unsigned int>("Durata traccia - minuti: ");
            unsigned int sec = SafeInput::read<unsigned int>("Durata traccia - secondi: ");

            std::cout<<"Nome traccia "<<i+1<<": "; std::getline(std::cin>>std::ws,nome);
            hasTesto = SafeInput::readBool("Ha testo");
            if (hasTesto) { std::cout<<"Testo: "; std::getline(std::cin>>std::ws,testo);} else testo="";
            
            std::vector<std::string> partecipanti;
            unsigned int numPartecipanti = SafeInput::read<unsigned int>("Partecipanti: ");
            for(unsigned int j=0; j<numPartecipanti; ++j){
                std::string p;
                std::cout<<"Partecipante "<<j+1<<": "; std::getline(std::cin>>std::ws,p);
                partecipanti.push_back(p);
            }
            tracce.emplace_back(nome, partecipanti, Durata(ore,minuti,sec), testo, hasTesto);
        }
        Album* album = new Album(artista, title, description, dataUscita, Durata(0,0,0), genere, tracce, label);
        album->updateDurata(); artista->addProduct(album);
    } else if (scelta==5) {
        unsigned int giorno=SafeInput::read<unsigned int>("Data di uscita - giorno: ");
        unsigned int mese = SafeInput::read<unsigned int>("Data di uscita - mese: ");
        unsigned int anno = SafeInput::read<unsigned int>("Data di uscita - anno: ");

        Data dataUscita(giorno,mese,anno);
        std::string genere;
        std::cout<<"Genere: "; std::getline(std::cin>>std::ws,genere);
        std::string nome,testo; bool hasTesto;
        unsigned int ore=SafeInput::read<unsigned int>("Durata principale - ore: ");
        unsigned int minuti=SafeInput::read<unsigned int>("Durata principale - minuti: ");
        unsigned int secondi=SafeInput::read<unsigned int>("Durata principale - secondi: ");

        std::cout<<"Nome traccia principale: "; std::getline(std::cin>>std::ws,nome);
        hasTesto=SafeInput::readBool("Ha testo");
        if(hasTesto){ std::cout<<"Testo: "; std::getline(std::cin>>std::ws,testo);} else testo="";

        std::vector<std::string> partecipanti;
        unsigned int numP=SafeInput::read<unsigned int>("Partecipanti: ");
        for(unsigned int j=0;j<numP;++j){
            std::string p; std::cout<<"Partecipante "<<j+1<<": "; std::getline(std::cin>>std::ws,p);
            partecipanti.push_back(p);
        }
        Traccia mainTrack(nome,partecipanti,Durata(ore,minuti,secondi),testo,hasTesto);
        bool isRemix=SafeInput::readBool("E' un remix");
        int chartPosition=SafeInput::read<int>("Posizione in classifica: ");

        Singolo* singolo=new Singolo(artista,title,description,dataUscita,Durata(0,0,0),genere,mainTrack,isRemix,chartPosition);
        singolo->updateDurata(); artista->addProduct(singolo);
    } else {
        unsigned int numDate = SafeInput::read<unsigned int>("Date tour: ");
        double prezzo = SafeInput::read<double>("Prezzo: ");
        bool disponibile = SafeInput::readBool("Disponibile");
        unsigned int quantita = SafeInput::read<unsigned int>("Quantita: ");

        Tour* tour = new Tour(artista,title,description,prezzo,disponibile,quantita);

        for(unsigned int i=0;i<numDate;++i){
            unsigned int g=SafeInput::read<unsigned int>("Data - giorno: ");
            unsigned int m=SafeInput::read<unsigned int>("Data - mese: ");
            unsigned int a=SafeInput::read<unsigned int>("Data - anno: ");
            unsigned int h=SafeInput::read<unsigned int>("Orario - ore: ");
            unsigned int min=SafeInput::read<unsigned int>("Orario - minuti: ");
            unsigned int sec=SafeInput::read<unsigned int>("Orario - secondi: ");
            std::cout<<"Luogo: "; std::string luogo; std::getline(std::cin>>std::ws,luogo);
            tour->addDataTour(DataTour(g,m,a,h,min,sec,luogo));
        }
        artista->addProduct(tour);
    }
}

// ----- ARTIST PRODUCT -----

void ConsoleEditorHandler::editTitoloProduct(ArtistProduct* p) {
    std::cout << "Nuovo titolo: ";
    std::string nuovo;
    std::getline(std::cin, nuovo);
    if (!nuovo.empty()) p->setTitle(nuovo);
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
    unsigned int g = SafeInput::read<unsigned int>("Nuova data uscita - giorno: ");
    unsigned int mth = SafeInput::read<unsigned int>("Nuova data uscita - mese: ");
    unsigned int a = SafeInput::read<unsigned int>("Nuova data uscita - anno: ");
    m->setDataUscita(Data(g,mth,a));
}

// ----- MERCH -----

void ConsoleEditorHandler::editPrezzoMerch(Merch* m) {
    double prezzo=SafeInput::read<double>("Nuovo prezzo: ");
    m->setPrezzo(prezzo);
}

void ConsoleEditorHandler::editDisponibileMerch(Merch* m) {
    bool disp=SafeInput::readBool("Disponibile");
    m->setDisponibile(disp);
}

void ConsoleEditorHandler::editQuantitaMerch(Merch* m) {
    unsigned int q=SafeInput::read<unsigned int>("Nuova quantita: ");
    m->setQuantita(q);
}

// ----- ALBUM -----

void ConsoleEditorHandler::editEtichettaAlbum(Album* album) {
    std::cout << "Nuova etichetta: "; std::string et; std::getline(std::cin>>std::ws,et);
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
        VisitorConsoleEditor v; t.accept(&v);
        album->updateDurata();
    }
}

// ----- SINGOLO -----

void ConsoleEditorHandler::editIsRemixSingolo(Singolo* s) {
    bool remix = SafeInput::readBool("E' un remix");
    s->setIsRemix(remix);
}

void ConsoleEditorHandler::editChartPositionSingolo(Singolo* s) {
    int pos = SafeInput::read<int>("Nuova posizione classifica: ");
    s->setChartPosition(pos);
}

void ConsoleEditorHandler::editMainTrackSingolo(Singolo* s) {
    VisitorConsoleEditor v; Traccia& t = s->getTracciaModificabile(); t.accept(&v);
    s->setDurata(t.getDurata());
}

// ----- CD -----

void ConsoleEditorHandler::editFormatoCD(CD* cd) {
    std::cout << "Nuovo formato: "; std::string f; std::getline(std::cin, f);
    cd->setFormato(f);
}

// ----- VINILE -----

void ConsoleEditorHandler::editRPMVinile(Vinile* v) {
    int rpm = SafeInput::read<int>("Nuovi RPM: ");
    v->setRpm(rpm);
}

void ConsoleEditorHandler::editDiametroVinile(Vinile* v) {
    int d = SafeInput::read<int>("Nuovo diametro (pollici): ");
    v->setDiametro(d);
}

// ----- TSHIRT -----

void ConsoleEditorHandler::editTagliaTShirt(TShirt* tshirt) {
    std::cout << "Nuova taglia: "; std::string taglia; std::getline(std::cin, taglia);
    tshirt->setTaglia(taglia);
}

void ConsoleEditorHandler::editColoreTShirt(TShirt* tshirt) {
    std::cout << "Nuovo colore: "; std::string colore; std::getline(std::cin, colore);
    tshirt->setColore(colore);
}

// ----- TOUR -----

void ConsoleEditorHandler::aggiungiDataTour(Tour* tour) {
    DataTour dt = creaDataTour(); tour->addDataTour(dt);
}

void ConsoleEditorHandler::rimuoviDataTour(Tour* tour, unsigned int index) {
    tour->removeDataTour(index);
}

void ConsoleEditorHandler::editDataTour(Tour* tour, unsigned int index) {
    auto& date = tour->getDateTourModificabile(); if (index < date.size()) { VisitorConsoleEditor v; date[index].accept(&v); }
}

// ----- TRACCIA -----

void ConsoleEditorHandler::editNomeTraccia(Traccia* t) {
    std::cout << "Nuovo nome: "; std::string nuovo; std::getline(std::cin, nuovo); t->setNome(nuovo);
}

void ConsoleEditorHandler::editDurataTraccia(Traccia* t) {
    int min = SafeInput::read<int>("Durata (minuti): ");
    int sec = SafeInput::read<int>("Durata (secondi): ");
    t->setDurata(Durata(0, min, sec));
}

void ConsoleEditorHandler::editTestoTraccia(Traccia* t) {
    std::cout << "Nuovo testo (vuoto se strumentale): "; std::string testo; std::getline(std::cin, testo);
    t->setTesto(testo); t->setHasTesto(!testo.empty());
}

void ConsoleEditorHandler::aggiungiPartecipanteTraccia(Traccia* t) {
    std::cout << "Nome partecipante: "; std::string p; std::getline(std::cin, p); t->addPartecipante(p);
}

void ConsoleEditorHandler::rimuoviPartecipanteTraccia(Traccia* t, unsigned int index) {
    t->removePartecipante(index);
}

void ConsoleEditorHandler::editPartecipanteTraccia(Traccia* t, unsigned int index) {
    std::cout << "Nuovo nome partecipante: "; std::string nuovo; std::getline(std::cin, nuovo); t->editPartecipante(index, nuovo);
}

void ConsoleEditorHandler::gestisciPartecipantiTraccia(Traccia* t) {
    int scelta = -1;
    while (scelta != 0) {
        std::cout << "\n--- Gestione Partecipanti ---\n";
        const auto& partecipanti = t->getPartecipanti();
        if (partecipanti.empty()) std::cout << "Nessun partecipante.\n";
        else {
            for (size_t i = 0; i < partecipanti.size(); ++i)
                std::cout << i+1 << ") " << partecipanti[i] << "\n";
        }
        std::cout << "\nOpzioni:\n1. Aggiungi\n2. Modifica\n3. Rimuovi\n0. Torna indietro\nScelta: ";
        scelta = SafeInput::read<int>("");
        switch (scelta) {
            case 1: aggiungiPartecipanteTraccia(t); break;
            case 2: {
                unsigned int idx = SafeInput::read<unsigned int>("Indice da modificare: ");
                if (idx>=1 && idx<=partecipanti.size()) editPartecipanteTraccia(t, idx-1);
                else std::cout<<"Indice non valido.\n";
                break;
            }
            case 3: {
                unsigned int idx = SafeInput::read<unsigned int>("Indice da rimuovere: ");
                if (idx>=1 && idx<=partecipanti.size()) rimuoviPartecipanteTraccia(t, idx-1);
                else std::cout<<"Indice non valido.\n";
                break;
            }
            case 0: std::cout<<"Uscita gestione partecipanti.\n"; break;
            default: std::cout<<"Scelta non valida.\n"; break;
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
    std::cout<<"Nuovo luogo: "; std::string l; std::getline(std::cin>>std::ws,l); d->setLuogo(l);
}

// ----- CREAZIONE OGGETTI -----

Traccia ConsoleEditorHandler::creaTraccia() {
    std::string nome, testo;
    unsigned int minuti = SafeInput::read<unsigned int>("Durata - minuti: ");
    unsigned int secondi = SafeInput::read<unsigned int>("Durata - secondi: ");
    std::cin.ignore();
    std::cout<<"Nome traccia: "; std::getline(std::cin, nome);
    std::cout<<"Testo (vuoto se strumentale): "; std::getline(std::cin, testo);
    bool hasTesto = !testo.empty();
    std::vector<std::string> partecipanti;
    while (true) {
        std::string p;
        std::cout<<"Aggiungi partecipante (vuoto fine): "; std::getline(std::cin, p);
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
    unsigned int mi= SafeInput::read<unsigned int>("Minuti: ");
    unsigned int s = SafeInput::read<unsigned int>("Secondi: ");
    std::cin.ignore();
    std::cout<<"Luogo: "; std::string luogo; std::getline(std::cin, luogo);
    return DataTour(g, m, a, h, mi, s, luogo);
}
