//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#ifndef ORDERING_H
#define ORDERING_H

#include "Types.h"

using namespace LibTIM;

template <class T>
class Ordering
{
 public:
         inline virtual bool islessequal(T &v, T &w)=0;
         inline virtual bool isequal(T &v, T &w)=0;
         inline virtual int getPriority(T &v)=0;
};

class ColorMarginalOrdering : public Ordering<RGB> {
    public:
          inline bool islessequal(RGB &v, RGB &w) {
            if(v[0]<=w[0] && v[1] <= w[1] && v[2]<=w[2])
                return true;
            else return false;
         }

         inline bool isequal(RGB &v, RGB &w) {
            if(v[0] == w[0] && v[1]==w[1] && v[2]==w[2]) return true;
            else return false;
         }

         inline int getPriority(RGB &v) {
            return -(v[0]+v[1]+v[2]);
         }
};


#endif // ORDERING_H
