# OOP Project

Questa repository è stata realizzata per lo sviluppo del progetto di Programmazione a Oggetti.

## Descrizione

Applicazione Qt per la gestione di **artisti** e dei loro **prodotti multimediali**, come:

- Album
- Singoli
- CD
- Vinili
- T-shirt
- Tour

Ogni prodotto è collegato a un artista e viene salvato in formato JSON o XML. I dati includono descrizioni, tracce, disponibilità, immagini locali o codificate in Base64.

Funzionalità principali:
- Visualizzazione con Visitor pattern
- Inserimento/modifica/eliminazione oggetti tramite dialog
- Filtri per tipo, genere, artista, disponibilità
- Salvataggio automatico opzionale
- Import/export JSON e XML
- Esportazione ridotta
- Validazione XML tramite XSD

## Requisiti

- Qt 6.5+ (`core`, `gui`, `widgets`, `xml`)
- `xmllint` con supporto `--schema` per validazione XML

Linux:
```bash
sudo apt install libxml2-utils
```

macOS:
```bash
brew install libxml2
```

Windows:
- Qt 6.5+ installato (incluso `qmake`)
- Git Bash o altro terminale Bash se si vuole usare lo script `installer.sh`
- In alternativa, usare il **Qt Command Prompt** per compilare manualmente (vedi sezione successiva).

## Problemi con l’installer su Windows

Lo script `installer.sh` è uno script Bash e **non** è eseguibile nativamente in PowerShell o nel prompt CMD.  
Per compilare l’applicazione su Windows, è possibile:

1. **Usare Git Bash**  
   - Aprire Git Bash nella cartella del progetto.  
   - Eseguire lo script come su Linux/macOS:
     ```bash
     ./installer.sh [path_qmake]
     ```
   - Se `path_qmake` è omesso, lo script cercherà `qmake` nel `PATH` di Git Bash.

2. **Compilazione manuale nel Qt Command Prompt**  
   - Aprire il **Qt Command Prompt** fornito da Qt (o un terminale che ha `qmake` nel `PATH`).  
   - Generare i makefile:
     ```cmd
     qmake oop_project.pro -r
     ```
   - Compilare:
     ```cmd
     nmake       # oppure jom se installato
     ```
   - Avviare l’eseguibile:
     ```cmd
     .\release\oop_project.exe
     ```

## Avvio rapido

Clona il repository:

```bash
git clone <url-repository>
cd oop_project
```

### Compilazione

Esegui lo script `installer.sh` (solo su Linux/macOS o su Git Bash):

```bash
./installer.sh [path_qmake]
```

- `path_qmake` è opzionale. Se omesso, verrà cercato in `$PATH`.

Lo script:
1. Verifica `xmllint` e supporto `--schema`
2. Pulisce e rigenera `.pro`, `Makefile`, `build/`
3. Compila il progetto
4. Esegue l'applicazione

## Avvio dell'applicazione

All'avvio del binario `./oop_project` (o `oop_project.exe` su Windows), viene mostrato un menu testuale con tre opzioni:

1. **Test import/export**  
   Esegue salvataggio e caricamento di file JSON e XML per verificarne la correttezza.

2. **Modalità CLI**  
   Interfaccia testuale per operazioni su artisti e prodotti.

3. **GUI Qt**  
   Avvia l'interfaccia grafica principale per gestione completa via menù, form, e visualizzazione dettagliata.

## Gerarchia sintetica delle classi

```
Artista
├── contiene una mappa di:
│   └── ArtistProduct (abstract)
│       ├── Musica (abstract)
│       │   ├── Album
│       │   │   └── std::vector<Traccia>
│       │   └── Singolo
│       │       └── Traccia
│       └── NotMusica (abstract)
│           ├── Merch (abstract)
│           │   ├── TShirt
│           │   └── Disco (abstract)
│           │       ├── CD
│           │       └── Vinile
│           └── Tour
│               └── std::vector<DataTour>
│
├── Altre classi di supporto:
    ├── Traccia
    │   ├── std::string nome
    │   └── std::vector<std::string> partecipanti
    ├── DataTour
    │   ├── Data data
    │   ├── Orario orario
    │   └── std::string luogo
    ├── Data
    │   └── giorno, mese, anno
    ├── Orario
    │   └── ore, minuti, secondi
    └── Durata
        └── ore, minuti, secondi
```