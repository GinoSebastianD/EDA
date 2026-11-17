#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
#include <memory>
#include <numeric>
#include <iomanip>


static constexpr int DIMENSIONS = 2;   
static constexpr int M = 4;   
static constexpr int m = 2;   
static constexpr int p = 1;   

static_assert(m >= 2 && m <= M / 2, "m must satisfy 2 <= m <= M/2");
static_assert(p >= 1 && p < M - m + 1, "p must be in [1, M-m]");

using namespace std;
using Coord = double;

struct Rectangle {
    std::array<Coord, DIMENSIONS> lo; // lower corner
    std::array<Coord, DIMENSIONS> hi; // upper corner

    Rectangle() {
        lo.fill(std::numeric_limits<Coord>::max());
        hi.fill(-std::numeric_limits<Coord>::max());
    }

    Rectangle(std::array<Coord, DIMENSIONS> lo_,
        std::array<Coord, DIMENSIONS> hi_)
        : lo(lo_), hi(hi_) {
    }

    // Area of the rectangle
    Coord area() const {
        Coord a = 1.0;
        for (int i = 0; i < DIMENSIONS; ++i)
            a *= std::max(Coord(0), hi[i] - lo[i]);
        return a;
    }

    // Margin = sum of lengths of edges (used in split criterion, §3.1)
    Coord margin() const {
        Coord s = 0;
        for (int i = 0; i < DIMENSIONS; ++i)
            s += hi[i] - lo[i];
        return s;
    }

    // Overlap between this and r
    Coord overlap(const Rectangle& r) const {
        Coord o = 1.0;
        for (int i = 0; i < DIMENSIONS; ++i) {
            Coord len = std::min(hi[i], r.hi[i]) - std::max(lo[i], r.lo[i]);
            if (len <= 0) return 0.0;
            o *= len;
        }
        return o;
    }

    // Area enlargement needed to include r
    Coord enlargement(const Rectangle& r) const {
        return combined(r).area() - area();
    }

    // MBR of this and r
    Rectangle combined(const Rectangle& r) const {
        Rectangle res;
        for (int i = 0; i < DIMENSIONS; ++i) {
            res.lo[i] = std::min(lo[i], r.lo[i]);
            res.hi[i] = std::max(hi[i], r.hi[i]);
        }
        return res;
    }

    // Center coordinate along axis i
    Coord center(int i) const { return (lo[i] + hi[i]) / 2.0; }

    bool contains(const Rectangle& r) const {
        for (int i = 0; i < DIMENSIONS; ++i)
            if (r.lo[i] < lo[i] || r.hi[i] > hi[i]) return false;
        return true;
    }

    bool intersects(const Rectangle& r) const {
        for (int i = 0; i < DIMENSIONS; ++i)
            if (lo[i] > r.hi[i] || hi[i] < r.lo[i]) return false;
        return true;
    }

    void print() const {
        std::cout << "[";
        for (int i = 0; i < DIMENSIONS; ++i) {
            if (i) std::cout << " x ";
            std::cout << "[" << lo[i] << "," << hi[i] << "]";
        }
        std::cout << "]";
    }
};
struct Node;

struct Entry
{
    Rectangle mbr;
    int id = -1;
    Node* child = nullptr;


    bool isleaf() const {
        return child == nullptr;
    }


};

struct Node
{
    bool leaf = true;
    int level = 0;
    Node* parent = nullptr;
    vector<Entry> entradas;

    int size() const {
        return static_cast<int>(entradas.size());
    }

    Rectangle mbr() const {
        Rectangle bb;
        for (auto & e : entradas)
        {
            bb = bb.combined(e.mbr);
        }
        return bb;
    }

};


class Rstartree
{
public:
    Rstartree() {
        root_ = newNode(true, 0);
    }
	

    void Insert(const Rectangle& r, int dataid) {
        overflowlevel_.assign(MaxLevel() + 2 ,false);
        Entry e;
        e.id = dataid;
        e.mbr = r;

        Insert_entrada(e, 0);

    }

   



private:
	Node* root_;
    vector<bool> overflowlevel_;

