//Copyright (C) 2013, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include "ragraph.h"
#include "cgraph.h"

using namespace std;

template <>
Image<RGB> CGraph<RGB>::syntheticImage()
{
    int a[]={0,0,0};
    int b[]={1,0,0};
    int c[]={0,1,0};
    int d[]={1,1,0};
    int e[]={1,1,1};
    int f[]={0,0,1};

    RGB A(a);
    RGB B(b);
    RGB C(c);
    RGB D(d);
    RGB E(e);
    RGB F(f);

    TSize size[]={7,1,1};
    TSpacing spacing[]={1.0,1.0,1.0};
//    const RGB data[]={  A,B,C,B,A,
//                        C,C,D,B,A,
//                        B,B,C,A,B,
//                        B,C,A,B,A,
//                        A,C,B,C,A};
    //const RGB data[]={  A,C,C,C,B,E,D,D,C,E,E,D,B,E,B,C,C,C,B,B,A};
    const RGB data[]={ A,E,C,F,B,E,A};

    //    TSize size[]={4,4,1};
    //    TSpacing spacing[]={1.0,1.0,1.0};
    //    const RGB data[]={A,A,B,B,
    //                 B,C,B,B,
    //                 A,A,E,D,
    //                 A,E,E,D};
    Image<RGB> imTmp(size,spacing,data);
    return imTmp;
}

template <>
Image<RGB> CGraph<RGB>::syntheticImage2()
{
    int a[]={0,0,0};
    int b[]={1,0,0};
    int c[]={0,1,0};
    int d[]={1,1,0};
    int e[]={1,1,1};
    int f[]={0,0,1};

    RGB A(a);
    RGB B(b);
    RGB C(c);
    RGB D(d);
    RGB E(e);
    RGB F(f);

    TSize size[]={21,1,1};
    TSpacing spacing[]={1.0,1.0,1.0};
//    const RGB data[]={  A,B,C,B,A,
//                        C,C,D,B,A,
//                        B,B,C,A,B,
//                        B,C,A,B,A,
//                        A,C,B,C,A};
    const RGB data[]={  A,C,C,C,B,E,D,D,C,E,E,D,B,E,B,C,C,C,B,B,A};
    //const RGB data[]={ A,E,C,F,B,E,A};

    //    TSize size[]={4,4,1};
    //    TSpacing spacing[]={1.0,1.0,1.0};
    //    const RGB data[]={A,A,B,B,
    //                 B,C,B,B,
    //                 A,A,E,D,
    //                 A,E,E,D};
    Image<RGB> imTmp(size,spacing,data);
    return imTmp;
}


