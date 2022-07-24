// YAL zeldan

#ifndef CXM_STATS_HXX
#define CXM_STATS_HXX

#include "defs.hxx"
#include "types.hxx"
#include "linear3d.hxx"

#undef min
#undef max

CXM_NS_BEGIN


class Stat
{
public:

    enum Compute // richiede di calcolare solo alcune informazioni
    {
        SUM = 1, // somma
        SUM2 = 2, // somma dei quadrati
        MINMAX = 4, // minimo e massimo

        SIGMA = SUM | SUM2,
        ALL = SIGMA | MINMAX
    };

    explicit Stat(Compute);

    void add(double); /// aggiunge un valore al set
    int count() const; /// numero di valori
    double sum() const; /// somma
    double sum2() const; /// somma dei quadrati
    double average() const; /// media
    double average2() const; /// media dei quadrati
    double min() const; /// minimo
    double max() const; /// massimo
    double mid() const; /// punto medio (massimo+minimo)/2
    double sigma() const; /// deviazione standard (non campionaria)
    double delta() const; /// estenzione (massimo-minimo)
    int nMin(); /// indice del primo valore minimo
    int nMax(); /// indice del primo valore massimo

    void clear(); /// cancella tutti i valori accumulati



protected:
    int _count;
    double _sum;
    double _qsum;
    double _min;
    double _max;
    int _nMin;
    int _nMax;
    Compute _ops;

};

class Stat2D
{
public:
    Stat2D();

    void add(const Vector2D &); // aggiunge un vettore al set
    int count() const; // numero di vettori
    const Vector2D sum() const; // somma di tutti i vettori
    double sum2() const; // somme dei quadrati delle norme di tutti i vettori
    const Vector2D average() const; // vettore medio (Sum()/Count())
    double average2() const; // media dei quadrati delle norme dei vettori
    double sigma() const; // devizione standard (vedi sorgente per formula)

    void clear(); // cancella tutti i vettori accumulati

protected:
    int _count;
    Vector2D _sum;
    double _qsum;

};

class Stat3D
{
public:

    Stat3D();

    void add(const Vector3D &); // aggiunge un vettore al set
    int count() const; // numero di vettori
    const Vector3D sum() const; // somma di tutti i vettori
    double sum2() const; // somme dei quadrati delle norme di tutti i vettori
    const Vector3D average() const; // vettore medio (Sum()/Count())
    double average2() const; // media dei quadrati delle norme dei vettori
    double sigma() const; // devizione standard (vedi sorgente per formula)

    void clear(); // cancella tutti i vettori accumulati

protected:
    int _count;
    Vector3D _sum;
    double _qsum;


};


CXM_NS_END

#endif

//.
