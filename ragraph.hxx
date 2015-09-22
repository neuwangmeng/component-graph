//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#include "ragraph.h"

template <class T>
void RAGraph<T>::computeFlatZones()
{
    imFlatZones.setSize(imSource.getSize());

    FlatSE se=this->connexity;

    std::queue<TOffset> fifo;

    const TCoord *back=se.getNegativeOffsets();
    const TCoord *front=se.getPositiveOffsets();

    TLabel BORDER=0;
    TLabel INIT=1;
    TLabel FIRSTLABEL=2;

    Image<T> imBorder=imSource;
    Image<TLabel> resBorder(imSource.getSize() );
    resBorder.fill(INIT);

    addBorders(imBorder,back,front,T(0));
    addBorders(resBorder,back,front,BORDER);

    se.setContext(imBorder.getSize());

    Image<RGB>::iterator it;
    Image<RGB>::iterator end=imBorder.end();

    FlatSE::iterator itSe;
    FlatSE::iterator endSe=se.end();

    TLabel currentLabel=FIRSTLABEL;

    TOffset curOffset=0;
    for(it=imBorder.begin(); it!=end; ++it,curOffset++)
    {
        if(resBorder(curOffset) ==INIT)
        {
            fifo.push(curOffset);

            while(!fifo.empty())
            {
                TOffset p=fifo.front();
                fifo.pop();

                resBorder(p)=currentLabel;

                for(itSe=se.begin(); itSe!=endSe; ++itSe)
                {
                    TOffset q=p+ *itSe;

                    if(imBorder(q)==imBorder(p) && resBorder(q)==INIT)
                    {

                        resBorder(q)=currentLabel;
                        fifo.push(q);
                    }
                }
            }
            currentLabel++;
        }
    }

    // compute imFlatZones by removing borders
    // and maps the result to 0 ... NB_LABEL-1
    Image<TLabel>::iteratorXYZ itLabelXYZ;
    Image<TLabel>::iteratorXYZ endRes=imFlatZones.end();
    for(itLabelXYZ=imFlatZones.begin(); itLabelXYZ!=endRes; ++itLabelXYZ)
    {
        *itLabelXYZ=resBorder(itLabelXYZ.x+back[0], itLabelXYZ.y+back[1], itLabelXYZ.z+back[2])-FIRSTLABEL;
    }
}

template <class T>
void RAGraph<T>::computeRAGraph() {

    int dx=imSource.getSizeX();
    int dy=imSource.getSizeY();
    int dz=imSource.getSizeZ();

    computeFlatZones();

    TLabel nbFlatZones=imFlatZones.getMax()+1;
    std::cerr << "Max fz : " << imFlatZones.getMax() << "\n";

    // flat-zones are numbered 0 to nbFlatZones-1
    // to achieve simple indexing we reserve nbFlatZones nodes
    // so nodes[FZi] stores the flat-zone numbered i
    std::cerr << "Nb flat zones : " << nbFlatZones << "\n";
    nodes.resize(nbFlatZones);
    for(int i=0; i<nodes.size(); i++) nodes[i]=new Vertex(i);

    for(int z=0; z<dz; z++)
        for(int y=0; y<dy; y++)
            for(int x=0; x<dx; x++) {

                Point <TCoord> p(x,y,z);
                // add pixel to corresponding node
                nodes[imFlatZones(p)]->pixels.push_back(p);
                nodes[imFlatZones(p)]->value=imSource(p);

                //scan the neighbors
                for(int i=0; i<connexity.getNbPoints(); i++) {
                    Point <TCoord> q=p+connexity.getPoint(i);

                    if(imSource.isPosValid(q)) {
                        if(imSource(p)!=imSource(q)) {
                         Vertex *v=nodes[imFlatZones(x,y,z)];
                         // search for comparable neighbor
                         if(imSource(p)<imSource(q) || imSource(p)>imSource(q)) {
                            v->insertCompNb(imFlatZones(q));

                         }
                         // non-comparable neighbor
                         else
                            v->insertNonCompNb(imFlatZones(q));
                        }
                    }
                }

            }
}

template <class T>
void RAGraph<T>::writeDot(const char *fileName) {
    // TODO
}

template <class T>
void RAGraph<T>::print() {
    for(int i=1; i<nodes.size(); i++) {
        assert(i=nodes[i]->index);
        std::cout << "Node : " << nodes[i]->index << "\n";
        std::cout << "\t Color : (" << (int)nodes[i]->value[0] << "," << (int)nodes[i]->value[1] << "," << (int)nodes[i]->value[2]   << ")\n";
        for(int j=0; j<nodes[i]->compNb.size(); j++) {
            std::cout << "\t ->" << nodes[i]->compNb[j]<< "\n";
        }

        for(int j=0; j<nodes[i]->nonCompNb.size(); j++) {
            std::cout << "\t ~~" << nodes[i]->nonCompNb[j]<< "\n";
        }
    }
}
