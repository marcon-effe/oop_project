#include "ConsoleArtistEditor.h"
#include "ConsoleEditorHandler.h"  // metodi editNomeArtista, editGenereArtista, etc.
#include <iostream>

#include "../view/ErrorManager.h"
#include "../visitors/VisitorConsoleEditor.h"

void ConsoleArtistEditor::modificaArtista(Artista* artista) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n=== Modifica Artista: " << artista->getNome() << " ===\n";
        std::cout << "1. Modifica nome\n";
        std::cout << "2. Modifica genere\n";
        std::cout << "3. Modifica descrizione/biografia\n";
        std::cout << "4. Modifica immagine\n";
        std::cout << "5. Gestisci prodotti\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore();

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editNomeArtista(artista); break;
                case 2: ConsoleEditorHandler::editGenereArtista(artista); break;
                case 3: ConsoleEditorHandler::editInfoArtista(artista); break;
                case 4: ConsoleEditorHandler::editImagePathArtista(artista); break;
                case 5: {
                    bool esciProdotti = false;
                    while (!esciProdotti) {
                        std::cout << "\n--- Gestione Prodotti ---\n";
                        std::cout << "1. Aggiungi nuovo prodotto\n";
                        std::cout << "2. Modifica prodotto esistente\n";
                        std::cout << "3. Rimuovi prodotto\n";
                        std::cout << "0. Torna indietro\n";
                        std::cout << "Scelta: ";
                
                        int sceltaProd;
                        std::cin >> sceltaProd;
                        std::cin.ignore();
                
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
                
                                    std::cout << "ID prodotto da editare: ";
                                    unsigned int pid;
                                    std::cin >> pid;
                                    std::cin.ignore();
                
                                    auto it = prodotti.find(pid);
                                    if (it != prodotti.end()) {
                                        VisitorConsoleEditor visitor;
                                        it->second->accept(&visitor); // dispatch polimorfico corretto
                                        std::cout << "modifica prodotto completata.\n";
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
                
                                    std::cout << "ID prodotto da eliminare: ";
                                    unsigned int pid;
                                    std::cin >> pid;
                                    std::cin.ignore();
                
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
                            }
                        } catch (const std::exception& ex) {
                            ErrorManager::showError(ex.what());
                        } catch (...) {
                            ErrorManager::showError("Errore sconosciuto nella gestione prodotti.");
                        }
                    }
                    break;
                }
                case 0: fine = true; break;
                default: std::cout << "Scelta non valida.\n";
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto nella modifica artista.");
        }
    }
}