    Node* newNode(bool leaf, int level) {
        Node* n = new Node;
        n->leaf = leaf;
        n->level = level;
        return n;
    }

    int MaxLevel() const {
        int cont = 0;
        Node* act = root_;
        while (!act->leaf)
        {
            act = act->entradas[0].child;
            cont++;
        }
        return cont;
    }



    void Insert_entrada(const Entry& e , int level) {
        Node* N = ChooseSubtree(e.mbr, level);
        N->entradas.push_back(e);

        if (e.child)
        {
            e.child->parent = N;
        }
        Node* NN = nullptr;
        if (N->size() > M)
        {
            NN = OverflowTreatment(N,level);
        }

    }

    Node* OverflowTreatment(Node* N, int level) {

        if (static_cast<int>(overflowlevel_.size()) <= level  )
        {
            overflowlevel_.resize(level + 1, false);
        }

        if (N != root_ && !overflowlevel_[level])
        {
            // Marcar antes de reinsertar: las llamadas recursivas deben
            // saber que este nivel ya utilizó su reinserción.
            overflowlevel_[level] = true;
            Reinsert(N, level);
            return nullptr;
             
        }

        return Split(N);

    }
    
    void Reinsert(Node* N , int level) {
            
    }


    Node* ChooseSubtree(const Rectangle& r , int targetlevel) {
        Node* N = root_;
        
        //al inicio de insertar , no entrara al while pq estamos en nivel 0 ps
        while (N->level != targetlevel) // si es nodo interno entra 
        {
            //si los hijos de N son hojas
            bool loshijossonhoja = (N->level == targetlevel + 1);

            if (loshijossonhoja)
            {
                Coord bestOverlap = numeric_limits<Coord>::max();
                Coord bestAreaaumento = numeric_limits<Coord>::max();
                Coord bestAreaactual = numeric_limits<Coord>::max();
                Entry* chosen = nullptr;

                vector<Rectangle> rec;
                for (auto& e : N->entradas)
                {
                     rec.push_back(e.mbr);
                }

                for (int i = 0 ; i < N->size(); i++){

                    Entry& e = N->entradas[i];
                    Rectangle enlarged = e.mbr.combined(r);

                    Coord obefore = 0;
                    for (int j = 0 ; j < N->size(); j++)
                    {
                        if (j!=i)
                        {
                            obefore = obefore + e.mbr.overlap(rec[j]);
                        }
                    }
                    Coord oafter = 0;
                    for (int j = 0; j < N->size(); j++)
                    {
                        if (j!=i)
                        {
                            oafter = oafter + enlarged.overlap(rec[j]);
                        }
                    }

                    Coord overlapminimo = oafter - obefore;
                    Coord areaminima = e.mbr.enlargement(r);
                    Coord areaactual = e.mbr.area();

                    if (overlapminimo <bestOverlap || (overlapminimo == bestOverlap && areaminima < bestAreaaumento) || (overlapminimo == bestOverlap && areaminima == bestAreaaumento
                        && areaactual < bestAreaactual)   )
                    {
                        bestOverlap = overlapminimo;
                        bestAreaaumento = areaminima;
                        bestAreaactual = areaactual;
                        chosen = &e;
                    }


                }

                N = chosen->child;
            }
            else
            {

                Coord bestAumentoArea = numeric_limits<Coord>::max();
                Coord bestAreaActual = numeric_limits<Coord>::max();
                Entry* chosen = nullptr;

                for (auto& e : N->entradas)
                {
                    Coord enl = e.mbr.enlargement(r);
                    Coord area = e.mbr.area();
                    if (enl < bestAumentoArea || (enl == bestAumentoArea && area < bestAreaActual))
                    {
                        bestAumentoArea = enl;
                        bestAreaActual = area;
                        chosen = &e;
                    }

                }
                N = chosen->child;
            }
        }
        return N;



    }


    Node* Split(Node* N) {


    }







};
