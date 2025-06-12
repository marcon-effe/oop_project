#include "ConsoleApp.h"
#include "SafeInput.h"
#include <iostream>
#include <filesystem>
#include <QDir>

#include "../data/DataManager.h"
#include "../view/ErrorManager.h"
#include "cli/ConsoleArtistEditor.h"
#include "ConsoleEditorHandler.h"

ConsoleApp::ConsoleApp() {}

ConsoleApp::~ConsoleApp() {
    for (auto& pair : artisti)
        delete pair.second;
}

void ConsoleApp::run() {
    while (true) {
        showMenu();
        int choice = SafeInput::read<int>("Scegli: ");

        try {
            switch (choice) {
                case 1: insertArtist(); break;
                case 2: modifyArtist(); break;
                case 3: deleteArtist(); break;
                case 4: saveData(); break;
                case 5: loadData(); break;
                case 6: clearAll(); break;
                case 7: printAllArtists(); break;
                case 0: return;
                default: std::cout << "Scelta non valida.\n"; break;
            }
        } catch (const std::exception& ex) {
            ErrorManager::logError(ex.what());
        } catch (...) {
            ErrorManager::logError("Errore sconosciuto.");
        }
    }
}

void ConsoleApp::showMenu() {
    std::cout << "\n=== Menu CLI ===\n";
    std::cout << "1. Inserisci nuovo Artista\n";
    std::cout << "2. Modifica Artista (e suoi Prodotti)\n";
    std::cout << "3. Elimina Artista\n";
    std::cout << "4. Salva su file (JSON o XML)\n";
    std::cout << "5. Carica da file (JSON o XML)\n";
    std::cout << "6. Cancella tutti i dati\n";
    std::cout << "7. Mostra tutti gli artisti e i loro prodotti\n";
    std::cout << "0. Esci\n";
}

void ConsoleApp::printAllArtists() const {
    if (artisti.empty()) {
        std::cout << "Nessun artista disponibile.\n";
        return;
    }

    std::cout << "\nArtisti caricati:\n";
    for (const auto& [id, artista] : artisti) {
        std::cout << "------------------------\n";
        artista->printInfo();
    }
    std::cout << "------------------------\n";
}

void ConsoleApp::insertArtist() {
    try {
        std::string nome = SafeInput::read("Inserisci il nome dell'artista: ");
        if (nome.empty()) {
            std::cout << "Nome non valido. Riprova.\n";
            return;
        }

        for (const auto& [id, artista] : artisti) {
            if (artista->getNome() == nome) {
                std::cout << "Artista con nome '" << nome << "' già esistente.\n";
                return;
            }
        }
        
        Artista* nuovo = ConsoleEditorHandler::creaArtista(nome);
        if (nuovo) {
            artisti[nuovo->getId()] = nuovo;
            std::cout << "Artista inserito con ID: " << nuovo->getId() << "\n";
        }
    } catch (const std::exception& ex) {
        ErrorManager::logError(ex.what());
    }
}

void ConsoleApp::modifyArtist() {
    Artista* a = selectArtist();
    if (!a) return;

    try {
        ConsoleArtistEditor::modificaArtista(artisti, a);
    } catch (const std::exception& ex) {
        ErrorManager::logError(ex.what());
    }
}

void ConsoleApp::deleteArtist() {
    Artista* a = selectArtist();
    if (!a) return;

    artisti.erase(a->getId());
    delete a;
    std::cout << "Artista eliminato.\n";
}

void ConsoleApp::saveData() {
    try {
        int formato = SafeInput::read<int>("Formato di salvataggio (1=JSON, 2=XML): ");

        // Lettura del nome file, può tornare anche stringa vuota
        std::string name = SafeInput::read("Nome file da salvare (senza estensione) [default: cli_save]: ");
        if (name.empty()) {
            name = "cli_save";
        }

        if (formato == 1) {
            std::string path = "saves/json/" + name + ".json";
            DataManager::saveToFileJson(artisti, path);
            std::cout << "Dati salvati su " << path << "\n";
        }
        else if (formato == 2) {
            std::string path = "saves/xml/" + name + ".xml";
            DataManager::saveToFileXml(artisti, path);
            std::cout << "Dati salvati su " << path << "\n";
        }
        else {
            std::cout << "Scelta non valida.\n";
        }
    }
    catch (const std::exception& ex) {
        ErrorManager::logError(ex.what());
    }
}


void ConsoleApp::loadData() {
    try {
        int formato = SafeInput::read<int>("Formato di caricamento (1=JSON, 2=XML): ");

        std::string pathBase = (formato == 1) ? "saves/json/" : "saves/xml/";
        QStringList filtro = (formato == 1) ? QStringList() << "*.json" : QStringList() << "*.xml";

        QDir dir(QString::fromStdString(pathBase));
        QStringList files = dir.entryList(filtro, QDir::Files);

        if (files.isEmpty()) {
            std::cout << "Nessun file trovato in " << pathBase << "\n";
            return;
        }

        std::cout << "File disponibili:\n";
        for (int i = 0; i < files.size(); ++i) {
            std::cout << " " << i + 1 << ". " << files[i].toStdString() << "\n";
        }

        int choice = SafeInput::read<int>("Seleziona file (numero): ");
        if (choice < 1 || choice > files.size()) {
            std::cout << "Scelta non valida.\n";
            return;
        }

        std::string fileChosen = pathBase + files[choice - 1].toStdString();

        for (auto& p : artisti)
            delete p.second;
        artisti.clear();

        if (formato == 1) {
            artisti = DataManager::loadFromFileJson(fileChosen);
        } else if (formato == 2) {
            artisti = DataManager::loadFromFileXml(fileChosen);
        } else {
            std::cout << "Scelta di formato non valida.\n";
            return;
        }

        std::cout << "Dati caricati da " << fileChosen << "\n";
    } catch (const std::exception& ex) {
        std::cout << "Errore durante il caricamento dati: " << ex.what() << "\n";
    } catch (...) {
        std::cout << "Errore sconosciuto durante il caricamento dati.\n";
    }
}

void ConsoleApp::clearAll() {
    for (auto& p : artisti)
        delete p.second;
    artisti.clear();
    std::cout << "Tutti i dati sono stati cancellati.\n";
}

Artista* ConsoleApp::selectArtist() {
    if (artisti.empty()) {
        std::cout << "Nessun artista disponibile.\n";
        return nullptr;
    }

    std::cout << "ID artisti disponibili:\n";
    for (const auto& [id, a] : artisti) {
        std::cout << "ID " << id << ": " << a->getNome() << "\n";
    }

    unsigned int id = SafeInput::read<unsigned int>("Inserisci ID artista: ");
    if (artisti.find(id) != artisti.end())
        return artisti[id];

    std::cout << "ID non trovato.\n";
    return nullptr;
}

Artista* ConsoleApp::getArtistById(unsigned int id) const {
    auto it = artisti.find(id);
    if (it != artisti.end()) {
        return it->second;
    }
    return nullptr;
}
