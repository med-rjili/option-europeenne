#include "elementsFinies.hpp"
#include <vector>
#include <fstream>
using namespace std;


void output(string file_name,const matrice& M, const vector<double> T)
{
    ofstream out(file_name);
    out << "$Time" << endl;
    for (auto& t:T) out << t << " ";
    out << endl << "$Solution" << endl;
    M.print(out);
}

vector<double> timeSlots(double t0, double tf, double dt)
{
    vector<double> Time;
    cout<<"t0 = "<<t0<<endl;
    cout<<"tf = "<<tf<<endl;
    cout<<"dt = "<<dt<<endl;
    int steps = (int) (tf-t0)/dt;
    cout<<"steps = "<<steps<<endl;
    for (int i=0; i<=steps; i++) {    
        Time.push_back(t0+i*dt);
    }
    if ((*Time.end())!= tf) Time.push_back(tf);
    return Time;
}

matrice matM_el(const Point& S1, const Point& S2, const Point& S3)
{
    //lecture des coordonnees de S1, S2, S3
    double x1= S1.x, y1= S1.y;
    double x2= S2.x, y2= S2.y;
    double x3= S3.x, y3= S3.y;

    double D = abs((x2-x3)*(y3-y1) - (x3-x1)*(y2-y3));


    //remplissage de la matrice M_el
    matrice M(3,3,D/12);
    for (int i=0; i<3; i++) M.set(i,i,D/24);

    return(M);
}
matrice mat_cov()
{
    matrice c(2,2);
    c.set(0,0,0.04);
    c.set(0,1,-0.024);
    c.set(1,0,-0.024);
    c.set(1,1,0.04);
    return c;
}
matrice A(const Point& P)
{
    matrice mat_A(2,2), C=mat_cov();
    double x= P.x, y=P.y;
    mat_A.set(0,0,C.val(0,0)*x*x);
    mat_A.set(0,1,C.val(0,1)*x*y);
    mat_A.set(1,0,C.val(1,0)*y*x);
    mat_A.set(1,1,C.val(1,1)*y*y);
    return (0.5*mat_A);
}

Vecteur<double> V(const Point& P)
{
    double r=0.05;
    Vecteur<double> vect_V(2); matrice C=mat_cov();
    double x=P.x, y=P.y;
    vect_V[0]= (C.val(0,0)+0.5*C.val(1,0)-r)*x;
    vect_V[1]= (C.val(1,1)+0.5*C.val(0,1)-r)*y;
    return vect_V;
}
matrice W_ref(const vector<Point>& P)
{
    matrice mat_W(4,3);
    int i=0;
    for(vector<Point>::const_iterator pp=P.begin();pp!=P.end();pp++,i++)
    {
        double x=(*pp).x,y=(*pp).y;
        mat_W.set(i,0,1-x-y);
        mat_W.set(i,1,x);
        mat_W.set(i,2,y);
    }
    return mat_W;
}
matrice grad_W(double x1, double x2, double x3, double y1, double y2, double y3)
{
    matrice gard_W(3, 2);
    gard_W.set(0,0,y2-y3);
    gard_W.set(0,1,-x2+x3);
    gard_W.set(1,0,y3-y1);
    gard_W.set(1,1,-x3+x1);
    gard_W.set(2,0,y1-y2);
    gard_W.set(2,1,-x1+x2);
    return gard_W;
}
pair<vector<Point>, Vecteur<double>> quad_2()
{
    vector<Point> S;
    S.push_back(Point(1./6, 1./6));
    S.push_back(Point(2./3, 1./6));
    S.push_back(Point(1./6, 2./3));
    Vecteur<double> C(3,1./6);
    C[0]=1./6;C[1]=1./6;C[2]=1./6;
    return std::make_pair( S, C);
}

