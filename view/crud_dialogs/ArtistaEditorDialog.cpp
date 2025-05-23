#include "ArtistaEditorDialog.h"
#include "../../data/DataManager.h"

ArtistaEditorDialog::ArtistaEditorDialog(
    Artista* existing,
    QWidget* parent,
    const std::set<std::string>& nomiEsistenti
) : QDialog(parent),
    m_original(existing),
    m_artist(nullptr),
    m_nomiEsistenti(nomiEsistenti)
{
    this->setObjectName("artistaEditorDialog");

    // tolgo il nome corrente dal set in modifica
    if (m_original) {
        m_nomiEsistenti.erase(m_original->getNome());
    }

    setWindowTitle(existing ? tr("Modifica Artista") : tr("Nuovo Artista"));

    auto* mainLayout = new QVBoxLayout(this);
    m_formLayout = new QFormLayout;
    mainLayout->addLayout(m_formLayout);

    buildFields();
    if (m_original) loadExisting();

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QAbstractButton* okBtn     = m_buttons->button(QDialogButtonBox::Ok);
    QAbstractButton* cancelBtn = m_buttons->button(QDialogButtonBox::Cancel);
    okBtn->setObjectName("okButton");
    cancelBtn->setObjectName("cancelButton");

    // disabilito Ok se nessun nome
    okBtn->setEnabled(m_original || !m_leNome->text().trimmed().isEmpty());

    connect(m_buttons, &QDialogButtonBox::accepted, this, &ArtistaEditorDialog::saveValues);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(m_buttons);

    // aggiorno lo stato di Ok al cambiare del testo
    connect(m_leNome, &QLineEdit::textChanged, this, [this](const QString& text) {
        m_buttons->button(QDialogButtonBox::Ok)
                 ->setEnabled(!text.trimmed().isEmpty());
    });
}

/* Inutile per il modo in cui è stato implementato, perché ogni dialog viene ricostruito da 0 */
/* void ArtistaEditorDialog::clearFields()
{
    while (m_formLayout->count() > 0) { 
        QLayoutItem* item = m_formLayout->takeAt(0);
        if (auto* w = item->widget()) 
            delete w; 
        delete item;
    }
} */

void ArtistaEditorDialog::buildFields()
{
    /* clearFields(); */

    m_leNome = new QLineEdit(this);
    m_leNome->setMaxLength(40);
    m_leNome->setPlaceholderText("Inserisci il nome dell'artista");
    m_formLayout->addRow(tr("Nome:"), m_leNome);

    m_leGenere = new QLineEdit(this);
    m_leGenere->setMaxLength(20);
    m_leGenere->setPlaceholderText("Inserisci il genere musicale");
    m_formLayout->addRow(tr("Genere:"), m_leGenere);

    m_teInfo        = new QTextEdit(this);
    m_formLayout->addRow(tr("Info:"), m_teInfo);

    m_leImagePath   = new QLineEdit(this);
    m_leImagePath->setReadOnly(true);
    m_btnSelectImg  = new QPushButton(tr("Scegli immagine…"), this);

    auto* imgLayout = new QHBoxLayout;
    imgLayout->setContentsMargins(0,0,0,0);
    imgLayout->addWidget(m_leImagePath);
    imgLayout->addWidget(m_btnSelectImg);

    m_formLayout->addRow(tr("Immagine:"), imgLayout);
    connect(m_btnSelectImg, &QPushButton::clicked, this, &ArtistaEditorDialog::selectImage);
}

void ArtistaEditorDialog::loadExisting()
{
    m_leNome->setText(QString::fromStdString(m_original->getNome()));
    m_leGenere->setText(QString::fromStdString(m_original->getGenere()));
    m_teInfo->setPlainText(QString::fromStdString(m_original->getInfo()));
    m_leImagePath->setText(QString::fromStdString(m_original->getImagePath()));
}

void ArtistaEditorDialog::selectImage()
{
    QString file = QFileDialog::getOpenFileName( this, tr("Seleziona immagine"), {}, tr("Immagini (*.png *.jpg *.jpeg *.bmp)"));

    if (!file.isEmpty())
        m_leImagePath->setText(file);
}

void ArtistaEditorDialog::saveValues()
{
    const QString qNome = m_leNome->text().trimmed();
    const QString qGenere = m_leGenere->text().trimmed();
    const QString infoText = m_teInfo->toPlainText().trimmed();
    const QString qImgPath = m_leImagePath->text().trimmed();

    if (qNome.isEmpty()) {
        QMessageBox::warning(this, tr("Errore"), tr("Il nome dell'artista è obbligatorio."));
        return;
    }

    if (m_nomiEsistenti.count(DataManager::sanitizeForPath(qNome.toStdString()))) {
        QMessageBox::warning(this, tr("Nome già esistente"),
                            tr("Esiste già un artista con questo nome."));
        return;
    }

    try {
        if (m_original) {
            // modifica esistente
            m_original->setNome(qNome.toStdString());
            m_original->setGenere(qGenere.toStdString());
            m_original->setInfo(infoText.toStdString());

            if (!qImgPath.isEmpty())
                m_original->setImagePath(qImgPath.toStdString());

            m_artist = m_original;
        } else {
            m_artist = new Artista(
                qNome.toStdString(),
                qGenere.toStdString(),
                infoText.toStdString()
            );

            if (!qImgPath.isEmpty())
                m_artist->setImagePath(qImgPath.toStdString());
        }
    }
    catch (const std::exception &ex) {
        QMessageBox::critical(this,
            tr("Errore di validazione"),
            QString::fromUtf8(ex.what()));
        return;
    }
}