#ifndef CONSOLEEDITORHANDLER_H
#define CONSOLEEDITORHANDLER_H

#include<string>

class Artista;
class ArtistProduct;
class Musica;
class Album;
class Singolo;
class Merch;
class TShirt;
class Disco;
class CD;
class Vinile;
class Tour;
class DataTour;
class Traccia;

class ConsoleEditorHandler {
public:
    // ARTISTA
    static void editNomeArtista(Artista* a, const std::string& nuovoNome);
    static void editGenereArtista(Artista* a);
    static void editInfoArtista(Artista* a);
    static void editImagePathArtista(Artista* a);

    // ARTIST PRODUCT (base)
    static void editDescrizioneProduct(ArtistProduct* p);
    static void editImagePathProduct(ArtistProduct* p);

    // MUSICA
    static void editGenereMusica(Musica* m);
    static void editDataUscitaMusica(Musica* m);

    // MERCH
    static void editPrezzoMerch(Merch* m);
    static void editDisponibileMerch(Merch* m);
    static void editQuantitaMerch(Merch* m);

    // ALBUM
    static void editEtichettaAlbum(Album* album);
    static void aggiungiTracciaAlbum(Album* album);
    static void rimuoviTracciaAlbum(Album* album, unsigned int index);
    static void editTracciaAlbum(Album* album, unsigned int index);

    // SINGOLO
    static void editMainTrackSingolo(Singolo* s);
    static void editIsRemixSingolo(Singolo* sing);
    static void editChartPositionSingolo(Singolo* sing);

    // CD
    static void editFormatoCD(CD* cd);

    // VINILE
    static void editRPMVinile(Vinile* vin);
    static void editDiametroVinile(Vinile* vin);

    // TSHIRT
    static void editTagliaTShirt(TShirt* tshirt);
    static void editColoreTShirt(TShirt* tshirt);

    // TOUR
    static void aggiungiDataTour(Tour* tour);
    static void rimuoviDataTour(Tour* tour, unsigned int index);
    static void editDataTour(Tour* tour, unsigned int index);

    // TRACCIA
    static void editNomeTraccia(Traccia* traccia);
    static void editDurataTraccia(Traccia* traccia);
    static void editTestoTraccia(Traccia* traccia);
    static void gestisciPartecipantiTraccia(Traccia* traccia);

    static void aggiungiPartecipanteTraccia(Traccia* traccia);
    static void editPartecipanteTraccia(Traccia* traccia, unsigned int index);
    static void rimuoviPartecipanteTraccia(Traccia* traccia, unsigned int index);

    // DATATOUR
    static void editDataDataTour(DataTour* dt);
    static void editOrarioDataTour(DataTour* dt);
    static void editLuogoDataTour(DataTour* dt);

    // CREAZIONE NUOVI
    static Traccia creaTraccia();
    static DataTour creaDataTour();
    static Artista* creaArtista(const std::string& nome);
    static void aggiungiProdottoArtista(Artista* artista);
};

#endif // CONSOLEEDITORHANDLER_H