/**
*   Computation of component-graph \ddot G
**/
template <class T>
int CGraph<T>::computeGraph() {

    OrderedQueue <int> pq;

    vector<bool> processed(rag->getSize());

    for(int i=0; i<rag->getSize(); i++) {

        RGB value=rag->getValue(i);
        int priority=order->getPriority(value);
        // put in the prior. queue the flat zone id
        pq.put(priority,i);
        processed[i]=false;
    }

    // index from flat-zone id to corresponding node
    vector <GraphNode<T> *> regionToNode(rag->getSize(),(GraphNode<T>*)(0) );
    std::queue<int> fifo;
    std::vector<bool> infifo(regionToNode.size(), false);

    std::vector<GraphNode<T> *> potentialChilds;

    while(!pq.empty() ) {
        int i=pq.get();

        if(this->graphWatcher!=0)
            {
            this->graphWatcher->progressUpdate();
            }
        if(regionToNode[i]==0) {
            /** i is a canonical region (i==regionToNode[i]->index) */
            GraphNode<T> *curNode=new GraphNode<T>(rag->getIndex(i) ,rag->getValue(i));
            // add region pixels to curNode
            curNode->pixels.insert(curNode->pixels.end(), rag->getPixels(i).begin(), rag->getPixels(i).end());
            regionToNode[i]=curNode;

           // int valueMax=curNode->color[0]+curNode->color[1]+curNode->color[2];

            potentialChilds.clear();

            /** start the propagation */
            fifo.push(i);
            infifo[i]=true;
            int M=0;
            while(!fifo.empty()) {
                int curpt=fifo.front();
                M++;
                fifo.pop();
                std::vector<int> listNb=rag->getAllNb(curpt);
                for(int v=0; v<listNb.size() ; v++ ) {
                    int nb=listNb[v];
                    T value1=rag->getValue(i);
                    T value2=rag->getValue(nb);

                    if(!infifo[nb] && order->islessequal(value1,value2)) {

                        if(order->isequal(value1, value2)) {
                            /** flat-zone which belongs to the same node: merge pixels*/
                            curNode->pixels.insert(curNode->pixels.end(), rag->getPixels(nb).begin(), rag->getPixels(nb).end());

                            regionToNode[nb]=curNode;
                        }
                        else {
                            /**  flat-zone which is potentially a descendant of the current node:
                             *   if a father of the node is comparable with curNode, then
                             *   a is not a direct child of curNode
                             */
                            GraphNode<T> *tmp=regionToNode[nb];

                            //valueMax=std::max(valueMax,tmp->color[0]+tmp->color[1]+tmp->color[2]);

                            assert(tmp!=0);
                            bool isChild=true;
                            for(int a=0; a<tmp->fathers.size(); a++) {
                                if(order->islessequal( curNode->value , tmp->fathers[a]->value) ) {
                                    isChild=false;
                                    break;
                                }
                            }
                            if(isChild) {
                                curNode->childs.push_back(tmp);
                                tmp->fathers.push_back(curNode);

                            }
                        }
                        //curNode->area+=rag->nodes[nb]->pixels.size();
                        //curNode->contrast=valueMax-(curNode->color[0]+curNode->color[1]+curNode->color[2]);
                        fifo.push(nb);
                        infifo[nb]=true;
                    }
                }
            }
            processed[i]=true;
        }
    }

    /** Assign a new index to each node */
    for(int i=0; i<regionToNode.size(); i++) {
        if(regionToNode[i]!=0 && regionToNode[i]->id==i) {
            graph.push_back(regionToNode[i]);
            graph[graph.size()-1]->id=graph.size()-1;
        }

    }

    /** Add fictitious root*/

    T value(0);

    root=new GraphNode<T>(-1,value);

        for (int i=0; i<graph.size(); i++) {

            if(graph[i]->fathers.size()==0) {
                root->addChild(graph[i]);
            }
        }

    return 1;
}

template <>
bool CGraph<RGB>::isLTE(RGB &v, RGB &w)
{
    if(v[0]>=w[0] && v[1]>=w[1] && v[2]>=w[2] )
        return true;
    else return false;
}

template <class T>
void CGraph<T>::areaFiltering(int areaMin)
{
    for(int i=0; i<graph.size(); i++) {
        GraphNode<T> *n=graph[i];
        if(getArea(n)<areaMin) n->active=false;
    }

    if(getArea(root)<areaMin) root->active=false;
}

template <class T>
void CGraph<T>::areaFiltering(int areaMin, int areaMax)
{
    for(int i=0; i<graph.size(); i++) {
        GraphNode<T> *n=graph[i];
        if(getArea(n)<areaMin || getArea(n)>areaMax) n->active=false;
    }

    if(getArea(root)<areaMin || getArea(root)>areaMax) root->active=false;
}

template <class T>
void CGraph<T>::contrastFiltering(int contrastMin)
{
    for(int i=0; i<graph.size(); i++) {
        GraphNode<T> *n=graph[i];
        if(getContrast(n)<contrastMin) n->active=false;
    }

    if(getContrast(root)<contrastMin) root->active=false;
}

