#include <vector>
//#include <iterator>  
#include <unordered_map>  
#include <algorithm>
#include <stdexcept>


template <typename T>
class topologicalordering;  // Forward declaration

template <typename T>
class Node {
private:
    bool vacant = false;
    bool onStack = false;
    int ord = -1;
    std::vector<Node<T>*> children;
    T value;

    //Node(int ord,T value) : ord(ord), value(value) {}

    friend class topologicalordering<T>;
};

template <typename T>
struct Edge {
private:
    Node<T>* start;
    Node<T>* end;

    Edge(Node<T>* start, Node<T>* end) : start(start), end(end) {}
    friend class topologicalordering<T>;
};


template <typename T>
class topologicalordering {
private:
    //vector<Node*> nodes=vector<Node*>();
    std::vector<Node<T>*> ordinv;
    std::vector<Edge<T>> newedges;
    std::unordered_map< T, Node<T>> ValueToNode;
    int usednodes = 0;


public:
    inline std::size_t size() {
        return ordinv.size();
    }

    /*~topologicalordering(){
        for (Node<T>* node : ordinv) {
            delete node;
        }
    }*/
    void reserve(size_t n) {
        ordinv.reserve(n);
        ValueToNode.reserve(n);
    }

    bool validate() {
        //cout << ordinv.size()<< endl;
        for (int i = 0; i < ordinv.size(); i++) {
            Node<T>* n = ordinv[i];
            if (n->ord != i)
                return false;

            for (Node<T>* childnode : n->children) {
                if (n->ord >= childnode->ord)
                    return false;
            }
            if (n->vacant || n->onStack)return false;

        }
        return true;
    }

    void addNode(T value) {
        getOrCreateNode(value, false);
    }


    void addedge(T start, T end) {//todo add direct cycle detection
        addedge(Edge(getOrCreateNode(start, true), getOrCreateNode(end, true)));
    }

    size_t size() const {
        return ordinv.size();
    }

    T operator[](size_t index) const {
        return ordinv[index]->value;
    }

    T at(size_t index) const {//TODO throw index out of bounds
        return ordinv.at(index)->value;
    }

private:



    Node<T>* getOrCreateNode(T value, bool used) {
        Node<T>& node = ValueToNode[value];
        if (node.ord == -1) { // creates a node if it is not in the nodes dictionary
            node.ord = ordinv.size();
            node.value = value;
            node.children.reserve(8);
            ordinv.push_back(&node);
        }
        if (used)
            markNodeAsUsed(&node); // assuming markNodeAsUsed takes a Node<T>* as an argument
        return &node;
    }

    /*Node<T>* getOrCreateNode(T value,bool used) {
        Node<T>*& nodeptr = ValueToNode[value];
        if (nodeptr==NULL) {//creates a node if it is not in the nodes dictionary
            int ord = ordinv.size();
            nodeptr = new Node<T>(ord, value);
            nodeptr->children.reserve(8);
            ordinv.push_back(nodeptr);
        }

        if(used)
            markNodeAsUsed(nodeptr);//das sorgt dafür das zwichen benutzten nodes keine unbenutzten knoten sind (unbenutzt heisst das der knoten keine einkommenden/ausgehende kanten hat)


        return nodeptr;
    }*/

    inline void markNodeAsUsed(Node<T>* nodeptr) {
        int nodeindex = nodeptr->ord;
        if (nodeindex >= usednodes) {//wenn die node nicht schon eine usednode ist
            if (nodeindex != usednodes) {
                Node<T>*& unusednodeptr = ordinv[usednodes];//get first unusednode
                allocate(unusednodeptr, nodeindex);//swap the nodes so the node we want is the first unusednode
                allocate(nodeptr, usednodes);
            }
            usednodes++;//make the first unusednode the last usednode
        }
    }





    void addedge(Edge<T> newedge) {
        if (newedge.start->ord > newedge.end->ord) {
            newedges.push_back(newedge);
        }

        newedge.start->children.push_back(newedge.end);

    }

public:
    void insertedges() {
        if (newedges.empty()) return;

        std::sort(newedges.begin(), newedges.end(),
            [](const Edge<T>& e1, const Edge<T>& e2) {
                return e1.start->ord > e2.start->ord;
            });

        int lowerbound = ordinv.size();
        int s = 0;
        std::vector<std::pair<Node<T>*, Node<T>*>> Q;

        for (int i = 0; i < newedges.size(); i++) {
            auto [x, y] = newedges[i];
            if (x->ord < lowerbound && i != 0) {
                if (!Q.empty()) {//TODO remove after testing
                    throw std::runtime_error("Q isnt empty D:");
                }

                //Q.clear();
                discover(newedges.begin() + s, newedges.begin() + i, Q);
                shift(lowerbound, Q);
                s = i;
            }

            lowerbound = std::min(y->ord, lowerbound);
        }
        Q.clear();
        //Q.reserve(newedges.size()-s);
        discover(newedges.begin() + s, newedges.end(), Q);
        shift(lowerbound, Q);


        newedges.clear();//<-lol i forgot this line and then i spent 2 day debugging

    }
private:
    inline void allocate(Node<T>* v, int i) {
        v->ord = i;
        ordinv[i] = v;
    }


    void shift(int frontidx, std::vector<std::pair<Node<T>*, Node<T>*>>& Q) {
        int fillidx = frontidx;
        auto [v, d] = Q.back(); Q.pop_back(); //i am more familiar with pythons pop

        while (true) {
            Node<T>* frontnode = ordinv.at(frontidx);
            if (!frontnode->vacant) {//shift non vacant nodes
                allocate(frontnode, fillidx++);
            }
            else {
                frontnode->vacant = false;
            }
            while (frontnode == d)//insert nodes from Q
            {
                allocate(v, fillidx++);
                if (Q.empty())return;
                std::tie(v, d) = Q.back(); Q.pop_back();
            }
            frontidx++;
        }



    }


    void dfs(Node<T>* v, int ub, std::vector<std::pair<Node<T>*, Node<T>*>>& Q) {
        v->vacant = true;
        v->onStack = true;
        //cout << "visiting" << v->id<<endl;
        for (Node<T>* s : v->children)
        {

            if (s->onStack)
                throw std::runtime_error("cycle");
            if ((!s->vacant) && (s->ord < ub))
                dfs(s, ub, Q);
        }
        v->onStack = false;

        Q.emplace_back(v, ordinv[ub]);
    }




    void discover(typename std::vector<Edge<T>>::iterator start,
        typename std::vector<Edge<T>>::iterator end,
        std::vector<std::pair<Node<T>*, Node<T>*>>& Q) {


        /*

        sort(start,end, [](const Edge<T>& edge1, const Edge<T>& edge2) {
            return edge1.start->ord > edge2.start->ord;
        });//ich bin relativ sicher das dieser sort vermeidbar ist

        */

        //vector<pair<Node*, Node*>> Q=vector<pair<Node*, Node*>>();//TODO return Q by refference
        for (auto it = start; it != end; ++it)
        {
            Edge<T> edge = *it;
            if (!edge.end->vacant)
            {
                dfs(edge.end, edge.start->ord, Q);
            }
        }


    }

};
