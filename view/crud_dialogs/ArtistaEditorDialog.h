#ifndef ARTISTA_EDITOR_DIALOG_H
#define ARTISTA_EDITOR_DIALOG_H

#include <set>

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include "../../model/artisti/Artista.h"

class ArtistaEditorDialog : public QDialog {
    Q_OBJECT
public:
    explicit ArtistaEditorDialog(Artista* existing, QWidget* parent = nullptr, const std::set<std::string>& nomiEsistenti = {});
    Artista* artist() const { return m_artist; }

private slots:
    void selectImage();
    void saveValues();

private:
    void buildFields();
    void loadExisting();
    void clearFields();

    // modello
    Artista* m_original;  // se nullptr → creazione
    Artista* m_artist;    // risultato
    QDialogButtonBox* m_buttons;
    std::set<std::string> m_nomiEsistenti;

    // UI
    QFormLayout* m_formLayout;
    QLineEdit*   m_leNome;
    QLineEdit*   m_leGenere;
    QTextEdit*   m_teInfo;
    QLineEdit*   m_leImagePath;
    QPushButton* m_btnSelectImg;
};

#endif
