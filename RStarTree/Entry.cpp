#include "Entry.h"
#include "Node.h"

// Constructor para hojas: guarda el punto y su MBB
Entry::Entry(const Point& p) : box(p), child(nullptr), data(p) {}

// Constructor para nodos internos: guarda el hijo y su MBB
Entry::Entry(Node* c, const Mbb& b) : box(b), child(c), data() {}
