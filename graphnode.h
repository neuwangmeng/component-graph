#ifndef GRAPHNODE_H
#define GRAPHNODE_H

template <class T> class GraphNode
{
private:
    int id; /// unique identifier
    T value; /// original node value
    T modifiedValue; /// modified node value

    bool active; /// true if the node contributes to image reconstruction

    std::vector<GraphNode *> childs; /// list of direct descendants
    std::vector<Node *> fathers; /// list of direct ascendants
    std::vector<int> pixels; /// list of pixels offsets belonging to the node


    // list of flat-zones belonging to node and having same value
    // std::vector<int > regions;

    //        int area;
    //        int contrast;
    //        bool active;

public:
    GraphNode(int id, GraphValue value, GraphAttributes attributes) {
        this->id=id;
        this->value=value;
        this->attributes=attributes;
        active=true;
    }
    void addChild(GraphNode *child) {
        this->childs.push_back(child);
        child->fathers.push_back(this);
    }
};

#endif // GRAPHNODE_H
