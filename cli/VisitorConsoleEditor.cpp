#include "VisitorConsoleEditor.h"

#include <iostream>
#include <unordered_map>
#include "../view/ErrorManager.h"

#include "../model/artisti/Artista.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/merch/TShirt.h"
#include "../model/tour/Tour.h"
#include "../model/musica/Traccia.h"
#include "../model/tour/DataTour.h"

#include "ConsoleEditorHandler.h"

void VisitorConsoleEditor::visit(Artista* artista) {
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
                                        std::cout << " - ID " << id << ": " << prod->getTitle() << "]\n";
                
                                    std::cout << "ID prodotto da editare: ";
                                    unsigned int pid;
                                    std::cin >> pid;
                                    std::cin.ignore();
                
                                    auto it = prodotti.find(pid);
                                    if (it != prodotti.end()) {
                                        it->second->accept(this); // dispatch polimorfico corretto
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
                                        std::cout << " - ID " << id << ": " << prod->getTitle() << "\n";
                
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
            ErrorManager::showError("Errore sconosciuto nella edit artista.");
        }
    }
}

void VisitorConsoleEditor::visit(Album* album) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- edit Album: " << album->getTitle() << " ---\n";
        std::cout << "1. edit titolo\n";
        std::cout << "2. edit descrizione\n";
        std::cout << "3. edit immagine\n";
        std::cout << "4. edit genere\n";
        std::cout << "5. edit data di uscita\n";
        std::cout << "6. edit etichetta\n";
        std::cout << "7. Gestisci tracce\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore();

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editTitoloProduct(album); break;
                case 2: ConsoleEditorHandler::editDescrizioneProduct(album); break;
                case 3: ConsoleEditorHandler::editImagePathProduct(album); break;
                case 4: ConsoleEditorHandler::editGenereMusica(album); break;
                case 5: ConsoleEditorHandler::editDataUscitaMusica(album); break;
                case 6: ConsoleEditorHandler::editEtichettaAlbum(album); break;
                case 7: {
                    bool fineTracce = false;
                    while (!fineTracce) {
                        std::cout << "\n--- Gestione Tracce ---\n";
                        const auto& tracce = album->getTracce();
                        if (tracce.empty()) {
                            std::cout << "Nessuna traccia presente.\n";
                        } else {
                            for (size_t i = 0; i < tracce.size(); ++i)
                                std::cout << i + 1 << ". " << tracce[i].getNome() << " (" 
                                          << tracce[i].getDurata().toString() << ")\n";
                        }

                        std::cout << "a. Aggiungi traccia\n";
                        std::cout << "m. Modifica traccia\n";
                        std::cout << "r. Rimuovi traccia\n";
                        std::cout << "x. Torna indietro\n";
                        std::cout << "Scelta: ";

                        std::string opzione;
                        std::getline(std::cin, opzione);

                        try {
                            if (opzione == "a") {
                                ConsoleEditorHandler::aggiungiTracciaAlbum(album);
                            } else if (opzione == "m") {
                                if (tracce.empty()) {
                                    std::cout << "Non ci sono tracce da editare.\n";
                                } else {
                                    int index;
                                    std::cout << "Numero traccia da editare: ";
                                    std::cin >> index;
                                    std::cin.ignore();
                                    if (index >= 1 && static_cast<size_t>(index) <= tracce.size()) {
                                        ConsoleEditorHandler::editTracciaAlbum(album, index - 1);
                                    } else {
                                        std::cout << "Numero traccia non valido.\n";
                                    }
                                }
                            } else if (opzione == "r") {
                                if (tracce.empty()) {
                                    std::cout << "Non ci sono tracce da rimuovere.\n";
                                } else {
                                    int index;
                                    std::cout << "Numero traccia da rimuovere: ";
                                    std::cin >> index;
                                    std::cin.ignore();
                                    if (index >= 1 && static_cast<size_t>(index) <= tracce.size()) {
                                        ConsoleEditorHandler::rimuoviTracciaAlbum(album, index - 1);
                                    } else {
                                        std::cout << "Numero traccia non valido.\n";
                                    }
                                }
                            } else if (opzione == "x") {
                                fineTracce = true;
                            } else {
                                std::cout << "Scelta non valida.\n";
                            }

                        } catch (const std::exception& ex) {
                            ErrorManager::showError(ex.what());
                        } catch (...) {
                            ErrorManager::showError("Errore sconosciuto durante la gestione tracce.");
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
            ErrorManager::showError("Errore sconosciuto nella edit album.");
        }
    }
}

