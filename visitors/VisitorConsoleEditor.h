#ifndef VISITORCONSOLEEDITOR_H
#define VISITORCONSOLEEDITOR_H

#include "VisitorInterfaceNotConst.h"

class Traccia;
class DataTour;

class VisitorConsoleEditor : public VisitorInterfaceNotConst {
    public:
        VisitorConsoleEditor() = default;
        ~VisitorConsoleEditor() = default;

        void visit(Artista* artista) override;
        void visit(Album* album) override;
        void visit(Singolo* singolo) override;
        void visit(Traccia* traccia) override;
        void visit(CD* cd) override;
        void visit(Vinile* vinile) override;
        void visit(TShirt* tshirt) override;
        void visit(Tour* tour) override;
        void visit(DataTour* dataTour) override;
};
#endif