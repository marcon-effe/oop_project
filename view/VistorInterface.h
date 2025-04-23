#ifndef VISITORINTERFACE_H
#define VISITORINTERFACE_H

// Forward definition 
class Album;
class Singolo;
class TShirt;
class CD;
class Vinile;
class Tour;
class Artista;

class VisitorInterface {
    public:
        virtual ~VisitorInterface() = default;
        virtual void visit(const Artista* artista) =0;
        virtual void visit(const Album* album) =0;
        virtual void visit(const Singolo* singolo) =0;
        virtual void visit(const TShirt* tshirt) =0;
        virtual void visit(const CD* cd) =0;
        virtual void visit(const Vinile* vinile) =0;
        virtual void visit(const Tour* tour) =0;
};

#endif