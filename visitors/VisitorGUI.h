#ifndef VISITORGUI_H
#define VISITORGUI_H

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <unordered_map>

#include "../model/core/ArtistProduct.h"
#include "../model/core/Musica.h"
#include "../model/musica/Album.h"
#include "../model/musica/Singolo.h"
#include "../model/merch/CD.h"
#include "../model/merch/Vinile.h"
#include "../model/merch/TShirt.h"
#include "../model/tour/Tour.h"
#include "../view/util/ClickableLabel.h"
#include "../view/util/ImageHelper.h"  
#include "VisitorInterfaceConst.h"
#include "../model/artisti/Artista.h"

class VisitorGUI : public QObject, public VisitorInterfaceConst {
    Q_OBJECT
public:
    explicit VisitorGUI(const std::unordered_map<unsigned int, Artista*>* artistsMap, QObject* parent = nullptr);
    QWidget* getWidget() const;

    void visit(const Album* album) override;
    void visit(const Singolo* singolo) override;
    void visit(const CD* cd) override;
    void visit(const Vinile* vinile) override;
    void visit(const TShirt* tshirt) override;
    void visit(const Tour* tour) override;

    void setArtistMap(const std::unordered_map<unsigned int, Artista*>& artistsMap);

private:
    void clearLayout();
    QWidget* createRelatedProductsSection(unsigned int artistId, unsigned int excludeProductId);
    QWidget* createTrackWidget(const Traccia& track);
    QWidget* createDateTourWidget(const DataTour& dt);

    QScrollArea*   scrollArea;
    QWidget*       scrollContent;
    QVBoxLayout*   layout;
    QWidget*       widget;

    const std::unordered_map<unsigned int, Artista*>* m_artists;
};

#endif // VISITORGUI_H
