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

## Problemi con l’installer su Windows

Oltre allo script Bash (`installer.sh`), è incluso anche uno script PowerShell (`installer.ps1`) per l’installazione su Windows. Tuttavia, **l’installer per Windows potrebbe fallire la compilazione** a causa di un **bug noto nel compilatore `g++` 11.2.0** incluso in alcune distribuzioni di Qt. Il bug si manifesta come errore interno (`internal compiler error`) durante la compilazione di `qfloat16.h`, un header usato internamente da Qt.

Lo script `installer.ps1` è comunque corretto e completo: verifica `xmllint`, genera correttamente il file `.pro`, compila e avvia l’applicazione. Su ambienti Windows privi di questo bug, lo script funziona correttamente.

### Soluzioni alternative

1. **Compilare con `installer.sh`** su Linux/macOS (o WSL)
2. **Compilare manualmente in ambiente Qt Command Prompt**
3. **Evitare temporaneamente il tipo `qfloat16`** disabilitando le macro, se si modifica il progetto

### Compilazione manuale su Windows

1. Aprire il **Qt Command Prompt** (incluso con l’installazione di Qt)
2. Generare i Makefile:
   ```cmd
   qmake oop_project.pro -r
   ```
3. Compilare:
   ```cmd
   mingw32-make
   ```
4. Avviare:
   ```cmd
   .\oop_project.exe
   ```

## Avvio rapido

Clona il repository:

```bash
git clone <url-repository>
cd oop_project
```

### Compilazione con Bash (Linux/macOS o Git Bash)

Esegui lo script `installer.sh`:

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
