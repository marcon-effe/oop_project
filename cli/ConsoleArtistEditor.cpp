#include "ConsoleArtistEditor.h"
#include "ConsoleEditorHandler.h"  // metodi editNomeArtista, editGenereArtista, etc.
#include <iostream>

void ConsoleArtistEditor::modificaArtista(Artista* artista) {
    std::cout << "\n=== Modifica Artista: " << artista->getNome() << " ===\n";
    std::cout << "1. Modifica nome\n";
    std::cout << "2. Modifica genere\n";
    std::cout << "3. Modifica info\n";
    std::cout << "4. Modifica immagine\n";
    std::cout << "0. Annulla\n";

    int scelta;
    std::cin >> scelta;
    switch (scelta) {
        case 1:
            ConsoleEditorHandler::editNomeArtista(artista);
            break;
        case 2:
            ConsoleEditorHandler::editGenereArtista(artista);
            break;
        case 3:
            ConsoleEditorHandler::editInfoArtista(artista);
            break;
        case 4:
            ConsoleEditorHandler::editImagePathArtista(artista);
            break;
        default:
            break;
    }

    ConsoleEditorHandler::aggiungiProdottoArtista(artista);
}
