//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef RAGRAPH_H
#define RAGRAPH_H


#include "Image.h"
#include "FlatSE.h"

using namespace LibTIM;

/** Region adjacency graph **/

template <class T>
class RAGraph {

private:
    struct Vertex {
        int index;

        T value;

        std::vector <int> compNb;
        std::vector <int> nonCompNb;

        std::vector <int> allNb;  /*!< All neighbors */
        std::vector <Point <TCoord> > pixels;

        Vertex(int n) {
            this->index=n;
        }

        Vertex() {
        }

        void insertCompNb(int index) {

            for(int i=0; i<compNb.size(); i++) {
                if(this->compNb[i]==index) return;
            }
            compNb.push_back(index);
            allNb.push_back(index);
        }

        void insertNonCompNb(int index) {
            for(int i=0; i<nonCompNb.size(); i++) {
                if(nonCompNb[i]==index) return;
            }
            nonCompNb.push_back(index);
            allNb.push_back(index);
        }
    };

    Image <T> imSource;
    FlatSE connexity;
    Image <TLabel> imFlatZones;
    std::vector <Vertex *> nodes;
    template <class U>
    void addBorders(Image<U> &im, const TCoord *preWidth, const TCoord *postWidth, U value)
    {
        TSize newSize[3];
        const TSize *oriSize = im.getSize();

        for (int i = 0; i < 3; i++)
        {
            newSize[i] = oriSize[i] + preWidth[i] + postWidth[i];
        }

        Image<U> temp(newSize);

        typename Image<U>::iterator it;
        typename Image<U>::iterator end=temp.end();

        std::fill(temp.begin(), end,value);
        temp.copy(im, preWidth[0],preWidth[1],preWidth[2]);

        im=temp;
    }

    void computeRAGraph();
    void computeFlatZones();

public :

    RAGraph(Image <T> imSource, FlatSE connexity) {
        this->imSource=imSource;
        this->connexity=connexity;
        computeRAGraph();
    }
    ~RAGraph() {
        for(int i=1; i<nodes.size(); i++) delete nodes[i];
    }

    int getSize() {return nodes.size();}
    T getValue(int i) {return nodes[i]->value;}
    std::vector<Point<TCoord> > getPixels(int i) {return nodes[i]->pixels;}
    int getIndex(int i) {return nodes[i]->index; }
    std::vector<int> getAllNb(int i) {return nodes[i]->allNb;}

    void writeDot(const char *fileName);
    void print();
};

#include "ragraph.hxx"

#endif // RAGRAPH_H
