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
    template <class T> void update(GraphNode<T,U> *n, const std::vector<Point <TCoord> > &pixels, const T &value) {
        attributes.update(pixels, value);
    }

    template <class T> void compute(GraphNode<T,U> *n) {};

    template <class T> void setValue(T &value) {
        attributes.setValue(value);
    }

    std::string writeDot() {
        return attributes.writeDot();
    };
};

#endif // GRAPHATTRIBUTES_H
