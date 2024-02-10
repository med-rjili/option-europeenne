#include <iostream>
#include "point.hpp"

#ifndef TRIANGLE_HPP_INCLUDED
#define TRIANGLE_HPP_INCLUDED

using namespace std;

class Triangle {
    public:
        int s1,s2,s3,ordre;
        Triangle(int s1,int s2,int s3, int ordre=1): s1(s1),s2(s2),s3(s3),ordre(ordre){}
        void print(std::ostream &out)const;
        friend std::ostream &operator<<(std::ostream &out, const Triangle &t);
};

#endif