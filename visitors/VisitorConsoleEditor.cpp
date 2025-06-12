#include "VisitorConsoleEditor.h"

#include <iostream>
#include <unordered_map>
#include "../view/ErrorManager.h"

#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/merch/TShirt.h"
#include "../model/tour/Tour.h"
#include "../model/musica/Traccia.h"
#include "../model/tour/DataTour.h"

#include "../cli/ConsoleEditorHandler.h"
#include "../cli/SafeInput.h"

void VisitorConsoleEditor::visit(Album* album) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica Album: " << album->getTitle() << " ---\n";
        std::cout << "1. Modifica genere\n";
        std::cout << "2. Modifica data di uscita\n";
        std::cout << "3. Modifica etichetta\n";
        std::cout << "4. Gestisci tracce\n";
        std::cout << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editGenereMusica(album); break;
                case 2: ConsoleEditorHandler::editDataUscitaMusica(album); break;
                case 3: ConsoleEditorHandler::editEtichettaAlbum(album); break;
                case 4: {
                    bool fineTracce = false;
                    while (!fineTracce) {
                        std::cout << "\n--- Gestione Tracce ---\n";
                        const auto& tracce = album->getTracce();
                        if (tracce.empty()) {
                            std::cout << "Nessuna traccia presente.\n";
                        } else {
                            for (size_t i = 0; i < tracce.size(); ++i)
                                std::cout << i + 1 << ". " << tracce[i].getNome()
                                          << " (" << tracce[i].getDurata().toString() << ")\n";
                        }

                        std::cout << "a. Aggiungi traccia\n";
                        std::cout << "m. Modifica traccia\n";
                        std::cout << "r. Rimuovi traccia\n";
                        std::cout << "x. Torna indietro\n";

                        std::string opzione = SafeInput::read("Scelta: ");

                        try {
                            if (opzione == "a") {
                                ConsoleEditorHandler::aggiungiTracciaAlbum(album);
                            } else if (opzione == "m") {
                                if (tracce.empty()) {
                                    std::cout << "Non ci sono tracce da modificare.\n";
                                } else {
                                    int index = SafeInput::read<int>("Numero traccia da modificare: ");
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
                                    int index = SafeInput::read<int>("Numero traccia da rimuovere: ");
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
                default:
                    std::cout << "Scelta non valida.\n";
                    fine = true;
                    break;
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto nella modifica dell'album.");
        }
    }
}

void VisitorConsoleEditor::visit(Singolo* singolo) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica Singolo: " << singolo->getTitle() << " ---\n";
        std::cout << "1. Modifica genere\n";
        std::cout << "2. Modifica data di uscita\n";
        std::cout << "3. Modifica posizione in classifica\n";
        std::cout << "4. Modifica se è un remix\n";
        std::cout << "5. Modifica traccia principale\n";
        std::cout << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

        try {
            switch (scelta) {
                case 1:
                    ConsoleEditorHandler::editGenereMusica(singolo);
                    break;
                case 2:
                    ConsoleEditorHandler::editDataUscitaMusica(singolo);
                    break;
                case 3:
                    ConsoleEditorHandler::editChartPositionSingolo(singolo);
                    break;
                case 4:
                    ConsoleEditorHandler::editIsRemixSingolo(singolo);
                    break;
                case 5:
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
            ErrorManager::showError("Errore sconosciuto durante la modifica del singolo.");
        }
    }
}

void VisitorConsoleEditor::visit(Traccia* traccia) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica Traccia: " << traccia->getNome() << " ---\n";
        std::cout << "1. Modifica durata\n";
        std::cout << "2. Modifica testo\n";
        std::cout << "3. Gestisci partecipanti\n";
        std::cout << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editDurataTraccia(traccia); break;
                case 2: ConsoleEditorHandler::editTestoTraccia(traccia); break;
                case 3: ConsoleEditorHandler::gestisciPartecipantiTraccia(traccia); break;
                case 0: fine = true; break;
                default: std::cout << "Scelta non valida.\n"; break;
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la modifica della traccia.");
        }
    }
}

void VisitorConsoleEditor::visit(CD* cd) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica CD: " << cd->getTitle() << " ---\n";
        std::cout << "1. Modifica prezzo\n";
        std::cout << "2. Modifica disponibilità\n";
        std::cout << "3. Modifica quantità\n";
        std::cout << "4. Modifica formato\n";
        std::cout << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editPrezzoMerch(cd); break;
                case 2: ConsoleEditorHandler::editDisponibileMerch(cd); break;
                case 3: ConsoleEditorHandler::editQuantitaMerch(cd); break;
                case 4: ConsoleEditorHandler::editFormatoCD(cd); break;
                case 0: fine = true; break;
                default: std::cout << "Scelta non valida.\n"; break;
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la modifica del CD.");
        }
    }
}

