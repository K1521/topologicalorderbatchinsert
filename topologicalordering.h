#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>




template<typename T>
class CycleDetectedException : public std::exception {
private:
    std::vector<std::pair<T, T>> cycleEdges;

public:
    CycleDetectedException(const std::vector<std::pair<T, T>>& cycleEdges);
    const char* what() const noexcept override;
    const std::vector<std::pair<T, T>>& getCycleEdges() const;
};

template <typename T>
class Node {
private:
    bool vacant = false;
    bool onStack = false;
    int ord;
    std::vector<Node<T>*> children;
    T value;

    Node(int ord, T value);

    template<typename U>
    friend class topologicalordering;
};

template <typename T>
struct Edge {
    Node<T>* start;
    Node<T>* end;

    Edge(Node<T>* start, Node<T>* end);

    template<typename U>
    friend class topologicalordering;

    template<typename U>
    friend class NodeHandle;
};

template <typename T>
class topologicalordering;

template <typename T>
class NodeHandle {
    topologicalordering<T>* owner;
    Node<T>* node;

public:
    NodeHandle(topologicalordering<T>* topoOrder, Node<T>* node);
    NodeHandle();
    void addedge(NodeHandle<T>& other);
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
    
        
        size_t size() const;
        ~topologicalordering();

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


#include "topologicalordering.cpp"