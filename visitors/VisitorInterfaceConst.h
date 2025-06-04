#ifndef VISITORINTERFACECONST_H
#define VISITORINTERFACECONST_H

class Artista;
class Album;
class Singolo;
class TShirt;
class CD;
class Vinile;
class Tour;

class VisitorInterfaceConst {
public:
    virtual ~VisitorInterfaceConst() = default;

    virtual void visit(const Album* album) = 0;
    virtual void visit(const Singolo* singolo) = 0;
    virtual void visit(const CD* cd) = 0;
    virtual void visit(const Vinile* vinile) = 0;
    virtual void visit(const TShirt* tshirt) = 0;
    virtual void visit(const Tour* tour) = 0;
};

#endif
