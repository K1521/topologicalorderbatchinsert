#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>



template <typename T>
class topologicalordering;  // Forward declaration

template <typename T>
class NodeHandle;

template<typename T>
class CycleDetectedException : public std::exception {
    private:
        std::vector<std::pair<T, T>> cycleEdges;

    public:
        CycleDetectedException(const std::vector<std::pair<T, T>>& cycleEdges) : cycleEdges(cycleEdges) {}

        const char* what() const noexcept override {
            return "Graph contains a cycle";
        }

        const std::vector<std::pair<T, T>>& getCycleEdges() const {
            return cycleEdges;
        }
};

template <typename T>
class Node {

    bool vacant = false;
    bool onStack = false;
    int ord;
    std::vector<Node<T>*> children;
    T value;

    Node(int ord,T value) : ord(ord), value(value) {}

    friend class topologicalordering<T>;
};

template <typename T>
struct Edge {

    Node<T>* start;
    Node<T>* end;

    Edge(Node<T>* start, Node<T>* end) : start(start), end(end) {}
    friend class topologicalordering<T>;
    friend class NodeHandle<T>;


};

template <typename T>
class NodeHandle {
    
    topologicalordering<T> *owner;
    Node<T> *node;

    public:
    NodeHandle(topologicalordering<T> *topoOrder, Node<T> *node): owner(topoOrder), node(node) {}
    //friend class topologicalordering<T>;

    
    NodeHandle():owner(nullptr), node(nullptr){}
    //only here so we can use a presized vector of this type
    //there is probably a better way


    void addedge(NodeHandle<T> &other){
        if(owner!=other.owner)
            throw std::runtime_error("Cannot add edge between nodes from different topological orderings.");
        owner->addedge(Edge(node,other.node));
    }
};



template <typename T>
class topologicalordering{
    friend class NodeHandle<T>;

    private:
        using Qtype = std::vector<std::pair<Node<T>*, int>>;
        std::vector<Node<T>*> ordinv;
        std::vector<Edge<T>> newedges;
        std::unordered_map< T, Node<T>* > ValueToNode;
        int usednodes=0;
        
        
    public:        
    
        
        size_t size() const {
            return ordinv.size();
        }

        ~topologicalordering() {
            for (Node<T>* node : ordinv) {
                delete node;
            }
        }

        bool validate();

        NodeHandle<T> addNode(T value);

        void reserve(size_t n) ;

        void addedge(T start,T end);

        size_t size() ;

        T operator[](size_t index) const ;

        T at(size_t index) const ;
        
        void insertedges();

    private:


        Node<T>* getOrCreateNode(T value) ;

 
        inline void markNodeAsUsed(Node<T>& nodeptr) ;

        
        void addedge(Edge<T> newedge);


        inline void allocate(Node<T>& v,int i);

       
        void shift(int frontidx, Qtype& Q) ;


        void dfs(Node<T>& v , int ub,Qtype& Q) ;

        void discover(typename std::vector<Edge<T>>::iterator start, 
            typename std::vector<Edge<T>>::iterator end, 
            Qtype &Q, 
            std::vector<std::pair<T, T>> &invallidedges, 
            int lb, int ub);



};