#ifndef GRAPHATTRIBUTES_H
#define GRAPHATTRIBUTES_H

#include <Types.h>
#include "graphnode.h"

template <class T, class U> class GraphNode;

template <class U>
class GraphAttributes
{
public:
    GraphAttributes() {};

    U attributes;
    template <class T> void update(GraphNode<T,U> *n, const std::vector<Point <TCoord> > &pixels) {
        attributes.update(pixels);
    }

    template <class T> void compute(GraphNode<T,U> *n) {};

};

#endif // GRAPHATTRIBUTES_H
