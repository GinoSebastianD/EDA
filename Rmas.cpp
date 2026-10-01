#include <iostream>
#include <vector>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <iomanip>

using namespace std;

// ============================================================
// RECTANGULO
// ============================================================

struct Rect {
    double x1, y1; // esquina inferior izquierda
    double x2, y2; // esquina superior derecha

    Rect() = default;

    Rect(double x1, double y1, double x2, double y2)
        : x1(x1), y1(y1), x2(x2), y2(y2) {}

    // --------------------------------------------------------
    // ¿Dos rectángulos se intersectan?
    // --------------------------------------------------------
    bool overlaps(const Rect& other) const {
        return !(x2 < other.x1 ||
                 other.x2 < x1 ||
                 y2 < other.y1 ||
                 other.y2 < y1);
    }

    // --------------------------------------------------------
    // Área
    // --------------------------------------------------------
    double area() const {
        return max(0.0, x2 - x1) *
               max(0.0, y2 - y1);
    }

    // --------------------------------------------------------
    // Combinar dos rectángulos -> MBR
    // --------------------------------------------------------
    static Rect combine(const Rect& a, const Rect& b) {
        return Rect(
            min(a.x1, b.x1),
            min(a.y1, b.y1),
            max(a.x2, b.x2),
            max(a.y2, b.y2)
        );
    }

    void print() const {
        cout << "[("
             << x1 << ", " << y1 << ") -> ("
             << x2 << ", " << y2 << ")]";
    }
};


// ============================================================
// OBJETO ORIGINAL
// ============================================================

struct Object {
    int id;
    Rect rect;
};


// ============================================================
// FRAGMENTO
//
// Un mismo objeto puede aparecer varias veces.
//
// Ejemplo:
//
//        objeto 5
//    +---------------+
//    |               |
// ---+--------|------+---- corte
//    |    5a  | 5b   |
//    +--------|------+
//
// Ambos fragmentos siguen teniendo id = 5.
// ============================================================

struct Fragment {
    int id;
    Rect rect;
};


// ============================================================
// NODO R+
// ============================================================

struct Node {

    bool leaf = true;

    // Región cubierta por este nodo.
    Rect region;

    // Solo si es hoja
    vector<Fragment> entries;

    // Solo si es nodo interno
    vector<unique_ptr<Node>> children;
};


// ============================================================
// R+ TREE
// ============================================================

class RPlusTree {

private:

    // Máximo de entradas deseado por hoja.
    int M;

    // Objetos originales.
    vector<Object> objects;

    unique_ptr<Node> root;

    static constexpr int MAX_DEPTH = 50;


    // ========================================================
    // MBR DE UN CONJUNTO DE FRAGMENTOS
    // ========================================================

    Rect calculateMBR(const vector<Fragment>& entries) const {

        Rect result = entries[0].rect;

        for (size_t i = 1; i < entries.size(); i++) {
            result = Rect::combine(result, entries[i].rect);
        }

        return result;
    }


    // ========================================================
    // INFORMACION DE UN SPLIT
    // ========================================================

    struct SplitInfo {

        bool valid = false;

        // 0 = eje X
        // 1 = eje Y
        int axis = 0;

        double cut = 0;

        int splits = 0;

        int leftCount = 0;
        int rightCount = 0;
    };


    // ========================================================
    // EVALUAR UN CORTE
    // ========================================================