template <class T>
void CGraph<T>::contrastFiltering(int contrastMin, int contrastMax)
{
    for(int i=0; i<graph.size(); i++) {
        GraphNode<T> *n=graph[i];
        if(getContrast(n)<contrastMin || getContrast(n)>contrastMax) n->active=false;
    }

    if(getContrast(root)<contrastMin || getContrast(root)>contrastMax) root->active=false;
}

template <class T>
void CGraph<T>::resetFiltering()
{
/** Initialisation*/
    for(int i=0; i<graph.size(); i++) {graph[i]->active=true;}
}

/**
    Keep the p % nodes having the biggest area
*/
template <class T>
void CGraph<T>::adaptiveAreaFiltering(int p)
{
    std::map<int, int> histo;
    for(int i=0; i<graph.size(); i++) {
        GraphNode<T> *n=graph[i];
        histo[-getArea(n)]++;
    }

    histo[-getArea(root)]++;

    int totalNodes=graph.size()+1;
    int bestNodes=(p*totalNodes)/100;

    int count=0;

    int threshold;

    std::map<int,int>::iterator it;
    for(it=histo.begin(); it!=histo.end(); ++it) {
        count+=it->second;
        if(count> bestNodes )
            {
            threshold=-it->first;
            break;
            }
    }
    areaFiltering(threshold);
}

/**
    Keep the p % nodes having the biggest area
*/
template <class T>
void CGraph<T>::adaptiveContrastFiltering(int p)
{
    std::map<int, int> histo;
    for(int i=0; i<graph.size(); i++) {
        GraphNode<T> *n=graph[i];
        histo[-getContrast(n)]++;
    }

    histo[-getContrast(root)]++;

    int totalNodes=graph.size()+1;
    int bestNodes=(p*totalNodes)/100;

    int count=0;

    int threshold;

    std::map<int,int>::iterator it;
    for(it=histo.begin(); it!=histo.end(); ++it) {
        count+=it->second;
        if(count> bestNodes )
            {
            threshold=-it->first;
            break;
            }
    }
    //qDebug() << "Area = " << threshold <<"\n";
    contrastFiltering(threshold);

}

template <>
void CGraph<RGB>::paintNode(Image <RGB> &imRes, GraphNode<RGB> *n, RGB &value)
{
    for(int i=0; i<n->pixels.size(); i++) {
        imRes(n->pixels[i])=value;
    }
}

template <>
void CGraph<RGB>::paintNodeSup(Image <RGB> &imRes, GraphNode<RGB> *n, RGB &value)
{
    for(int i=0; i<n->pixels.size(); i++) {
        imRes(n->pixels[i])=value;
        RGB imValue=imRes(n->pixels[i]);
        imValue[0]=std::max(imValue[0],value[0]);
        imValue[1]=std::max(imValue[1],value[1]);
        imValue[2]=std::max(imValue[2],value[2]);
        imRes(n->pixels[i])=imValue;
    }
}

