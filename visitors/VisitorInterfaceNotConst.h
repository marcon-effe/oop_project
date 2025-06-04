#ifndef VISITORINTERFACENOTCONST_H
#define VISITORINTERFACENOTCONST_H

// Forward definition 
class Album;
class Singolo;
class TShirt;
class CD;
class Vinile;
class Tour;
class Artista;
class DataTour; // Aggiunto per DataTour
class Traccia; // Aggiunto per Traccia

class VisitorInterfaceNotConst {
    public:
        virtual ~VisitorInterfaceNotConst() = default;
        virtual void visit(Album* album) =0;
        virtual void visit(Singolo* singolo) =0;
        virtual void visit(TShirt* tshirt) =0;
        virtual void visit(CD* cd) =0;
        virtual void visit(Vinile* vinile) =0;
        virtual void visit(Tour* tour) =0;
        virtual void visit(DataTour* dataTour) = 0; // Aggiunto per DataTour
        virtual void visit(Traccia* traccia) = 0; // Aggiunto per Traccia
};

#endif