    SplitInfo evaluateCut(
        const vector<Fragment>& entries,
        int axis,
        double cut
    ) const {

        SplitInfo info;

        info.axis = axis;
        info.cut = cut;

        for (const Fragment& e : entries) {

            if (axis == 0) {

                // --------------------
                // CORTE VERTICAL
                // --------------------

                bool left =
                    e.rect.x1 < cut;

                bool right =
                    e.rect.x2 > cut;

                if (left)
                    info.leftCount++;

                if (right)
                    info.rightCount++;

                if (left && right)
                    info.splits++;
            }
            else {

                // --------------------
                // CORTE HORIZONTAL
                // --------------------

                bool bottom =
                    e.rect.y1 < cut;

                bool top =
                    e.rect.y2 > cut;

                if (bottom)
                    info.leftCount++;

                if (top)
                    info.rightCount++;

                if (bottom && top)
                    info.splits++;
            }
        }


        // Deben existir elementos a ambos lados.
        if (info.leftCount == 0 ||
            info.rightCount == 0) {

            return info;
        }

        /*
         * También queremos que ambos subconjuntos
         * sean menores que el original.
         *
         * Evita recursión infinita, por ejemplo:
         *
         * +-------------------------+
         * | A                       |
         * | B                       |
         * | C                       |
         * +-------------------------+
         *
         * si TODOS cruzan exactamente el mismo corte.
         */

        if (info.leftCount >= (int) entries.size() ||
            info.rightCount >= (int) entries.size()) {

            return info;
        }

        info.valid = true;

        return info;
    }


    // ========================================================
    // BUSCAR MEJOR PARTICION
    // ========================================================

    SplitInfo choosePartition(
        const vector<Fragment>& entries
    ) const {

        SplitInfo best;

        int bestSplits =
            numeric_limits<int>::max();

        int bestBalance =
            numeric_limits<int>::max();


        // ====================================================
        // CANDIDATOS EN X
        // ====================================================

        vector<double> xs;

        for (const Fragment& e : entries) {

            xs.push_back(e.rect.x1);
            xs.push_back(e.rect.x2);
        }


        sort(xs.begin(), xs.end());

        xs.erase(
            unique(xs.begin(), xs.end()),
            xs.end()
        );


        for (size_t i = 1; i < xs.size(); i++) {

            double cut =
                (xs[i - 1] + xs[i]) / 2.0;

            SplitInfo current =
                evaluateCut(entries, 0, cut);

            if (!current.valid)
                continue;

            int balance =
                max(current.leftCount,
                    current.rightCount);

            /*
             * Criterio:
             *
             * 1. Minimizar splits
             * 2. Balancear cantidad
             */

            if (current.splits < bestSplits ||
               (current.splits == bestSplits &&
                balance < bestBalance)) {

                best = current;

                bestSplits = current.splits;
                bestBalance = balance;
            }
        }


        // ====================================================
        // CANDIDATOS EN Y
        // ====================================================

        vector<double> ys;

        for (const Fragment& e : entries) {

            ys.push_back(e.rect.y1);
            ys.push_back(e.rect.y2);
        }


        sort(ys.begin(), ys.end());

        ys.erase(
            unique(ys.begin(), ys.end()),
            ys.end()
        );


        for (size_t i = 1; i < ys.size(); i++) {

            double cut =
                (ys[i - 1] + ys[i]) / 2.0;

            SplitInfo current =
                evaluateCut(entries, 1, cut);

            if (!current.valid)
                continue;

            int balance =
                max(current.leftCount,
                    current.rightCount);

            if (current.splits < bestSplits ||
               (current.splits == bestSplits &&
                balance < bestBalance)) {

                best = current;

                bestSplits = current.splits;
                bestBalance = balance;
            }
        }

        return best;
    }


    // ========================================================
    // DIVIDIR LOS RECTANGULOS SEGUN EL CORTE
    // ========================================================

