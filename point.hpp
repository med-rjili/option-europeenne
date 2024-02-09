#include <iostream>


using namespace std;


class Point {
    public:
        double x,y;
        Point (double x=0,double y=0);
        void print(std::ostream &out=std::cout) const;
        friend std::ostream &operator<<(std::ostream &out, const Point &pt);
        Point& operator=(const Point& other);
        Point& operator+=(const Point& mat);
        Point& operator-=(const Point& mat);
        Point& operator*=(double alpha);
        Point& operator/=(double alpha);
    };


Point operator+(const Point& pt1, const Point& pt2);
Point operator-(const Point& pt1, const Point& pt2);
Point operator*(double alpha, const Point& pt);
Point operator/(const Point& pt,double alpha);