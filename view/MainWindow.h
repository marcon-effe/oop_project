#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <unordered_map> 

#include "../model/artisti/Artista.h"
#include "../model/core/ArtistProduct.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void resizeEvent(QResizeEvent* event) override;
    void setupUI();
    void loadDataFromSaves(const std::string& path = "saves/json/test_artisti.json");

    QVBoxLayout* rightLayout;
    void clearRightPanel();
    void clearAll();
    void updateListWidgets();
    void sortArtistListWidget();
    void sortProductListWidget();
    void saveIfAutosaveEnabled();

    QAction* nuovoAction;
    QAction* autosaveAction;
    QAction* inserisciArtista;
    QAction* inserisciProdotto;
    QAction* modificaArtista;
    QAction* modificaProdotto;
    QAction* eliminaArtista;
    QAction* eliminaProdotto;

    // Widget principali
    QListWidget *artistListWidget;
    QListWidget *productListFullWidget;
    QLabel *nameLabel;
    QLabel *descriptionLabel;
    QLabel *savedAlbumsLabel;
    QLabel *artistImage;

    // Layout dinamici scrollabili
    QHBoxLayout *prodottiArtistaLayout;
    QHBoxLayout *releaseArtistaLayout;

    // Dati
    std::unordered_map<unsigned int, Artista*> artists;
    std::unordered_map<unsigned int, ArtistProduct*> prodotti;

private slots:
    void onNuovoProgetto();
    void onInserisciArtista();
    void onInserisciProdotto();
    void onModificaArtista();
    void onModificaProdotto();
    void onEliminaArtista();
    void onEliminaProdotto();

    void handleArtistSelection(QListWidgetItem* item);
    void handleProductSelection(QListWidgetItem* item);
    
    void openArtistFilterDialog();
    void applyArtistGenreFilter(const QString& genere);
    
    void openProductFilterDialog();
    void applyProductFilters(const std::vector<std::string>& tipi, const QString& genereMusica, bool disponibile, const QString& artistaId);

public slots:
    void importData();
    void exportData();
    
    void filterArtistList(const QString& query);
    void filterProductList(const QString& query);

};

#endif // MAINWINDOW_H