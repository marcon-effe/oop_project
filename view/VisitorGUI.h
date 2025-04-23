#ifndef VISITORGUI_H
#define VISITORGUI_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

#include "./VistorInterface.h"

#include "../model/core/ArtistProduct.h"
#include "../model/artisti/Artista.h"
#include "../model/core/Musica.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/merch/TShirt.h"
#include "../model/tour/Tour.h"

class VisitorGUI : public QObject, public VisitorInterface {
    Q_OBJECT        // uso di connect
public:
    explicit VisitorGUI(QObject* parent = nullptr);
    QWidget* getWidget() const;

    // Visitor methods
    void visit(const Artista* artista) override;
    void visit(const Album* album) override;
    void visit(const Singolo* singolo) override;
    void visit(const CD* cd) override;
    void visit(const Vinile* vinile) override;
    void visit(const TShirt* tshirt) override;
    void visit(const Tour* tour) override;

private:
    void clearLayout();
    QLabel* createImageLabel(const std::string& imagePathStr, bool isArtist) const;

    QWidget* widget;
    QVBoxLayout* layout;
};

#endif // VISITORGUI_H