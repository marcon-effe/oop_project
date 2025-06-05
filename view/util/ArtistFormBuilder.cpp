#include "ArtistFormBuilder.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "../../data/DataManager.h"

ArtistFormBuilder::ArtistFormBuilder(std::unordered_map<unsigned, Artista*>& artistsMap,
                                     Artista* existing,
                                     const std::set<std::string>& nomiEsistenti,
                                     QWidget* parentWidget)
    : m_artists(artistsMap),
      m_original(existing),
      m_artist(nullptr),
      m_nomiEsistenti(nomiEsistenti),
      m_widget(nullptr),
      m_mainLayout(nullptr),
      m_formLayout(nullptr),
      m_leNome(nullptr),
      m_leGenere(nullptr),
      m_teInfo(nullptr),
      m_leImagePath(nullptr),
      m_btnSelectImg(nullptr),
      m_buttons(nullptr)
{
    m_widget = new QWidget(parentWidget);

    m_mainLayout = new QVBoxLayout(m_widget);
    m_mainLayout->setContentsMargins(24, 24, 24, 24);
    m_mainLayout->setSpacing(12);

    m_formLayout = new QFormLayout;
    m_mainLayout->addLayout(m_formLayout);

    buildFormFields();

    if (m_original) {
        loadExistingValues();
    }

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, m_widget);
    QAbstractButton* okBtn     = m_buttons->button(QDialogButtonBox::Ok);
    QAbstractButton* cancelBtn = m_buttons->button(QDialogButtonBox::Cancel);
    okBtn->setObjectName("okButton");
    cancelBtn->setObjectName("cancelButton");

    // Disabilito “Ok” finché il nome è vuoto (in insert mode) o altri campi obbligatori mancanti
    okBtn->setEnabled(m_original != nullptr || !m_leNome->text().trimmed().isEmpty());

    connect(m_buttons, &QDialogButtonBox::accepted, this, &ArtistFormBuilder::onSaveValues);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &ArtistFormBuilder::onCancel);

    m_mainLayout->addWidget(m_buttons);

    connect(m_leNome, &QLineEdit::textChanged, this, [this](const QString& txt) {
        m_buttons->button(QDialogButtonBox::Ok)->setEnabled(!txt.trimmed().isEmpty());
    });
}

void ArtistFormBuilder::buildFormFields()
{
    m_leNome = new QLineEdit(m_widget);
    m_leNome->setMaxLength(40);
    m_leNome->setPlaceholderText("Inserisci il nome dell'artista");
    m_formLayout->addRow(tr("Nome:"), m_leNome);

    m_leGenere = new QLineEdit(m_widget);
    m_leGenere->setMaxLength(20);
    m_leGenere->setPlaceholderText("Inserisci il genere musicale");
    m_formLayout->addRow(tr("Genere:"), m_leGenere);

    m_teInfo = new QTextEdit(m_widget);
    m_formLayout->addRow(tr("Info:"), m_teInfo);

    m_leImagePath  = new QLineEdit(m_widget);
    m_leImagePath->setReadOnly(true);
    m_btnSelectImg = new QPushButton(tr("Scegli immagine…"), m_widget);

    auto* imgLayout = new QHBoxLayout;
    imgLayout->setContentsMargins(0, 0, 0, 0);
    imgLayout->addWidget(m_leImagePath);
    imgLayout->addWidget(m_btnSelectImg);
    m_formLayout->addRow(tr("Immagine:"), imgLayout);

    connect(m_btnSelectImg, &QPushButton::clicked, this, &ArtistFormBuilder::onSelectImage);
}

void ArtistFormBuilder::loadExistingValues()
{
    m_leNome->setText(QString::fromStdString(m_original->getNome()));
    m_leGenere->setText(QString::fromStdString(m_original->getGenere()));
    m_teInfo->setPlainText(QString::fromStdString(m_original->getInfo()));
    m_leImagePath->setText(QString::fromStdString(m_original->getImagePath()));
}

void ArtistFormBuilder::onSelectImage()
{
    QString file = QFileDialog::getOpenFileName(m_widget,
                                                tr("Seleziona immagine"),
                                                {},
                                                tr("Immagini (*.png *.jpg *.jpeg *.bmp)"));
    if (!file.isEmpty()) {
        m_leImagePath->setText(file);
    }
}

void ArtistFormBuilder::onSaveValues()
{
    const QString qNome   = m_leNome->text().trimmed();
    const QString qGenere = m_leGenere->text().trimmed();
    const QString infoTxt = m_teInfo->toPlainText().trimmed();
    const QString qImg    = m_leImagePath->text().trimmed();

    // Controllo obbligatorietà del nome
    if (qNome.isEmpty()) {
        QMessageBox::warning(m_widget,
                             tr("Errore"),
                             tr("Il nome dell'artista è obbligatorio."));
        return;
    }

    // Controllo unicità: tolgo il nome corrente dal set se in edit mode
    {
        std::set<std::string> tempNomi = m_nomiEsistenti;
        if (m_original) {
            // Sanitizzo il nome in modo da cercare il nome corretto
            std::string originaleSanitizzato = DataManager::sanitizeForPath(m_original->getNome());
            tempNomi.erase(originaleSanitizzato);
        }
        if (tempNomi.count(DataManager::sanitizeForPath(qNome.toStdString()))) {
            QMessageBox::warning(m_widget,
                                 tr("Nome già esistente"),
                                 tr("Esiste già un'artista con questo nome."));
            return;
        }
    }

    try {
        if (m_original) {
            // Modifica in-place
            m_original->setNome(qNome.toStdString());
            m_original->setGenere(qGenere.toStdString());
            m_original->setInfo(infoTxt.toStdString());
            if (!qImg.isEmpty()) {
                m_original->setImagePath(qImg.toStdString());
            }
            m_artist = m_original;
        }
        else {
            // Creazione di un nuovo Artista
            Artista* nuovo = new Artista(qNome.toStdString(),
                                         qGenere.toStdString(),
                                         infoTxt.toStdString());
            if (!qImg.isEmpty()) {
                nuovo->setImagePath(qImg.toStdString());
            }
            m_artist = nuovo;
            // Inserisco nella mappa <id, Artista*>
            unsigned newId = m_artist->getId();
            m_artists[newId] = m_artist;
        }
    }
    catch (const std::exception& ex) {
        QMessageBox::critical(m_widget,
                              tr("Errore di validazione"),
                              QString::fromUtf8(ex.what()));
        return;
    }

    emit editingAccepted(m_artist);
}

void ArtistFormBuilder::onCancel()
{
    emit editingCanceled();
}