void VisitorConsoleEditor::visit(Singolo* singolo) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica Singolo: " << singolo->getTitle() << " ---\n";
        std::cout << "1. Modifica titolo\n";
        std::cout << "2. Modifica descrizione\n";
        std::cout << "3. Modifica immagine\n";
        std::cout << "4. Modifica genere\n";
        std::cout << "5. Modifica data di uscita\n";
        std::cout << "6. Modifica posizione in classifica\n";
        std::cout << "7. Modifica se è un remix\n";
        std::cout << "8. Modifica traccia principale\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            switch (scelta) {
                case 1:
                    ConsoleEditorHandler::editTitoloProduct(singolo);
                    break;
                case 2:
                    ConsoleEditorHandler::editDescrizioneProduct(singolo);
                    break;
                case 3:
                    ConsoleEditorHandler::editImagePathProduct(singolo);
                    break;
                case 4:
                    ConsoleEditorHandler::editGenereMusica(singolo);
                    break;
                case 5:
                    ConsoleEditorHandler::editDataUscitaMusica(singolo);
                    break;
                case 6:
                    ConsoleEditorHandler::editChartPositionSingolo(singolo);
                    break;
                case 7:
                    ConsoleEditorHandler::editIsRemixSingolo(singolo);
                    break;
                case 8:
                    ConsoleEditorHandler::editMainTrackSingolo(singolo);
                    break;
                case 0:
                    fine = true;
                    break;
                default:
                    std::cout << "Scelta non valida.\n";
                    break;
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la edit del singolo.");
        }
    }
}

void VisitorConsoleEditor::visit(Traccia* traccia) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- edit Traccia: " << traccia->getNome() << " ---\n";
        std::cout << "1. edit nome\n";
        std::cout << "2. edit durata\n";
        std::cout << "3. edit testo\n";
        std::cout << "4. Gestisci partecipanti\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            switch (scelta) {
                case 1:
                    ConsoleEditorHandler::editNomeTraccia(traccia);
                    break;
                case 2:
                    ConsoleEditorHandler::editDurataTraccia(traccia);
                    break;
                case 3:
                    ConsoleEditorHandler::editTestoTraccia(traccia);
                    break;
                case 4:
                    // Nuovo metodo centralizzato per gestione partecipanti
                    ConsoleEditorHandler::gestisciPartecipantiTraccia(traccia);
                    break;
                case 0:
                    fine = true;
                    break;
                default:
                    std::cout << "Scelta non valida.\n";
                    break;
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la edit della traccia.");
        }
    }
}

void VisitorConsoleEditor::visit(CD* cd) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- edit CD: " << cd->getTitle() << " ---\n";
        std::cout << "1. edit titolo\n";
        std::cout << "2. edit descrizione\n";
        std::cout << "3. edit immagine\n";
        std::cout << "4. edit prezzo\n";
        std::cout << "5. edit disponibilità\n";
        std::cout << "6. edit quantità\n";
        std::cout << "7. edit formato\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editTitoloProduct(cd); break;
                case 2: ConsoleEditorHandler::editDescrizioneProduct(cd); break;
                case 3: ConsoleEditorHandler::editImagePathProduct(cd); break;
                case 4: ConsoleEditorHandler::editPrezzoMerch(cd); break;
                case 5: ConsoleEditorHandler::editDisponibileMerch(cd); break;
                case 6: ConsoleEditorHandler::editQuantitaMerch(cd); break;
                case 7: ConsoleEditorHandler::editFormatoCD(cd); break;
                case 0: fine = true; break;
                default: std::cout << "Scelta non valida.\n";
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la edit del CD.");
        }
    }
}

void VisitorConsoleEditor::visit(Vinile* vinile) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- edit Vinile: " << vinile->getTitle() << " ---\n";
        std::cout << "1. Modifica titolo\n";
        std::cout << "2. Modifica descrizione\n";
        std::cout << "3. Modifica immagine\n";
        std::cout << "4. Modifica prezzo\n";
        std::cout << "5. Modifica disponibilità\n";
        std::cout << "6. Modifica quantità\n";
        std::cout << "7. Modifica RPM\n";
        std::cout << "8. Modifica diametro\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editTitoloProduct(vinile); break;
                case 2: ConsoleEditorHandler::editDescrizioneProduct(vinile); break;
                case 3: ConsoleEditorHandler::editImagePathProduct(vinile); break;
                case 4: ConsoleEditorHandler::editPrezzoMerch(vinile); break;
                case 5: ConsoleEditorHandler::editDisponibileMerch(vinile); break;
                case 6: ConsoleEditorHandler::editQuantitaMerch(vinile); break;
                case 7: ConsoleEditorHandler::editRPMVinile(vinile); break;
                case 8: ConsoleEditorHandler::editDiametroVinile(vinile); break;
                case 0: fine = true; break;
                default: std::cout << "Scelta non valida.\n";
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la edit del vinile.");
        }
    }
}

