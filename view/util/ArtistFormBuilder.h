#ifndef ARTISTAFORMBUILDER_H
#define ARTISTAFORMBUILDER_H

#include <QObject>
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <unordered_map>
#include <set>

#include "../../model/artisti/Artista.h"

class ArtistFormBuilder : public QObject {
    Q_OBJECT

public:
    explicit ArtistFormBuilder(std::unordered_map<unsigned, Artista*>& artistsMap,
                               Artista* existing,
                               const std::set<std::string>& nomiEsistenti,
                               QWidget* parentWidget = nullptr);

    ~ArtistFormBuilder() override = default;

    QWidget* getWidget() const { return m_widget; }

signals:
    void editingAccepted(Artista* result);
    void editingCanceled();

private slots:
    void onSelectImage();
    void onSaveValues();
    void onCancel();

private:
    void buildFormFields();
    void loadExistingValues();

    std::unordered_map<unsigned, Artista*>& m_artists;
    Artista*       m_original;
    Artista*       m_artist;
    std::set<std::string> m_nomiEsistenti;

    QWidget*       m_widget;
    QVBoxLayout*   m_mainLayout;

    QFormLayout*   m_formLayout;
    QLineEdit*     m_leNome;
    QLineEdit*     m_leGenere;
    QTextEdit*     m_teInfo;
    QLineEdit*     m_leImagePath;
    QPushButton*   m_btnSelectImg;

    QDialogButtonBox* m_buttons;
};

#endif // ARTISTAFORMBUILDER_H