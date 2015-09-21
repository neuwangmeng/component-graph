#ifndef GRAPHATTRIBUTES_H
#define GRAPHATTRIBUTES_H

#include<map>

class GraphAttributes
{
private:
    std::map<enum AttributesName, int value> attributes; /// associative array: attribute name -> attribute value
public:
    GraphAttributes();

    enum AttributesName {
        Area,
        Contrast,
        Volume,
        Compacity
    };
};

#endif // GRAPHATTRIBUTES_H
