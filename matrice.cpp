#include "matrice.hpp"
matrice::matrice(){};
matrice:: matrice(int rows,int cols): rows{rows}, cols{cols} {}
matrice::matrice(int rows, int cols, double val): rows{rows},cols{cols} {
    if (val!=0.0){
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                data[std::make_pair(i, j)]=val;
            }
        }

    }

}
matrice::matrice(const matrice& other) : data(other.data), rows(other.rows), cols(other.cols) {}
int matrice::getNbRows(){
    return rows;
}
int matrice::getNbCols(){
    return cols;
}
Vecteur<double> matrice::matrix_vector(const Vecteur<double> &v) const
{
    Vecteur<double> result(rows, 0);
    for (const auto &entry : data)
    {
        int i = entry.first.first;
        int j = entry.first.second;
        result[i] += entry.second * v[j];
    }
    return result;
}

void matrice::print(std::ostream &out) const
{
    for (int i = 0; i < rows; i++)
    {
        out << "(";
        for (int j = 0; j < cols; j++)
        {
            if(j<cols-1)out << val(i, j) << " ";
            else out<<val(i, j);
        }
        out << ")\n";
    }
}

std::ostream &operator<<(std::ostream &out, const matrice &mat)
{
    mat.print(out);
    return out;
}

matrice &matrice::operator=(const matrice &other)
{
    if (this != &other)
    {
        data = other.data;
        rows = other.rows;
        cols = other.cols;
    }
    return *this;
}

matrice &matrice::operator+=(const matrice &mat)
{
    for (const auto &entry : mat.data)
    {
        auto it = data.find(entry.first);
        if (it != data.end())
        {
            double sum = it->second + entry.second;
            if (sum != 0.0)
            {
                it->second = sum;
            }
            else
            {
                data.erase(it);
            }
        }
        else
        {
            if (entry.second != 0.0)
            {
                data[entry.first] = entry.second;
            }
        }
    }
    return *this;
}

matrice &matrice::operator*=(const double alpha)
{
    for (auto &entry : data)
    {
        entry.second *= alpha;
        if (entry.second == 0.0)
        {
            data.erase(entry.first);
        }
    }
    return *this;
}

matrice &matrice::operator-=(const matrice &mat)
{
    for (const auto &entry : mat.data)
    {
        auto it = data.find(entry.first);
        if (it != data.end())
        {
            double diff = it->second - entry.second;
            if (diff != 0.0)
            {
                it->second = diff;
            }
            else
            {
                data.erase(it);
            }
        }
        else
        {
            if (-entry.second != 0.0)
            {
                data[entry.first] = -entry.second;
            }
        }
    }
    return *this;
}

matrice &matrice::operator/=(const double alpha)
{
    if (alpha == 0)
    {
        std::cout << "Division by zero error!!" << std::endl;
        exit(-1);
    }
    else
    {
        *this *= 1.0 / alpha;
    }
    return *this;
}

matrice operator+(const matrice &mat1, const matrice &mat2)
{
    matrice res = mat1;
    return res += mat2;
}

matrice operator-(const matrice &mat1, const matrice &mat2)
{
    matrice res = mat1;
    return res -= mat2;
}

matrice operator*(double alpha, const matrice &mat)
{
    matrice res = mat;
    return res *= alpha;
}

Vecteur<double> operator*(const matrice &mat, const Vecteur<double> &vect){
  return mat.matrix_vector(vect);
}

matrice operator/(const matrice &mat, double alpha)
{
    matrice res = mat;
    return res /= alpha;
}

double matrice::val(int i, int j) const
{
    auto it = data.find(std::make_pair(i, j));
    if (it != data.end())
    {
        return it->second;
    }
    return 0.0;
}
double matrice::operator()(int row, int col) const
{
    return val(row, col);
}

void matrice::set(int row, int col, double val)
{
    if (val == 0.0)
    {
        auto it = data.find(std::make_pair(row, col));
        if (it != data.end())
        {
            data.erase(it);
        }
    }
    else
    {
        data[std::make_pair(row, col)] = val;
    }
}

std::pair<matrice, matrice> matrice::factorisationLU() const
{
    matrice L(rows, cols), U(rows, cols);
    for (int i = 0; i < rows; i++)
    {
        L.set(i, i, 1); // Initialize diagonal of L with 1
        for (int j = i; j < cols; j++)
        {
            double sum = 0.0;
            for (int k = 0; k < i; k++)
            {
                sum += L(i, k) * U(k, j);
            }
            U.set(i, j, (*this)(i, j) - sum); // Update U using L and previously computed U values
        }
        for (int j = i + 1; j < rows; j++)
        {
            double sum = 0.0;
            for (int k = 0; k < i; k++)
            {
                sum += L(j, k) * U(k, i);
            }
            L.set(j, i, ((*this)(j, i) - sum) / U(i, i)); // Update L using previously computed L values and U
        }
    }
    return std::make_pair(L, U);
}

Vecteur<double> resoudre(const matrice &A,const Vecteur<double> &b)
{
    std::pair< matrice,matrice> result_ = A.factorisationLU();
    matrice L=result_.first;
    matrice U=result_.second;
    int n = L.getNbRows();
    Vecteur<double> y(n); // Pour stocker les solutions de Ly=b
    Vecteur<double> x(n); // Pour stocker les solutions de Ux=y

    // Résolution de Ly=b (système triangulaire inférieur)
    for (int i = 0; i < n; ++i)
    {
        double sum = 0.0;
        for (int j = 0; j < i; ++j)
        {
            sum += L.val(i, j) * y[j];
        }
        y[i] = (b[i] - sum) / L.val(i, i);
    }

    // Résolution de Ux=y (système triangulaire supérieur)
    for (int i = n - 1; i >= 0; --i)
    {
        double sum = 0.0;
        for (int j = i + 1; j < n; ++j)
        {
            sum += U.val(i, j) * x[j];
        }
        x[i] = (y[i] - sum) / U.val(i, i);
    }

    return x;
}

Vecteur<double> matrice::getRow(int row){
    Vecteur<double> v(cols, 0);
    for (int j = 0; j < cols; j++)
    {
        v[j] = val(row, j);
    }
    return v;

}

void matrice::setRow(int row,const Vecteur <double> &v){
    for (int j = 0; j < cols; j++)
    {
        set(row, j, v[j]);
    }
}