template <>
Image<RGB> CGraph<RGB>::constructImageInf()
{
    std::queue<GraphNode<RGB> *> fifo;

    vector <bool> active(graph.size(),true);

    OrderedQueue <GraphNode<RGB> *> pq;

    vector<bool> processed(graph.size());

    RGB rgbmin(0,0,0);

    /** Warning : nodes[0] does not exist !!!! */
    for(int i=0; i<graph.size(); i++) {

        graph[i]->modifiedValue =rgbmin;

        RGB value=graph[i]->value;

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put(-(R+G+B),graph[i]);
        processed[i]=false;
    }

    /** Special case for the root*/
    root->modifiedValue=rgbmin;

    while(!pq.empty()) {
        GraphNode<RGB> *n=pq.get();

        if(n->active==false && n->fathers.size()>1) {

            int nactive=0;
            RGB value(255,255,255);
            for(int j=0; j<n->fathers.size(); j++) {
                if(n->fathers[j]->active==true)
                {
                    nactive++;
                    value[0]=std::min(value[0],n->fathers[j]->value[0]);
                    value[1]=std::min(value[1],n->fathers[j]->value[1]);
                    value[2]=std::min(value[2],n->fathers[j]->value[2]);
                }
            }
            if(nactive>1) {
                for(int j=0; j<n->fathers.size(); j++) {

                    if(n->fathers[j]->active==true) {
                        n->fathers[j]->modifiedValue=value;
                    }
                }
            }
        }
    }

    Image<RGB> imRes=imSource;
    imRes.fill(0);

    for(int i=0; i<graph.size(); i++) {


        RGB value=graph[i]->value;

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put((R+G+B),graph[i]);
        processed[i]=false;
    }

    /** Special case for the root*/
    pq.put(-1,root);

    while(!pq.empty()) {
        GraphNode<RGB> *curNode=pq.get();
        if(curNode->active==true && curNode->modifiedValue !=rgbmin) {
            curNode->modifiedValue=curNode->value;
        }

        paintNode(imRes,curNode, curNode->modifiedValue);

        for(int c=0; c<curNode->childs.size(); c++) {
            GraphNode<RGB> *curChild=curNode->childs[c];
            if(curChild->active==false) {
               curChild->modifiedValue=curNode->modifiedValue;
            }

        }
    }

    return imRes;

}

template <>
Image<RGB> CGraph<RGB>::constructImage()
{
    Image<RGB> imRes=imSource;
    imRes.fill(0);
    std::queue<GraphNode<RGB> *> fifo;

    OrderedQueue <GraphNode<RGB> *> pq;

    vector<bool> processed(graph.size());

    RGB rgbmin(0,0,0);

    for(int i=0; i<graph.size(); i++) {

        RGB value=graph[i]->value;

        pq.put(-order->getPriority(value),graph[i]);
        processed[i]=false;
    }

    /** Special case for the root*/
    pq.put(-1,root);


    while(!pq.empty()) {
        GraphNode<RGB> *curNode=pq.get();
        if(curNode->active==true ) {
            curNode->modifiedValue=curNode->value;
        }

        paintNode(imRes,curNode, curNode->modifiedValue);

        for(int c=0; c<curNode->childs.size(); c++) {
            GraphNode<RGB> *curChild=curNode->childs[c];
            if(curChild->active==false) {
               curChild->modifiedValue=curNode->modifiedValue;
            }
        }
    }

    return imRes;
}


template <class T>
bool CGraph<T>::notComparable(Image<T> &im, Point<TCoord> &p, Point<TCoord> &q)
{
    if(!(im(p)<im(q) ) && !(im(p)>im(q)) && !(im(p)==im(q)))
        return true;
    else return false;

}

template <>
int CGraph<RGB>::writeDot(const char *filename)
{
    if(root!=0)
    {
        std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
        if(!file)
        {
            std::cerr << "File I/O error\n";
            return 0;
        }

        /** Maximum label of the graph **/
        int labelMax=graph.size()+1;
        bool isActive[labelMax];
        for(int i=0; i<labelMax; i++) isActive[i]=true;

        file << "digraph G {\n";

        std::queue <GraphNode<RGB> *> fifo;
        fifo.push(root);
        while(!fifo.empty() )
        {
            GraphNode<RGB> *tmp=fifo.front();
            fifo.pop();

            std::stringstream nodeName;
            nodeName.str("");
            nodeName << "\"" << tmp->id << ":(" <<(int)tmp->value[0] << "," <<
                        (int)tmp->value[1]<<","<<
                        (int)tmp->value[2]<< " a=" << attributes[GraphAttributes::Area][tmp->id] << ")\"";
            if(!tmp->active)
                file << "\t" << nodeName.str() << "[style=filled, fillcolor=grey];\n";

            for(int i=0; i<tmp->childs.size(); i++)
            {
                std::stringstream nodeNameChild;
                nodeNameChild << "\"" << tmp->childs[i]->id << ":(" <<(int)tmp->childs[i]->value[0] << "," <<
                                 (int)tmp->childs[i]->value[1]<<","<<
                                 (int)tmp->childs[i]->value[2]<< " a=" << attributes[GraphAttributes::Area][tmp->childs[i]->id] << ")\"";

                file << "\t" <<
                        nodeName.str() << "->" << nodeNameChild.str() << ";\n";

                if(isActive[tmp->childs[i]->id]==true)
                {
                    fifo.push(tmp->childs[i]);
                    isActive[tmp->childs[i]->id]=false;
                }
            }

        }

        file << "}\n";

        file.close();
        return 1;
    }
    else
        return 0;
}

