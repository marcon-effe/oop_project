#ifndef DISCO_H
#define DISCO_H

#include <string>

#include "Merch.h"
#include "../../include/data_format.h"


class Disco : public Merch {
protected:
    std::string produttoreStampe;
    std::string codiceRiconoscimento;
    std::string tipoProdotto;       // DA VEDERE SE MANTENERE
public:
    Disco(const std::string &t, const std::string &desc, double prezzo, bool disponibile, unsigned int quantita, const std::string &codice, const std::string &produttoreStampe, const std::string &codiceRiconoscimento, const std::string &tipoProdotto);
    Disco(Merch* m, std::string ps, std::string cr, std::string tp);
    Disco(const Disco* d);
    Disco(const QJsonObject& json);
    Disco(const QDomElement& xml);
    virtual ~Disco();

    std::string getProduttoreStampe() const;
    void setProduttoreStampe(const std::string &ps);

    std::string getCodiceRiconoscimento() const;
    void setCodiceRiconoscimento(const std::string &cr);

    std::string getTipoProdotto() const;
    void setTipoProdotto(const std::string &tp);

    virtual void printInfo() const override;
    QJsonObject toJson() const override;
    QDomElement toXml(QDomDocument& doc) const override;

    // OVERLOADING OPERATORI
    friend bool operator==(const Disco& a, const Disco& b);
    friend bool operator!=(const Disco& a, const Disco& b);
};

#endif // DISCO_H
