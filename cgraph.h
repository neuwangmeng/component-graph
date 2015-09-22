//Copyright (C) 2013, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#ifndef CGRAPH_H
#define CGRAPH_H

#include <vector>
#include <map>

#include "Image.h"
#include "FlatSE.h"
#include "ragraph.h"
#include "graphnode.h"
#include "cgraphwatcher.h"
#include "colorordering.h"

using namespace LibTIM;
using namespace std;


enum class GraphAttributes {
    Area,
    Contrast,
    Volume,
    Compacity
};

/** Component-graph storage and computation **/

template <class T> class CGraph
{
private:
    RAGraph *rag; /*!< Region adjacency graph on which is computed the component-graph */

    Image<RGB> imSource; /*! Source image (2D or 3D) */
    FlatSE connexity;   /*!< Underlying connexity (2D: 4- 8-  or 3D : 6- 18- 26-) */

    /** Container of the nodes of the (directed) graph **/
    std::vector<GraphNode<T> *> graph;
    /** Attributes computed in the graph **/
    std::map<GraphAttributes,bool> usedAttributes;
    /** Attributes of the nodes **/
    std::map<GraphAttributes,std::vector<int> > attributes;
    /** Root of the graph **/
    GraphNode *root;

public:

    CGraph(Image <RGB> &imSource, FlatSE &connexity, std::vector<GraphAttributes> listOfAttributes) {
        this->imSource=imSource;
        this->connexity=connexity;
        this->rag=new RAGraph(imSource,connexity);

        for(int i=0; i<listOfAttributes.size(); i++) {
            std::vector<int> v;
            this->attributes[listOfAttributes[i]]=v;
        }
    }

    CGraph(RAGraph *rag) : rag(rag) {}
    ~CGraph() { delete rag;}

    Node *componentGraphNaive(FlatSE &connexity);

    /** Component-graph \ddot G **/
    int computeGraph(ColorOrdering *order, CGraphWatcher *watcher);
    /** Component-graph G **/
    int computeGraphFull(ColorOrdering *order, CGraphWatcher *watcher);

    /** Return synthetic images */
    static Image<RGB> syntheticImage();
    static Image<RGB> syntheticImage2();

    /** Write graph in dot format **/
    int writeDot(const char *filename);

    void areaFiltering(int areaMin);
    void reconstructMin();

    Image<RGB> constructImage(ColorOrdering *order);

    /** Helper functions **/
    vector<GraphNode<T> *> computeComponents(ColorOrdering *order, OrderedQueue<RGB> &pq);
    static bool notComparable(Image<RGB> &im, Point<TCoord> &p, Point<TCoord> &q);

    bool isEqual(GraphNode<T> *n, GraphNode<T> *m);
    bool isIncluded(GraphNode<T> *n, GraphNode<T> *m);
    bool isIncludedFast(GraphNode<T> *n, GraphNode<T> *m, vector<bool> &tmp);
    void computeLinks(ColorOrdering *order, vector<Node *> &nodes);
    GraphNode<T> *addRoot(vector<GraphNode<T> *> &nodes);
    vector<GraphNode<T> *> minimalElements(ColorOrdering *order, vector<GraphNode<T> *> &nodes, vector<bool> &tmp);
    void computeTransitiveReduction(ColorOrdering *order, vector<GraphNode<T> *> &nodes);

    int computeGraphInverse(CGraphWatcher *watcher);
    Image<RGB> constructImageInf();
    void areaFiltering(int areaMin, int areaMax);
    bool isLTE(RGB &v, RGB &w);
    Image<RGB> constructImageInverse();

    void contrastFiltering(int contrastMin);
    void contrastFiltering(int contrastMin, int contrastMax);
    void resetFiltering();

    /** Adaptive filtering **/
    void adaptiveAreaFiltering(int p);
    void adaptiveContrastFiltering(int p);

    bool isIncluded(GraphNode<T> *n, GraphNode<T> *m, vector<bool> &tmp);
private:
    void paintNode(Image<RGB> &im, GraphNode<T> *n, RGB &value);
    void paintNodeSup(Image<RGB> &imRes, GraphNode<T> *n, RGB &value);

    vector<GraphNode<T> *> computeComponents(Image<RGB> &im, FlatSE &connexity);

};

#endif // CGRAPH_H
