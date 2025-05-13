#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>
#include <unordered_map>
#include <string>

class Artista;
class ArtistProduct;

class FilterDialog : public QDialog {
    Q_OBJECT

public:
    explicit FilterDialog(QWidget* parent = nullptr);

    void setupForArtisti(const std::unordered_map<unsigned int, Artista*>& artists);
    void setupForProdotti(const std::unordered_map<unsigned int, ArtistProduct*>& prodotti, const std::unordered_map<unsigned int, Artista*>& artisti);

    QString getSelectedGenere() const;

    QString getSelectedGenereMusica() const;
    std::vector<std::string> getSelectedTipoProdotto() const;
    bool isDisponibileChecked() const;
    QString getSelectedArtistaId() const;

private:
    QVBoxLayout* mainLayout = nullptr;

    // Artista
    QComboBox* genereCombo = nullptr;

    // Prodotti
    QComboBox* artistaCombo = nullptr;
    QComboBox* genereMusicaCombo = nullptr;
    QCheckBox* disponibileCheck = nullptr;

    std::unordered_map<QString, std::string> artistaNomeToId;

    QCheckBox* albumCheck = nullptr;
    QCheckBox* singoloCheck = nullptr;
    QCheckBox* cdCheck = nullptr;
    QCheckBox* vinileCheck = nullptr;
    QCheckBox* tshirtCheck = nullptr;
    QCheckBox* tourCheck = nullptr;
};

#endif // FILTERDIALOG_H