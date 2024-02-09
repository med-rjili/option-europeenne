#include "point.hpp"

using namespace std;



Point::Point(double x,double y):x(x),y(y){}
void Point::print(std::ostream &out)const{
    out<<"("<<x<<","<<y<<")";
}

std::ostream &operator<<(std::ostream &out, const Point &pt) {
    pt.print(out);
    return out;
}

Point& Point::operator=(const Point& other){
    if(this != &other){
        x=other.x;
        y=other.y;
    }
    return *this;
}

Point& Point::operator+=(const Point& pt){
    x+=pt.x;
    y+=pt.y;
    return *this;
}
Point& Point::operator-=(const Point& pt){
    x-=pt.x;
    y-=pt.y;
    return *this;
}
Point& Point::operator*=(double alpha){
    x*=alpha;
    y*=alpha;
    return *this;
}
Point& Point::operator/=(double alpha){
    if(alpha==0) {
        cout<<"divide by zero error !!!!"<<endl;
        exit(-1);
    }
    x/=alpha;
    y/=alpha;
    return *this;
}


Point operator+(const Point& pt1, const Point& pt2){
    Point res=pt1;
    res+=pt2;
    return res;
}
Point operator-(const Point& pt1, const Point& pt2){
    Point res=pt1;
    res-=pt2;
    return res;
}
Point operator*(double alpha, const Point& pt){
    Point res=pt;
    res*=alpha;
    return res;
}
Point operator/(const Point& pt, double alpha){
    Point res=pt;
    res/=alpha;
    return res;
}