    void splitEntries(
        const vector<Fragment>& entries,
        const SplitInfo& split,
        vector<Fragment>& A,
        vector<Fragment>& B
    ) const {

        for (const Fragment& f : entries) {

            Rect r = f.rect;


            // =================================================
            // CORTE EN X
            // =================================================

            if (split.axis == 0) {

                double cut = split.cut;


                // completamente izquierda
                if (r.x2 <= cut) {

                    A.push_back(f);
                }

                // completamente derecha
                else if (r.x1 >= cut) {

                    B.push_back(f);
                }

                // ---------------------------------------------
                // ATRAVIESA EL CORTE
                // ---------------------------------------------
                else {

                    Fragment left = f;
                    Fragment right = f;

                    /*
                     * Rectángulo:
                     *
                     *  +--------------------+
                     *  |          |         |
                     *  |          |         |
                     *  +--------------------+
                     *             ^
                     *            cut
                     *
                     * queda:
                     *
                     *  +----------+
                     *  |          |
                     *  +----------+
                     *
                     *             +---------+
                     *             |         |
                     *             +---------+
                     */

                    left.rect.x2 = cut;

                    right.rect.x1 = cut;

                    A.push_back(left);
                    B.push_back(right);
                }
            }


            // =================================================
            // CORTE EN Y
            // =================================================

            else {

                double cut = split.cut;


                // completamente abajo
                if (r.y2 <= cut) {

                    A.push_back(f);
                }

                // completamente arriba
                else if (r.y1 >= cut) {

                    B.push_back(f);
                }

                // atraviesa el corte
                else {

                    Fragment bottom = f;
                    Fragment top = f;

                    bottom.rect.y2 = cut;

                    top.rect.y1 = cut;

                    A.push_back(bottom);
                    B.push_back(top);
                }
            }
        }
    }


    // ========================================================
    // CONSTRUIR R+ RECURSIVAMENTE
    // ========================================================

    unique_ptr<Node> build(
        const vector<Fragment>& entries,
        int depth
    ) {

        if (entries.empty())
            return nullptr;


        auto node =
            make_unique<Node>();

        node->region =
            calculateMBR(entries);


        // ====================================================
        // CREAR HOJA
        // ====================================================

        if ((int) entries.size() <= M ||
            depth >= MAX_DEPTH) {

            node->leaf = true;
            node->entries = entries;

            return node;
        }


        // ====================================================
        // ELEGIR PARTICION
        // ====================================================

        SplitInfo split =
            choosePartition(entries);


        /*
         * Caso degenerado:
         *
         * No encontramos una partición que reduzca
         * los conjuntos.
         *
         * Por ejemplo, muchos rectángulos exactamente
         * iguales.
         */

        if (!split.valid) {

            node->leaf = true;
            node->entries = entries;

            return node;
        }


        vector<Fragment> left;
        vector<Fragment> right;


        splitEntries(
            entries,
            split,
            left,
            right
        );


        // ====================================================
        // NODO INTERNO
        // ====================================================

        node->leaf = false;


        auto child1 =
            build(left, depth + 1);

        auto child2 =
            build(right, depth + 1);


        if (child1)
            node->children.push_back(
                move(child1)
            );

        if (child2)
            node->children.push_back(
                move(child2)
            );


        return node;
    }


    // ========================================================
    // RECONSTRUIR ARBOL
    // ========================================================

    void rebuild() {

        if (objects.empty()) {

            root.reset();
            return;
        }


        vector<Fragment> fragments;


        for (const Object& obj : objects) {

            fragments.push_back({
                obj.id,
                obj.rect
            });
        }


        root =
            build(fragments, 0);
    }


    // ========================================================
    // SEARCH RECURSIVO
    // ========================================================

    void searchRecursive(
        const Node* node,
        const Rect& query,
        unordered_set<int>& result
    ) const {

        if (node == nullptr)
            return;


        /*
         * Si la región completa del nodo
         * no toca la consulta,
         * no necesitamos entrar.
         */

        if (!node->region.overlaps(query))
            return;


        // ====================================================
        // HOJA
        // ====================================================

        if (node->leaf) {

            for (const Fragment& entry :
                 node->entries) {

                if (entry.rect.overlaps(query)) {

                    result.insert(entry.id);
                }
            }

            return;
        }


        // ====================================================
        // NODO INTERNO
        // ====================================================

        for (const auto& child :
             node->children) {

            if (child->region.overlaps(query)) {

                searchRecursive(
                    child.get(),
                    query,
                    result
                );
            }
        }
    }


    // ========================================================
    // PRINT
    // ========================================================

