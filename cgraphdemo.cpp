//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include <ctime>

#include "cgraph.h"
#include "cgraphwatcher.h"
#include "ordering.h"

using namespace std;

// for LibTIM classes
using namespace LibTIM;

class graphWatcher : public CGraphWatcher{
    public :
        void progressUpdate(int ncur, int nfinal) {
            if(ncur%100==0) {
                std::cout << ncur << " / " << nfinal << "\r";
            }
        }

        void progressUpdate() {
            curProgress++;
            if(curProgress%100==0) {
                std::cout << this->curProgress << " / " << this->finalProgress << "\n";

            }
        }
    graphWatcher(int finalProgress) : CGraphWatcher(finalProgress) {}
};

class myAttributes {
public:
    myAttributes() {
        area=0;
        volume=0;
        contrast=0;
        RGB value(0,0,0);
        valueMax=0;
        compacity=0;
    };

    void updateArea(const std::vector<Point <TCoord > > &pixels) {
        this->area+=pixels.size();
    }

    void updateContrast(const RGB &v) {
        this->valueMax=std::max(this->valueMax,v[0]+v[1]+v[2]);
        this->contrast=this->valueMax-(this->value[0]+this->value[1]+this->value[2]);
    }

    void update(const std::vector< Point <TCoord > > &pixels, const RGB &value) {
        updateArea(pixels);
        updateContrast(value);
    };

    std::string writeDot() {
        std::stringstream s;
        s << "a=" << area << "," <<
             "c=" << contrast ;
        return s.str();
    }

    void setValue(RGB &value) {
        this->value=value;
    }

    RGB value;
    int area;
    int volume;
    int contrast;
    int valueMax;
    int compacity;
};

/**
* Colour image attribute filtering based on \ddot component-graph on RGB colour space
**/

int main(int argc, char *argv[])
{
    if(argc!=4)
    {
        cout<<"Usage: " << argv[0] << " <source_image.ppm> <area_min> <contrast_min>  \n";
        exit(1);
    }

    // Declaration of imSrc (source image)

    Image <RGB> imSrc;

    // Declaration of :
    // -connexity: 8-adjacency
    FlatSE connexity;
    connexity.make2DN8();

    // Load source image imSrc in ppm format, 2D RGB (8 bits)
    Image<RGB>::load(argv[1],imSrc);

    int areaMin=atoi(argv[2]);
    int contrastMin=atoi(argv[3]);

    ColorMarginalOrdering  *order=new ColorMarginalOrdering();

    CGraph<RGB,myAttributes> *cgraph=new CGraph<RGB,myAttributes>(imSrc,connexity,order);

    // Track computation progress
    graphWatcher *myWatcher=new graphWatcher(imSrc.getBufSize());
    cgraph->addWatcher(myWatcher);
    // Set marginal ordering on RGB colour space
    // Compute \ddot component-graph
    cgraph->computeGraph();

    // Area and contrast filtering
    //cgraph->areaFiltering(areaMin);
    //cgraph->contrastFiltering(contrastMin);

    // Write graph in dot format
    // Filtered nodes are shown in gray
    cgraph->writeDot("cgraph.dot");

    // Compute resulting image
    //Image<RGB> result=cgraph->constructImage();
    // Save result in ppm format
    //result.save("result.ppm");

    //delete cgraph;
}

