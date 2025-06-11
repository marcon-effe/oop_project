#include "ConsoleArtistEditor.h"
#include "SafeInput.h"
#include "ConsoleEditorHandler.h"
#include <iostream>
#include <limits>

#include "../view/ErrorManager.h"
#include "../visitors/VisitorConsoleEditor.h"

// so che il blocco di codice seguente contiene molti controlli annidati ma si è reso necessario questo approccio
// per implementare l'assoluta uncità (speriamo) di nomi di artisti e nomi prodotti in seguito alla progettazione
// del sistema di gestione delle immagini 

void ConsoleArtistEditor::modificaArtista(const std::unordered_map<unsigned int, Artista*>& artisti, Artista* artista) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n=== Modifica Artista: " << artista->getNome() << " ===\n"
                  << "1. Modifica nome\n"
                  << "2. Modifica genere\n"
                  << "3. Modifica descrizione/biografia\n"
                  << "4. Modifica immagine\n"
                  << "5. Gestisci prodotti\n"
                  << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

        try {
            switch (scelta) {
                case 1:{
                    std::cout << "Modifica nome dell'artista: ";
                    std::string nuvonome;
                    std::getline(std::cin, nuvonome);
                    if (nuvonome.empty()) {
                        std::cout << "Nome non valido. Riprova.\n";
                        break;
                    }

                    for (const auto& [id, a] : artisti) {
                        std::cout << "Controllo nome: " << a->getNome() << "\n";
                        if (a->getNome() == nuvonome) {
                            std::cout << "Nome già in uso da un altro artista.\n";
                            return;
                        }
                    }
                    ConsoleEditorHandler::editNomeArtista(artista, nuvonome);
                    break;
                }
                case 2:
                    ConsoleEditorHandler::editGenereArtista(artista);
                    break;
                case 3:
                    ConsoleEditorHandler::editInfoArtista(artista);
                    break;
                case 4:
                    ConsoleEditorHandler::editImagePathArtista(artista);
                    break;
                case 5: {
                    bool esciProdotti = false;
                    while (!esciProdotti) {
                        std::cout << "\n--- Gestione Prodotti ---\n"
                                  << "1. Aggiungi nuovo prodotto\n"
                                  << "2. Modifica prodotto esistente\n"
                                  << "3. Rimuovi prodotto\n"
                                  << "0. Torna indietro\n";

                        int sceltaProd = SafeInput::read<int>("Scelta: ");

                        try {
                            switch (sceltaProd) {
                                case 1:
                                    ConsoleEditorHandler::aggiungiProdottoArtista(artista);
                                    break;
                                case 2: {
                                    const auto& prodotti = artista->getProducts();
                                    if (prodotti.empty()) {
                                        std::cout << "Nessun prodotto da editare.\n";
                                        break;
                                    }
                                    std::cout << "Prodotti disponibili:\n";
                                    for (const auto& [id, prod] : prodotti)
                                        std::cout << "ID " << id << ": " << prod->getTitle() << "\n";

                                    unsigned int pid = SafeInput::read<unsigned int>("ID prodotto da editare: ");
                                    auto it = prodotti.find(pid);
                                    if (it != prodotti.end()) {
                                        std::cout << "Scegli il tipo di modifica:\n"
                                                  << "1. Modifica campi comuni\n"
                                                  << "2. Modifica campi specifici del prodotto\n"
                                                  << "0. Annulla\n";
                                        int sceltaModifica = SafeInput::read<int>("Scelta: ");
                                        switch(sceltaModifica) {
                                            case 1:{
                                                std::cout << "Scegli il campo da modificare:\n"
                                                          << "1. Modifica titolo\n"
                                                          << "2. Modifica descrizione\n"
                                                          << "3. Modifica immagine\n";
                                                int campo = SafeInput::read<int>("Scelta: ");
                                                switch(campo) {
                                                    case 1:{
                                                        std::string nuovoTitolo;
                                                        std::cout << "Inserisci il nuovo titolo: ";
                                                        std::getline(std::cin, nuovoTitolo);
                                                        if (nuovoTitolo.empty()) {
                                                            std::cout << "Titolo non valido.\n";
                                                            break;
                                                        }
                                                        for (const auto& [id, prod] : prodotti) {
                                                            if (prod->getTitle() == nuovoTitolo && id != pid) {
                                                                std::cout << "Titolo già in uso da un altro prodotto.\n";
                                                                return;
                                                            }
                                                        }
                                                        it->second->setTitle(nuovoTitolo);
                                                        break;
                                                    }
                                                    case 2:
                                                        ConsoleEditorHandler::editDescrizioneProduct(it->second);
                                                        break;
                                                    case 3:
                                                        ConsoleEditorHandler::editImagePathProduct(it->second);
                                                        break;
                                                    default:
                                                        std::cout << "Scelta non valida.\n";
                                                }
                                                break;
                                            }
                                            case 2: {
                                                VisitorConsoleEditor visitor;
                                                it->second->accept(&visitor);
                                                std::cout << "Modifica completata.\n";
                                                break;
                                            }
                                            case 0:
                                                std::cout << "Modifica annullata.\n";
                                                break;
                                            default:
                                                std::cout << "Scelta non valida.\n";
                                        }
                                    } else {
                                        std::cout << "ID non trovato.\n";
                                    }
                                    break;
                                }
                                case 3: {
                                    const auto& prodotti = artista->getProducts();
                                    if (prodotti.empty()) {
                                        std::cout << "Nessun prodotto da eliminare.\n";
                                        break;
                                    }
                                    std::cout << "Prodotti disponibili:\n";
                                    for (const auto& [id, prod] : prodotti)
                                        std::cout << "ID " << id << ": " << prod->getTitle() << "\n";

                                    unsigned int pid = SafeInput::read<unsigned int>("ID prodotto da eliminare: ");
                                    auto it = prodotti.find(pid);
                                    if (it != prodotti.end()) {
                                        artista->removeProduct(pid);
                                    } else {
                                        std::cout << "ID non trovato.\n";
                                    }
                                    break;
                                }
                                case 0:
                                    esciProdotti = true;
                                    break;
                                default:
                                    std::cout << "Scelta non valida.\n";
                                    break;
                            }
                        } catch (const std::exception& ex) {
                            ErrorManager::showError(ex.what());
                        } catch (...) {
                            ErrorManager::showError("Errore sconosciuto nella gestione prodotti.");
                        }
                    }
                    break;
                }
                case 0:
                    fine = true;
                    break;
                default:
                    std::cout << "Scelta non valida.\n";
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto nella modifica artista.");
        }
    }
}
