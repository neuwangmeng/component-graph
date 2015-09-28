#ifndef TESTS_H
#define TESTS_H

#include "Image.h"

using namespace LibTIM;

class Tests
{
public:
    Tests();
    Image<RGB> syntheticImage();
    Image<RGB> syntheticImage2();
};

#endif // TESTS_H
