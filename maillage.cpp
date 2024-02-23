#include "maillage.hpp"
#include "point.hpp"
#include <iostream>
#include <fstream>

using namespace std;

Maillage::Maillage(int m, int n): dimH(m+1),dimV(n+1){
    Maillage_carre();
}
void Maillage::Maillage_carre(){
    double pasH=1.0/(dimH-1);
    double pasV=1.0/(dimV-1);
    Point p00(0,0);
    for(int i=0;i<dimH;i++){
        for(int j=0;j<dimV;j++){
            p00=Point(i*pasH,0)+Point(0,j*pasV);
            points.push_back(p00);
            if(i<dimH-1 and j<dimV-1) {
                triangles.push_back(Triangle(j+i*dimV,j+i*dimV+1,j+(i+1)*dimV));
                triangles.push_back(Triangle(j+i*dimV+1,j+(i+1)*dimV,j+1+(i+1)*dimV));
            }
        }
    }
    
}
Maillage::Maillage(int a,int b,int c,int d,int m, int n):dimH(m+1),dimV(n+1){
    Maillage_rectangle(a,b,c,d);
}
void Maillage::Maillage_rectangle(int a,int b,int c,int d){
    Maillage_carre();
    for(unsigned long i=0;i<points.size();i++)
    {
       
        points[i].x=points[i].x*(b-a)+a;
        points[i].y=points[i].y*(d-c)+c;
        
    }


}

void Maillage::print()const{
    cout<<"Points:"<<endl;
    for(auto pt:points){
        cout<<pt<<" ";
    }
    cout<<endl<<"Triangles"<<endl;
    for(auto t:triangles){
        cout<<t<<" ";
    }
    cout<<endl;
}

void Maillage::Save (const char *fn) const
{
    ofstream os(fn) ;
    os << "$Nodes" << endl;
    os << points.size() << endl;
    for (auto& p: points) p.print(os);
    os << "$Elements" << endl;
    os << triangles.size() << endl;
    for (auto& num: triangles) num.print(os);
    os.close() ;
}