void VisitorConsoleEditor::visit(TShirt* tshirt) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- edit TShirt: " << tshirt->getTitle() << " ---\n";
        std::cout << "1. edit titolo\n";
        std::cout << "2. edit descrizione\n";
        std::cout << "3. edit immagine\n";
        std::cout << "4. edit prezzo\n";
        std::cout << "5. edit disponibilità\n";
        std::cout << "6. edit quantità\n";
        std::cout << "7. edit taglia\n";
        std::cout << "8. edit colore\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editTitoloProduct(tshirt); break;
                case 2: ConsoleEditorHandler::editDescrizioneProduct(tshirt); break;
                case 3: ConsoleEditorHandler::editImagePathProduct(tshirt); break;
                case 4: ConsoleEditorHandler::editPrezzoMerch(tshirt); break;
                case 5: ConsoleEditorHandler::editDisponibileMerch(tshirt); break;
                case 6: ConsoleEditorHandler::editQuantitaMerch(tshirt); break;
                case 7: ConsoleEditorHandler::editTagliaTShirt(tshirt); break;
                case 8: ConsoleEditorHandler::editColoreTShirt(tshirt); break;
                case 0: fine = true; break;
                default:
                    std::cout << "Scelta non valida.\n";
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la edit della TShirt.");
        }
    }
}

void VisitorConsoleEditor::visit(Tour* tour) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- edit Tour: " << tour->getTitle() << " ---\n";
        std::cout << "1. Modifica titolo\n";
        std::cout << "2. Modifica descrizione\n";
        std::cout << "3. Modifica immagine\n";
        std::cout << "4. Gestisci date del tour\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editTitoloProduct(tour); break;
                case 2: ConsoleEditorHandler::editDescrizioneProduct(tour); break;
                case 3: ConsoleEditorHandler::editImagePathProduct(tour); break;
                case 4: {
                    bool fineDate = false;
                    while (!fineDate) {
                        std::cout << "\n--- Gestione Date del Tour ---\n";
                        const auto& date = tour->getDateTour();
                        if (date.empty()) {
                            std::cout << "Nessuna data attualmente.\n";
                        } else {
                            for (size_t i = 0; i < date.size(); ++i) {
                                std::cout << i + 1 << ". ";
                                std::cout << i << ") " << date[i].toString() << "\n";
                            }
                        }

                        std::cout << "a. Aggiungi nuova data\n";
                        std::cout << "m. Modifica data esistente\n";
                        std::cout << "r. Rimuovi data\n";
                        std::cout << "x. Torna indietro\n";
                        std::cout << "Scelta: ";

                        std::string opzione;
                        std::getline(std::cin, opzione);

                        try {
                            if (opzione == "a") {
                                ConsoleEditorHandler::aggiungiDataTour(tour);
                            } else if (opzione == "m") {
                                int index;
                                std::cout << "Numero data da editare: ";
                                std::cin >> index;
                                std::cin.ignore();
                                if (index >= 1 && index <= static_cast<int>(date.size())) {
                                    DataTour& data = const_cast<DataTour&>(date[index - 1]);
                                    data.accept(this);
                                } else {
                                    std::cout << "Numero non valido.\n";
                                }
                            } else if (opzione == "r") {
                                int index;
                                std::cout << "Numero data da rimuovere: ";
                                std::cin >> index;
                                std::cin.ignore();
                                if (index >= 1 && index <= static_cast<int>(date.size())) {
                                    ConsoleEditorHandler::rimuoviDataTour(tour, index - 1);
                                } else {
                                    std::cout << "Numero non valido.\n";
                                }
                            } else if (opzione == "x") {
                                fineDate = true;
                            } else {
                                std::cout << "Scelta non valida.\n";
                            }
                        } catch (const std::exception& ex) {
                            ErrorManager::showError(ex.what());
                        } catch (...) {
                            ErrorManager::showError("Errore sconosciuto nella gestione delle date.");
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
            ErrorManager::showError("Errore sconosciuto durante la edit del Tour.");
        }
    }
}

void VisitorConsoleEditor::visit(DataTour* dataTour) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- edit Data del Tour ---\n";
        std::cout << "1. Modifica data (giorno/mese/anno)\n";
        std::cout << "2. Modifica orario (ore/minuti/secondi)\n";
        std::cout << "3. Modifica luogo\n";
        std::cout << "0. Torna indietro\n";
        std::cout << "Scelta: ";

        int scelta;
        std::cin >> scelta;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try {
            switch (scelta) {
                case 1:
                    ConsoleEditorHandler::editDataDataTour(dataTour);
                    break;
                case 2:
                    ConsoleEditorHandler::editOrarioDataTour(dataTour);
                    break;
                case 3:
                    ConsoleEditorHandler::editLuogoDataTour(dataTour);
                    break;
                case 0:
                    fine = true;
                    break;
                default:
                    std::cout << "Scelta non valida.\n";
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la edit della DataTour.");
        }
    }
}