/** check if set n is equal to m */
template <>
bool CGraph<RGB>::isEqual(GraphNode<RGB> *n, GraphNode<RGB> *m) {

    if(getArea(n)==getArea(m)) return false;

    for(int i=0; i<n->pixels.size(); i++) {
        TOffset curPixel=this->imSource.getOffset(n->pixels[i]);
        bool curPixelIncluded=false;
        for(int j=0; j<m->pixels.size(); j++) {
            if(curPixel==this->imSource.getOffset(m->pixels[j]))
                {
                curPixelIncluded=true;
                }
        }
        if(curPixelIncluded==false) return false;
    }

    return true;
}

/** Test if set n is included in m
*/
template <class T>
bool CGraph<T>::isIncluded(GraphNode<T> *n, GraphNode<T> *m, vector<bool> &tmp)
{
    tmp.assign(tmp.size(), false);
    for(int i=0; i<m->pixels.size(); i++)
        tmp[m->pixels[i]]=true;

    for(int i=0; i<n->pixels.size(); i++)
        if(tmp[n->pixels[i]]==false) return false;
    return true;
}

template <class T>
void CGraph<T>::computeLinks(vector <GraphNode<T> *> &nodes)
{
    vector<bool> tmp(imSource.getBufSize(),false);
    for(int i=0; i<nodes.size(); i++) {
        GraphNode<T> *n=nodes[i];
        for(int j=0; j<nodes.size(); j++) {
            if(j!=i && nodes[j]->area<=nodes[i]->area && order->islessequal(nodes[i]->color,nodes[j]->color)) {
                GraphNode<T> *m=nodes[j];
                if(!order->isequal(m->color,n->color) && isIncluded(m,n,tmp)) {
                    n->addChild(m);
                }
            }
        }
    }
}

template <class T>
GraphNode<T> * CGraph<T>::addRoot(vector <GraphNode<T> *> &nodes)
{
    GraphNode<T> *root=new GraphNode<T>(-1,0 ,0);

    for (int i=0; i<nodes.size(); i++) {
        if(nodes[i]->fathers.size()==0) {
            root->addChild(nodes[i]);
        }
    }
    return root;
}

template <class T>
vector <GraphNode<T> *> CGraph<T>::minimalElements(vector <GraphNode<T> *> &nodes, vector <bool> &tmp) {
    vector <GraphNode<T> *> res;

    vector <bool> active(nodes.size(), true);
    for(int j=0; j<nodes.size(); j++) {
        RGB i=nodes[j]->color;
        for(int k=0; k<nodes.size(); k++) {
            if(k!=j) {
                RGB value2=nodes[k]->color;
                if(order->islessequal(i,value2) && isIncluded(nodes[k],nodes[j], tmp) ) {
                    active[k]=false;
                }
            }
        }
    }

    for(int j=0; j<nodes.size(); j++) {
        if(active[j]) res.push_back(nodes[j]);
    }

    return res;
}

/** Compute transitive reduction of graph from its root
* For each node:
*  - keep as childs only the minimal elements of the childs
*/
template <class T>
void CGraph<T>::computeTransitiveReduction(vector<GraphNode<T> *> &nodes)
{
    vector<bool> tmp(imSource.getBufSize(),false);
    for(int i=0; i<nodes.size(); i++) {
        GraphNode<T> *curNode=nodes[i];
        curNode->childs=minimalElements(order,curNode->childs, tmp);
    }
}

