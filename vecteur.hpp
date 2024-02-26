#ifndef VECTEUR_HPP_INCLUDED
#define VECTEUR_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <cstdlib>

template <typename T>
class Vecteur : public std::vector<T>
{
public:
    Vecteur(int d = 0, const T &x = T())
    {
        if (d > 0)
            this->assign(d, x);
    }
    Vecteur(const Vecteur& other) : Vecteur(other.size()) {
    *this = other;  // Call the copy assignment operator
}

    void print(std::ostream &out) const
    {
        int d = this->size();
        out << "(";
        for (int i = 0; i < d - 1; i++)
            out << this->at(i) << ",";
        out << this->at(d - 1) << ")";
    }

    int get_dim() const
    {
        return this->size();
    }

    const T &operator()(int i) const
    {
        return this->at(i - 1);
    }

    T &operator()(int i)
    {
        return this->at(i - 1);
    }

    Vecteur<T> &operator+=(const Vecteur<T> &v)
    {
        if (v.size() != this->size())
            exit(EXIT_FAILURE);
        auto itv = v.begin();
        for (auto it = this->begin(); it != this->end(); ++it, ++itv)
            *it += *itv;
        return *this;
    }

    Vecteur<T> &operator-=(const Vecteur<T> &v)
    {
        if (v.size() != this->size())
            exit(EXIT_FAILURE);
        auto itv = v.begin();
        for (auto it = this->begin(); it != this->end(); ++it, ++itv)
            *it -= *itv;
        return *this;
    }

    Vecteur<T> &operator*=(const T &a)
    {
        for (auto it = this->begin(); it != this->end(); ++it)
            *it *= a;
        return *this;
    }

    Vecteur<T> &operator/=(const T &a)
    {
        if (a == 0)
            exit(EXIT_FAILURE);
        for (auto it = this->begin(); it != this->end(); ++it)
            *it /= a;
        return *this;
    }

    Vecteur<T> &operator=(const Vecteur<T> &other)
    {
        if (this != &other)
        {
            this->clear();
            this->insert(this->begin(), other.begin(), other.end());
        }
        return *this;
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &out, const Vecteur<T> &u)
{
    u.print(out);
    return out;
}

template <typename T>
Vecteur<T> operator-(const Vecteur<T> &u)
{
    Vecteur<T> w = u;
    return w *= -1;
}

template <typename T>
Vecteur<T> operator+(const Vecteur<T> &u, const Vecteur<T> &v)
{
    Vecteur<T> w = u;
    return w += v;
}

template <typename T>
Vecteur<T> operator-(const Vecteur<T> &u, const Vecteur<T> &v)
{
    Vecteur<T> w = u;
    return w -= v;
}

template <typename T>
Vecteur<T> operator*(const Vecteur<T> &u, const T &a)
{
    Vecteur<T> w = u;
    return w *= a;
}

template <typename T>
Vecteur<T> operator*(const T &a, const Vecteur<T> &u)
{
    Vecteur<T> w = u;
    return w *= a;
}

template <typename T>
Vecteur<T> operator/(const Vecteur<T> &u, const T &a)
{
    Vecteur<T> w = u;
    return w /= a;
}

template <typename T>
T operator|(const Vecteur<T> &u, const Vecteur<T> &v)
{
    if (u.size() != v.size())
        exit(EXIT_FAILURE);
    T ps = T();
    auto itv = v.begin();
    for (auto it = u.begin(); it != u.end(); ++it, ++itv)
        ps += (*it) * (*itv);
    return ps;
}

#endif // VECTEUR_HPP_INCLUDED
