#include "triangle.hpp"
#include <iostream>

using namespace std;

void Triangle::print(std::ostream &out)const{
        out<<"Triangle("<<s1<<","<<s2<<","<<s3<<")";
    }
std::ostream &operator<<(std::ostream &out, const Triangle &t) {
    t.print(out);
    return out;
}