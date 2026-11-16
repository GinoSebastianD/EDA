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


};
