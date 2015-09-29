#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <vector>
#include "Image.h"
#include "graphattributes.h"

using namespace LibTIM;

template <class U> class GraphAttributes;

template <class T, class U> class GraphNode
{
public:
    int id; /// unique identifier
    T value; /// original node value
    T modifiedValue; /// modified node value

    bool active; /// true if the node contributes to image reconstruction

    std::vector<GraphNode *> childs; /// list of direct descendants
    std::vector<GraphNode *> fathers; /// list of direct ascendants
    std::vector<Point<TCoord> > pixels; /// list of pixels offsets belonging to the node
    GraphAttributes<U> attributes;
    void updateAttributes(const std::vector<Point<TCoord> > &pixels, const T &value) {
        // insert pixels in node
        this->pixels.insert(this->pixels.end(), pixels.begin(), pixels.end());
        // update node attributes
        attributes.update(this, pixels,value);
    }

    GraphNode(int id, T value) {
        this->id=id;
        this->value=value;
        this->modifiedValue=value;
        this->active=true;
        this->attributes.setValue(value);
    }
    void addChild(GraphNode *child) {
        this->childs.push_back(child);
        child->fathers.push_back(this);
    }
};

#endif // GRAPHNODE_H