void VisitorConsoleEditor::visit(Vinile* vinile) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica Vinile: " << vinile->getTitle() << " ---\n";
        std::cout << "1. Modifica prezzo\n";
        std::cout << "2. Modifica disponibilità\n";
        std::cout << "3. Modifica quantità\n";
        std::cout << "4. Modifica RPM\n";
        std::cout << "5. Modifica diametro\n";
        std::cout << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editPrezzoMerch(vinile); break;
                case 2: ConsoleEditorHandler::editDisponibileMerch(vinile); break;
                case 3: ConsoleEditorHandler::editQuantitaMerch(vinile); break;
                case 4: ConsoleEditorHandler::editRPMVinile(vinile); break;
                case 5: ConsoleEditorHandler::editDiametroVinile(vinile); break;
                case 0: fine = true; break;
                default: std::cout << "Scelta non valida.\n"; break;
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la modifica del vinile.");
        }
    }
}

void VisitorConsoleEditor::visit(TShirt* tshirt) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica TShirt: " << tshirt->getTitle() << " ---\n";
        std::cout << "1. Modifica prezzo\n";
        std::cout << "2. Modifica disponibilità\n";
        std::cout << "3. Modifica quantità\n";
        std::cout << "4. Modifica taglia\n";
        std::cout << "5. Modifica colore\n";
        std::cout << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

        try {
            switch (scelta) {
                case 1: ConsoleEditorHandler::editPrezzoMerch(tshirt); break;
                case 2: ConsoleEditorHandler::editDisponibileMerch(tshirt); break;
                case 3: ConsoleEditorHandler::editQuantitaMerch(tshirt); break;
                case 4: ConsoleEditorHandler::editTagliaTShirt(tshirt); break;
                case 5: ConsoleEditorHandler::editColoreTShirt(tshirt); break;
                case 0: fine = true; break;
                default: std::cout << "Scelta non valida.\n"; break;
            }
        } catch (const std::exception& ex) {
            ErrorManager::showError(ex.what());
        } catch (...) {
            ErrorManager::showError("Errore sconosciuto durante la modifica della TShirt.");
        }
    }
}
void VisitorConsoleEditor::visit(Tour* tour) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica Tour: " << tour->getTitle() << " ---\n";
        std::cout << "1. Gestisci date del tour\n";
        std::cout << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

        try {
            switch (scelta) {
                case 1: {
                    bool fineDate = false;
                    while (!fineDate) {
                        std::cout << "\n--- Gestione Date del Tour ---\n";
                        const auto& date = tour->getDateTour();
                        if (date.empty()) {
                            std::cout << "Nessuna data attualmente.\n";
                        } else {
                            for (size_t i = 0; i < date.size(); ++i) {
                                std::cout << i + 1 << ". " << date[i].toString() << "\n";
                            }
                        }

                        std::cout << "a. Aggiungi nuova data\n";
                        std::cout << "m. Modifica data esistente\n";
                        std::cout << "r. Rimuovi data\n";
                        std::cout << "x. Torna indietro\n";

                        std::string opzione = SafeInput::read("Scelta: ");

                        try {
                            if (opzione == "a") {
                                ConsoleEditorHandler::aggiungiDataTour(tour);
                            } 
                            else if (opzione == "m") {
                                if (date.empty()) {
                                    std::cout << "Non ci sono date da modificare.\n";
                                } else {
                                    int index = SafeInput::read<int>("Numero data da modificare: ");
                                    if (index >= 1 && static_cast<size_t>(index) <= date.size()) {
                                        DataTour& data = const_cast<DataTour&>(date[index - 1]);
                                        data.accept(this);
                                    } else {
                                        std::cout << "Numero non valido.\n";
                                    }
                                }
                            } 
                            else if (opzione == "r") {
                                if (date.empty()) {
                                    std::cout << "Non ci sono date da rimuovere.\n";
                                } else {
                                    int index = SafeInput::read<int>("Numero data da rimuovere: ");
                                    if (index >= 1 && static_cast<size_t>(index) <= date.size()) {
                                        ConsoleEditorHandler::rimuoviDataTour(tour, index - 1);
                                    } else {
                                        std::cout << "Numero non valido.\n";
                                    }
                                }
                            } 
                            else if (opzione == "x") {
                                fineDate = true;
                            } 
                            else {
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
            ErrorManager::showError("Errore sconosciuto durante la modifica del Tour.");
        }
    }
}

void VisitorConsoleEditor::visit(DataTour* dataTour) {
    bool fine = false;
    while (!fine) {
        std::cout << "\n--- Modifica Data del Tour ---\n";
        std::cout << "1. Modifica data (giorno/mese/anno)\n";
        std::cout << "2. Modifica orario (ore/minuti/secondi)\n";
        std::cout << "3. Modifica luogo\n";
        std::cout << "0. Torna indietro\n";

        int scelta = SafeInput::read<int>("Scelta: ");

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
