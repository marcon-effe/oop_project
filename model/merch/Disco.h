#ifndef DISCO_H
#define DISCO_H

#include "Merch.h"


class Disco : public Merch {
protected:
    std::string produttoreStampe;
    std::string codiceRiconoscimento;
    std::string tipoProdotto;  // potrebbe essere mantenuto o rimosso in base ai futuri filtri

public:
    Disco(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& produttoreStampe, const std::string& codiceRiconoscimento, const std::string& tipoProdotto);

    Disco(Artista* owner, const std::string& t, const std::string& desc, double prezzo, bool disponibile, unsigned int quantita, const std::string& codice, const std::string& produttoreStampe, const std::string& codiceRiconoscimento, const std::string& tipoProdotto, const std::string& img);

    Disco(Merch* base, const std::string& produttoreStampe, const std::string& codiceRiconoscimento, const std::string& tipoProdotto);

    Disco(const Disco* disco);

    Disco(Artista* owner, const QJsonObject& json);

    Disco(Artista* owner, const QDomElement& xml);

    virtual ~Disco();

    std::string getProduttoreStampe() const;
    void setProduttoreStampe(const std::string& produttoreStampe);

    std::string getCodiceRiconoscimento() const;
    void setCodiceRiconoscimento(const std::string& codiceRiconoscimento);

    std::string getTipoProdotto() const;
    void setTipoProdotto(const std::string& tipoProdotto);

    // Funzioni di utility
    virtual void printInfo() const override;
    virtual QJsonObject toJson(bool reduced=false) const override;
    virtual QDomElement toXml(QDomDocument& doc, bool reduced=false) const override;

    friend bool operator==(const Disco& a, const Disco& b);
    friend bool operator!=(const Disco& a, const Disco& b);
};

#endif // DISCO_H