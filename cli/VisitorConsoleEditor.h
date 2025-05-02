#ifndef VISITORCONSOLEEDITOR_H
#define VISITORCONSOLEEDITOR_H

class Artista;
class Album;
class Singolo;
class CD;
class Vinile;
class TShirt;
class Tour;
class Traccia;
class DataTour;

class VisitorConsoleEditor{
    public:
        VisitorConsoleEditor() = default;
        ~VisitorConsoleEditor() = default;

        void visit(Artista* artista);

        void visit(Album* album);
        void visit(Singolo* singolo);
        void visit(Traccia* traccia);

        void visit(CD* cd);
        void visit(Vinile* vinile);
        void visit(TShirt* tshirt);

        void visit(Tour* tour);
        void visit(DataTour* dataTour);
        
};


#endif // VISITORCONSOLEEDITOR_H