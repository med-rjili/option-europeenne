#ifndef ELEMENTS_FINIS_HPP
#define ELEMENTS_FINIS_HPP
#include "maillage.hpp"
#include "vecteur.hpp"
#include "matrice.hpp"

vector<double> timeSlots(double t0, double tf, double dt);
void output(string file_name,const matrice& M, const vector<double> T);
matrice mat_cov();
matrice A(const Point& p);
Vecteur<double> V(const Point& p);
matrice W_ref(const vector<Point>& p);
matrice grad_W(double x1, double x2, double x3, double y1, double y2, double y3);
pair<vector<Point>, Vecteur<double>> quad_2();
pair<vector<Point>, Vecteur<double>> quad_3();
vector<Point> ref_transform(const vector<Point>& P,pair<matrice, Vecteur<double>> p);
pair<matrice, Vecteur<double>> trans_elt(double x1, double x2, double x3, double y1, double y2, double y3);
matrice matM_el(const Point& S1, const Point& S2, const Point& S3);
matrice matK_el(const Point& S1, const Point& S2, const Point& S3);
matrice matB_el(const Point& S1, const Point& S2, const Point& S3);
#endif // ELEMENTS_FINIS_HPP