/** Compute the nodes for G and \dot G component-graph
**/
template <class T>
vector<GraphNode<T> *> CGraph<T>::computeComponents(OrderedQueue <RGB> &pq)
{
    int dx=imSource.getSizeX();
    int dy=imSource.getSizeY();

    vector <GraphNode<T> *> nodes;

    Image<bool> active(imSource.getSize());
    Image<bool> inQueue(imSource.getSize());

    inQueue.fill(false);
    std::queue<Point<TCoord > > fifo;

    int index=1;

    while(!pq.empty())
    {
        RGB value=pq.get();

        active.fill(true);

        int ncomposantes=0;

        for(int y=0; y<dy; y++)
            for(int x=0;x<dx; x++) {

                if(active(x,y) && order->islessequal(value,imSource(x,y))) {
                /** Construct a new node and begin a propagation
                **/
                    Point <TCoord> p(x,y);
                    inQueue.fill(false);

                    fifo.push(p);
                    ncomposantes++;
                    inQueue(p)=true;

                    GraphNode<T> *n=new GraphNode<T>(index,value,0);
                    index++;

                     if(index%100000==0)
                    {
                        std::cout << "Node : " << n->index << " Value: "<< (int)value[0] << "," << (int)value[1] << "," << (int)value[2] << "\n";
                    }

                    while(!fifo.empty()) {
                        Point<TCoord> curPt=fifo.front();
                        fifo.pop();

                        active(curPt)=false;

                        n->area++;
                        n->pixels.push_back(imSource.getOffset(curPt));
                        for(int i=0; i<connexity.getNbPoints(); i++) {
                            Point<TCoord> q=curPt+connexity.getPoint(i);

                            if(imSource.isPosValid(q)) {

                                if(inQueue(q)==false && order->islessequal(n->color,imSource(q)) ) {
                                    fifo.push(q);
                                    inQueue(q)=true;
                                }
                            }
                        }
                    }
                    nodes.push_back(n);
                }

            }
        if(ncomposantes!=0) {
        std::cout << "Level: " << (int)value[0] << " " << (int)value[1] << " " << (int)value[2] << "\n";
        std::cout << "N Composantes : " << ncomposantes << "\n";
        }
    }

    return nodes;
}



/**
* Compute component-graph G
* -compute all components
* -compute inclusion relations/ construct links
* -compute the transitive reduction
**/
template <class T>
int CGraph<T>::computeGraphFull()
{
    Image <T> im=this->imSource;
    OrderedQueue <RGB> pq;

    vector<GraphNode<T> *> nodes;

    vector <RGB> colorProcessed;

    /** Put all colors in priority queue
    **/
    for(int r=0; r<256; r++)
        for(int g=0; g<256; g++)
            for(int b=0; b<256; b++)
            {
                RGB value(r,g,b);
                pq.put(order->getPriority(value),value);

            }

    std::cout << "Begin compute components\n";
    clock_t c1=clock();

    nodes=computeComponents(order,pq);

    std::cout << "Number of nodes: " << nodes.size() << "\n";

     clock_t c2=clock();
    std::cout << "End compute components Time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";


    std::cout << "Begin compute links\n";
    c1=clock();
    computeLinks(order,nodes);
    c2=clock();
    std::cout << "End compute links Time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";

    for(int i=0; i<nodes.size(); i++)
        graph.push_back(nodes[i]);
    root=addRoot(nodes);

    writeDot("fulllinks.dot");
//    std::cout << "Begin compute transitive reduction\n";
//    c1=clock();
   computeTransitiveReduction(order, nodes);
//    c2=clock();
//    std::cout << "End compute transitive reduction time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";

    writeDot("fullgraph.dot");
    return 0;
}