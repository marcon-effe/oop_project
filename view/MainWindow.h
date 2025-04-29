#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <unordered_map>  // aggiunto per usare le hash map

#include "../model/artisti/Artista.h"
#include "../model/core/ArtistProduct.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private:
    void resizeEvent(QResizeEvent* event) override;
    void setupUI();
    void loadDataFromSaves(const std::string& path = "saves/json/test_artisti.json");

    QVBoxLayout* rightLayout;
    void clearRightPanel();

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
    std::unordered_map<unsigned int, Artista*> artists;             // modificato da QList
    std::unordered_map<unsigned int, ArtistProduct*> prodotti;      // modificato da QList

private slots:
    void handleArtistSelection(QListWidgetItem* item);
    void handleProductSelection(QListWidgetItem* item);
};

#endif // MAINWINDOW_H