pair<vector<Point>, Vecteur<double>> quad_3()
{
    vector<Point> S;
    S.push_back(Point(1.0/3, 1.0/3));
    S.push_back(Point(0.6, 0.2));
    S.push_back(Point(0.2, 0.6));
    S.push_back(Point(0.2, 0.2));
    Vecteur<double> C(4);
    C[0]=-27.0/48;C[1]=25.0/48;C[2]=25.0/48;C[3]=25.0/48;
    return std::make_pair(S, C);
}
vector<Point> ref_transform(const vector<Point>& P,pair<matrice, Vecteur<double>> v)
{
    vector<Point> res;
    for (unsigned long i=0; i<P.size(); i++){
            Vecteur<double> pt(2), new_pt(2);
            pt[0]=P[i].x;
            pt[1]=P[i].y;
            new_pt = (v.first)*pt + v.second;
            res.push_back(Point(new_pt[0], new_pt[1]));
    }
    return res;
}

pair<matrice, Vecteur<double>> trans_elt(double x1, double x2, double x3, double y1, double y2, double y3)
{
    matrice Bl(2,2); Vecteur<double> Sl(2);
    Bl.set(0,0,x2-x1);
    Bl.set(0,1,x3-x1);
    Bl.set(1,0,y2-y1);
    Bl.set(1,1,y3-y1);
    Sl[0]=x1;Sl[1]=y1;
    return std::make_pair(Bl, Sl);
}

matrice matK_el(const Point& S1, const Point& S2, const Point& S3)
{
    matrice K(3,3);

    //lecture des coordonnees de S1, S2, S3
    double x1= S1.x, y1= S1.y;
    double x2= S2.x, y2= S2.y;
    double x3= S3.x, y3= S3.y;

    //Calculs preliminaires
    double D = (x2-x3)*(y3-y1) - (x3-x1)*(y2-y3);
    double abs_det_Bl = abs((x2-x1)*(y3-y1) -(y2-y1)*(x3-x1));
    //Matrice Grad_W: Grad_W[i] = gard(wi)
    matrice Grad_W = grad_W(x1, x2, x3, y1, y2, y3);
    
    //Quadrature de gauss a 3 points d'ordre 2;
    pair<vector<Point>, Vecteur<double>> Q= quad_2();
    vector<Point> S_hat=Q.first; Vecteur<double> c_hat= Q.second;
    vector<Point> S_hat_trans= ref_transform(S_hat, trans_elt(x1, x2, x3, y1, y2, y3));
    for (int i=0; i<3; i++){
        for (int j=0; j<3; j++){
            //Calcul de la fomule de quadrature
            for(int k=0; k<3; k++){
                K.set(i,j,K.val(i,j)+ (c_hat[k]* ((A(S_hat_trans[k])*Grad_W.getRow(i))|Grad_W.getRow(j))) * (abs_det_Bl/(D*D))); // A ????
            }
        }
    }
    return K;
}
matrice matB_el(const Point& S1, const Point& S2, const Point& S3)
{
    matrice B(3,3);

    //lecture des coordonnees de S1, S2, S3
    double x1= S1.x, y1= S1.y;
    double x2= S2.x, y2= S2.y;
    double x3= S3.x, y3= S3.y;

    //Quadrature de gauss a 4 points d'ordre 3;
    pair<vector<Point>, Vecteur<double>> Q= quad_3();
    vector<Point> S_hat=Q.first; Vecteur<double> c_hat= Q.second;
    vector<Point> S_hat_trans= ref_transform(S_hat, trans_elt(x1, x2, x3, y1, y2, y3));

    //Calculs preliminaires
    double D = (x2-x3)*(y3-y1) - (x3-x1)*(y2-y3);
    double abs_det_Bl = abs((x2-x1)*(y3-y1) -(y2-y1)*(x3-x1));

    //Matrice Grad_W: Grad_W[i] = gard(wi)
    matrice Grad_W = grad_W(x1, x2, x3, y1, y2, y3);


    //Matrie W_ref: W_ref[i][j]= wj[Pi];
    matrice W = W_ref(S_hat);

    for (int i=0; i<3; i++){
        for (int j=0; j<3; j++){
            //Calcul de la fomule de quadrature a 4 points
            for(int k=0; k<4; k++){
                B.set(i,j,B.val(i,j)+ c_hat[k]*(V(S_hat_trans[k])|Grad_W.getRow(j))* W.val(k,i)*(abs_det_Bl/D)) ;
            }
        }
    }
    return B;
}
