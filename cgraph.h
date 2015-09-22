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
#include "ordering.h"

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
    RAGraph<T> *rag; /*!< Region adjacency graph on which is computed the component-graph */
    Image<T> imSource; /*! Source image (2D or 3D) */
    FlatSE connexity;   /*!< Underlying connexity (2D: 4- 8-  or 3D : 6- 18- 26-) */
    Ordering <T> *order; /*!<Ordering used for type T */

    /** Track computation progress **/
    CGraphWatcher *graphWatcher;
    /** Container of the nodes of the (directed) graph **/
    std::vector<GraphNode<T> *> graph;
    /** Attributes of the nodes **/
    std::map<GraphAttributes,std::vector<int> > attributes;
    /** Root of the graph **/
    GraphNode<T> *root;

public:

    CGraph(Image <RGB> &imSource, FlatSE &connexity, std::vector<GraphAttributes> listOfAttributes, Ordering<T> *order) : graphWatcher(0) {
        this->imSource=imSource;
        this->connexity=connexity;
        this->rag=new RAGraph<T>(imSource,connexity);
        for(int i=0; i<listOfAttributes.size(); i++) {
            std::vector<int> v;
            this->attributes[listOfAttributes[i]]=v;
        }
        this->order=order;
    }

    CGraph(RAGraph<T> *rag) : rag(rag) {}
    ~CGraph() { delete rag;}

    void addWatcher(CGraphWatcher *graphWatcher) {
        this->graphWatcher=graphWatcher;
    }

    /** Component-graph \ddot G **/
    int computeGraph();
    /** Component-graph G **/
    int computeGraphFull();

    /** Return synthetic images */
    static Image<RGB> syntheticImage();
    static Image<RGB> syntheticImage2();

    /** Write graph in dot format **/
    int writeDot(const char *filename);

    void areaFiltering(int areaMin);
    void reconstructMin();

    Image<RGB> constructImage();

    /** Helper functions **/
    vector<GraphNode<T> *> computeComponents(OrderedQueue<RGB> &pq);
    static bool notComparable(Image<T> &im, Point<TCoord> &p, Point<TCoord> &q);

    bool isEqual(GraphNode<T> *n, GraphNode<T> *m);
    bool isIncluded(GraphNode<T> *n, GraphNode<T> *m);
    bool isIncludedFast(GraphNode<T> *n, GraphNode<T> *m, vector<bool> &tmp);
    void computeLinks(vector<GraphNode<T> *> &nodes);
    GraphNode<T> *addRoot(vector<GraphNode<T> *> &nodes);
    vector<GraphNode<T> *> minimalElements(vector<GraphNode<T> *> &nodes, vector<bool> &tmp);
    void computeTransitiveReduction( vector<GraphNode<T> *> &nodes);

    Image<RGB> constructImageInf();
    void areaFiltering(int areaMin, int areaMax);
    bool isLTE(T &v, T &w);

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

    int getArea(GraphNode <T> *node) {return attributes[GraphAttributes::Area][node->id];}
    int getContrast(GraphNode <T> *node) {return attributes[GraphAttributes::Contrast][node->id];}
    int getVolume(GraphNode <T> *node) {return attributes[GraphAttributes::Volume][node->id];}
    int getCompacity(GraphNode <T> *node) {return attributes[GraphAttributes::Compacity][node->id];}
};

#include "cgraph.hxx"

#endif // CGRAPH_H
