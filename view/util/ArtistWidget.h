#ifndef ARTISTWIDGET_H
#define ARTISTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QListWidget>
#include <unordered_map>

#include "ImageHelper.h"

class Artista;
class ArtistProduct;
class Traccia;
class DataTour;

class ArtistWidget : public QObject
{
    Q_OBJECT

public:
    explicit ArtistWidget(const std::unordered_map<unsigned int, Artista*>* artistsMap,
                          QObject* parent = nullptr);
    ~ArtistWidget() override = default;

    QWidget* getWidget() const;

    void showArtista(const Artista* artista, QWidget* parent);

signals:
    void prodottoSelezionato(ArtistProduct* prodotto);

private:
    const std::unordered_map<unsigned int, Artista*>* m_artists;

    QWidget*       scrollContent;
    QVBoxLayout*   layout;
    QScrollArea*   scrollArea;
    QWidget*       widget; 

    void clearLayout();
};

#endif // ARTISTWIDGET_H
