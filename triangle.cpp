#include "triangle.hpp"
#include <iostream>

using namespace std;

void Triangle::print(std::ostream &out)const{
        out<<"Triangle("<<s1<<","<<s2<<","<<s3<<")";
    }

int Triangle::getS(int i){
    switch (i)
    {
    case 1:
        return s1;
        break;
    
    case 2:
        return s2;
        break;
    default:
        return s3;
    }
}
std::ostream &operator<<(std::ostream &out, const Triangle &t) {
    t.print(out);
    return out;
}