    void printRecursive(
        const Node* node,
        int level
    ) const {

        if (!node)
            return;


        string tabs(
            level * 4,
            ' '
        );


        cout << tabs;

        if (node->leaf)
            cout << "HOJA ";
        else
            cout << "INTERNO ";


        node->region.print();

        cout << "\n";


        // ====================================================
        // HOJA
        // ====================================================

        if (node->leaf) {

            for (const Fragment& f :
                 node->entries) {

                cout
                    << tabs
                    << "    ID "
                    << f.id
                    << " -> ";

                f.rect.print();

                cout << "\n";
            }
        }


        // ====================================================
        // INTERNO
        // ====================================================

        else {

            for (const auto& child :
                 node->children) {

                printRecursive(
                    child.get(),
                    level + 1
                );
            }
        }
    }


public:

    // ========================================================
    // CONSTRUCTOR
    // ========================================================

    explicit RPlusTree(int capacity = 3)
        : M(capacity) {}


    // ========================================================
    // INSERT
    // ========================================================

    void insert(
        int id,
        double x1,
        double y1,
        double x2,
        double y2
    ) {

        if (x1 > x2)
            swap(x1, x2);

        if (y1 > y2)
            swap(y1, y2);


        Rect r(
            x1,
            y1,
            x2,
            y2
        );


        // Evitar IDs repetidos
        for (const Object& obj : objects) {

            if (obj.id == id) {

                cout
                    << "El ID "
                    << id
                    << " ya existe.\n";

                return;
            }
        }


        objects.push_back({
            id,
            r
        });


        rebuild();
    }


    // ========================================================
    // REMOVE
    // ========================================================

    bool remove(int id) {

        auto it =
            remove_if(
                objects.begin(),
                objects.end(),
                [id](const Object& obj) {
                    return obj.id == id;
                }
            );


        if (it == objects.end())
            return false;


        objects.erase(
            it,
            objects.end()
        );


        rebuild();

        return true;
    }


    // ========================================================
    // SEARCH
    // ========================================================

    vector<int> search(
        double x1,
        double y1,
        double x2,
        double y2
    ) const {

        Rect query(
            min(x1, x2),
            min(y1, y2),
            max(x1, x2),
            max(y1, y2)
        );


        unordered_set<int> uniqueResults;


        searchRecursive(
            root.get(),
            query,
            uniqueResults
        );


        vector<int> result(
            uniqueResults.begin(),
            uniqueResults.end()
        );


        sort(
            result.begin(),
            result.end()
        );


        return result;
    }


    // ========================================================
    // PRINT
    // ========================================================

    void print() const {

        cout
            << "\n========== R+ TREE ==========\n";


        if (!root) {

            cout
                << "Arbol vacio\n";

            return;
        }


        printRecursive(
            root.get(),
            0
        );


        cout
            << "==============================\n";
    }
};


// ============================================================
// MAIN
// ============================================================

int main() {

    /*
     * Capacidad aproximada por hoja = 3
     */

    RPlusTree tree(3);


    // ========================================================
    // INSERTAR RECTANGULOS
    // ========================================================

    tree.insert(
        1,
        1, 1,
        4, 4
    );

    tree.insert(
        2,
        5, 1,
        8, 4
    );

    tree.insert(
        3,
        2, 5,
        6, 8
    );

    tree.insert(
        4,
        7, 5,
        10, 8
    );


    /*
     * Este rectángulo es interesante
     * porque atraviesa varias regiones.
     *
     * El R+ puede terminar dividiéndolo
     * en diferentes fragmentos.
     */

    tree.insert(
        5,
        3, 2,
        9, 7
    );


    tree.insert(
        6,
        11, 1,
        13, 4
    );


    // ========================================================
    // MOSTRAR ARBOL
    // ========================================================

    tree.print();


    // ========================================================
    // CONSULTA
    //
    // ventana:
    //
    // (4,3) -------- (7,6)
    //
    // ========================================================

    cout
        << "\nConsulta [(4,3) -> (7,6)]\n";


    vector<int> resultado =
        tree.search(
            4, 3,
            7, 6
        );


    cout
        << "Objetos encontrados: ";


    for (int id : resultado) {

        cout
            << id
            << " ";
    }


    cout << "\n";


    // ========================================================
    // ELIMINAR
    // ========================================================

    cout
        << "\nEliminando objeto 3...\n";


    tree.remove(3);


    tree.print();


    return 0;
}
