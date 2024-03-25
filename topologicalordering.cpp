#include "topologicalordering.h"


template<typename T>
CycleDetectedException<T>::CycleDetectedException(const std::vector<std::pair<T, T>>& cycleEdges) 
    : cycleEdges(cycleEdges) {}

template<typename T>
const char* CycleDetectedException<T>::what() const noexcept {
    return "Graph contains a cycle";
}

template<typename T>
const std::vector<std::pair<T, T>>& CycleDetectedException<T>::getCycleEdges() const {
    return cycleEdges;
}




template<typename T>
NodeHandle<T>::NodeHandle(topologicalordering<T>* topoOrder, Node<T>* node) : owner(topoOrder), node(node) {}

template<typename T>
NodeHandle<T>::NodeHandle() : owner(nullptr), node(nullptr) {}

template<typename T>
void NodeHandle<T>::addedge(NodeHandle<T>& other) {
    if (owner != other.owner)
        throw std::runtime_error("Cannot add edge between nodes from different topological orderings.");
    owner->addedge(Edge(node, other.node));
}





template<typename T>
Node<T>::Node(int ord, T value) : ord(ord), value(value) {}



template<typename T>
Edge<T>::Edge(Node<T>* start, Node<T>* end) : start(start), end(end) {}


template<typename T>
size_t topologicalordering<T>::size() const {
    return ordinv.size();
}

template<typename T>
topologicalordering<T>::~topologicalordering() {
    for (Node<T>* node : ordinv) {
        delete node;
    }
}

template<typename T>
bool topologicalordering<T>::validate() {
    for (int i = 0; i < ordinv.size(); i++) {
        Node<T>& n = *ordinv[i];
        if (n.ord != i)
            return false;

        for (Node<T>* childnode : n.children) {
            if (n.ord >= childnode->ord)
                return false;
        }
        if (n.vacant || n.onStack)
            return false;
    }
    return true;
}

template<typename T>
NodeHandle<T> topologicalordering<T>::addNode(T value) {
    return NodeHandle(this, getOrCreateNode(value)); // Assuming getOrCreateNode function is defined elsewhere
}

template<typename T>
void topologicalordering<T>::reserve(size_t n) {
    ordinv.reserve(n);
    ValueToNode.reserve(n);
}

template<typename T>
void topologicalordering<T>::addedge(T start, T end) {
    addedge(Edge(getOrCreateNode(start), getOrCreateNode(end)));
}


template<typename T>
T topologicalordering<T>::operator[](size_t index) const {
    return ordinv[index]->value;
}

template<typename T>
T topologicalordering<T>::at(size_t index) const {
    return ordinv.at(index)->value; // TODO: Add index out of bounds handling
}




template<typename T>
Node<T>* topologicalordering<T>::getOrCreateNode(T value) {
    Node<T>*& nodeptr = ValueToNode[value];
    if (nodeptr == nullptr) { // Creates a node if it is not in the nodes dictionary
        int ord = ordinv.size();
        nodeptr = new Node<T>(ord, value);
        nodeptr->children.reserve(8);
        ordinv.push_back(nodeptr);
    }

    return nodeptr;
}

template<typename T>
void topologicalordering<T>::markNodeAsUsed(Node<T>& nodeptr) {
    int nodeindex = nodeptr.ord;
    if (nodeindex >= usednodes) { // If the node is not already a used node
        if (nodeindex != usednodes) {
            Node<T>& unusednodeptr = *ordinv[usednodes]; // Get the first unused node
            allocate(unusednodeptr, nodeindex); // Swap the nodes so the node we want is the first unused node
            allocate(nodeptr, usednodes);
        }
        usednodes++; // Make the first unused node the last used node
    }
}

template<typename T>
void topologicalordering<T>::addedge(Edge<T> newedge) {
    markNodeAsUsed(*newedge.start);

    if (newedge.start->ord > newedge.end->ord) {
        newedges.push_back(newedge);
    }

    newedge.start->children.push_back(newedge.end);
}



template<typename T>
void topologicalordering<T>::insertedges() {
    if (newedges.empty()) return;

    // sort invalidating edges into descending order by index of tail
    std::sort(newedges.begin(), newedges.end(),
            [](const Edge<T>& e1, const Edge<T>& e2) {
                return e1.start->ord > e2.start->ord;
            });

    int lowerbound = newedges[0].end->ord;
    int upperbound = newedges[0].start->ord;
    int s = 0; // start of current region
    Qtype Q;

    std::vector<std::pair<T, T>> invallidedges;

    for (int i = 1; i < newedges.size(); i++) {
        auto [x, y] = newedges[i];
        if (x->ord < lowerbound) {
            if (!Q.empty()) {
                throw std::runtime_error("Q isn't empty D:");
            }

            discover(newedges.begin() + s, newedges.begin() + i, Q, invallidedges, lowerbound, lowerbound);
            shift(lowerbound, Q);
            s = i;
            upperbound = x->ord;
        }

        lowerbound = std::min(y->ord, lowerbound);
    }

    if (!Q.empty()) {
        throw std::runtime_error("Q isn't empty D:");
    }

    discover(newedges.begin() + s, newedges.end(), Q, invallidedges, lowerbound, lowerbound);
    shift(lowerbound, Q);

    newedges.clear();

    if (!invallidedges.empty()) {
        throw CycleDetectedException<T>(invallidedges);
    }
}

template<typename T>
void topologicalordering<T>::allocate(Node<T>& v, int i) {
    v.ord = i;
    ordinv[i] = &v;
}

       
template<typename T>
void topologicalordering<T>::shift(int frontidx, Qtype& Q) {
    int fillidx = frontidx;
    auto [nodetoinsert, insertafter] = Q.back(); Q.pop_back(); //i am more familiar with pythons pop

    while (true) {
        Node<T>& frontnode = *ordinv.at(frontidx);
        if (!frontnode.vacant) { // shift non-vacant nodes
            allocate(frontnode, fillidx++);
        } else {
            frontnode.vacant = false;
        }

        // while (&frontnode==insertafter)
        while (frontidx == insertafter) { // insert nodes from Q
            allocate(*nodetoinsert, fillidx++);
            if (Q.empty()) return;
            std::tie(nodetoinsert, insertafter) = Q.back(); Q.pop_back();
        }
        frontidx++;
    }
}

template<typename T>
void topologicalordering<T>::dfs(Node<T>& v, int ub, Qtype& Q) {
    v.vacant = true;
    v.onStack = true;

    for (Node<T>* s : v.children) {
        if (s->onStack)
            throw std::runtime_error("cycle");
        if ((!s->vacant) && (s->ord < ub))
            dfs(*s, ub, Q);
    }

    v.onStack = false;

    Q.emplace_back(&v, ub);
}

template<typename T>
void topologicalordering<T>::discover(typename std::vector<Edge<T>>::iterator start,
                                      typename std::vector<Edge<T>>::iterator end,
                                      Qtype& Q,
                                      std::vector<std::pair<T, T>>& invallidedges,
                                      int lb,
                                      int ub) {
    for (auto it = start; it != end; ++it) {
        Edge<T> edge = *it;
        if (!edge.end->vacant) {
            try {
                dfs(*edge.end, edge.start->ord, Q);
            } catch (const std::runtime_error& e) {
                for (int i = lb; i < ub; ++i) ordinv[i]->onStack = false;

                while (!Q.empty()) {
                    auto [nodetoinsert, insertafter] = Q.back();
                    if (edge.start->ord != insertafter) break;
                    Q.pop_back();
                }

                invallidedges.emplace_back(edge.start->value, edge.end->value);
            }
        }
    }
}