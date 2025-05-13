#ifndef DISCO_H
#define DISCO_H

#include <string>

#include "Merch.h"
#include "../../include/data_format.h"

class Disco : public Merch {
protected:
    std::string produttoreStampe;
    std::string codiceRiconoscimento;
    std::string tipoProdotto;  // potrebbe essere mantenuto o rimosso in base ai futuri filtri

public:
    // Costruttore standard
    Disco(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& produttoreStampe, const std::string& codiceRiconoscimento, const std::string& tipoProdotto);

    // Costruttore con immagine
    Disco(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& produttoreStampe, const std::string& codiceRiconoscimento, const std::string& tipoProdotto, const std::string& img);

    // Costruttore di trasformazione da Merch*
    Disco(Merch* base, const std::string& produttoreStampe, const std::string& codiceRiconoscimento, const std::string& tipoProdotto);

    // Costruttore di copia
    Disco(const Disco* disco);

    // Costruttore da JSON
    Disco(Artista* owner, const QJsonObject& json);

    // Costruttore da XML
    Disco(Artista* owner, const QDomElement& xml);

    // Distruttore
    virtual ~Disco();

    // Getter/Setter
    std::string getProduttoreStampe() const;
    void setProduttoreStampe(const std::string& produttoreStampe);

    std::string getCodiceRiconoscimento() const;
    void setCodiceRiconoscimento(const std::string& codiceRiconoscimento);

    std::string getTipoProdotto() const;
    void setTipoProdotto(const std::string& tipoProdotto);

    // Funzioni di utility
    virtual void printInfo() const override;
    virtual QJsonObject toJson() const override;
    virtual QDomElement toXml(QDomDocument& doc) const override;

    // Overloading operatori
    friend bool operator==(const Disco& a, const Disco& b);
    friend bool operator!=(const Disco& a, const Disco& b);
};

#endif // DISCO_H