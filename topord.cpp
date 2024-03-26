#include <vector>
//#include <iterator>  
#include <unordered_map>  
#include <algorithm>
#include <stdexcept>




template <typename T>class topologicalordering;  // Forward declaration
template <typename T>class NodeHandle;

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

template<typename T>
class CycleDetectedException : public std::exception {
private:
    std::vector<std::pair<T, T>> cycleEdges;

public:
    CycleDetectedException(const std::vector<std::pair<T, T>>& cycleEdges):cycleEdges(cycleEdges) {};

    const char* what() const noexcept {
        return "Graph contains a cycle";
    }
    const std::vector<std::pair<T, T>>& getCycleEdges() const {
        return cycleEdges;
    }
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
    public:
    topologicalordering<T> *owner;
    Node<T> *node;
    NodeHandle(topologicalordering<T> *topoOrder, Node<T> *node): owner(topoOrder), node(node) {}
    //friend class topologicalordering<T>;

    NodeHandle():owner(nullptr), node(nullptr){}


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
        //vector<Node*> nodes=vector<Node*>();
        using Qtype = std::vector<std::pair<Node<T>*, int>>;
        std::vector<Node<T>*> ordinv;
        std::vector<Edge<T>> newedges;
        std::unordered_map< T, Node<T>* > ValueToNode;
        int usednodes=0;
        
        
    public:        
        inline std::size_t size(){
            return ordinv.size();
        }

        

        ~topologicalordering(){
            for (Node<T>* node : ordinv) {
                delete node;
            }
        }

        bool validate(){
            //cout << ordinv.size()<< endl;
            for(int i=0;i<ordinv.size();i++){
                Node<T> &n=*ordinv[i];
                if(n.ord!=i)
                    return false;
                
                for(Node<T>* childnode:n.children){
                    if(n.ord>=childnode->ord)
                    return false;
                }
                if(n.vacant||n.onStack)return false;
                    
            }
            return true;
        }

        NodeHandle<T> addNode(T value){
            return NodeHandle(this,getOrCreateNode(value));
        }

        void reserve(size_t n) {
            ordinv.reserve(n);
            ValueToNode.reserve(n);
        }

         


        void addedge(T start,T end){//todo add direct cycle detection
            addedge(Edge(getOrCreateNode(start),getOrCreateNode(end)));
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

        int ord(T node)const {//index of node in the topological order
            auto it = ValueToNode.find(node); 
            if (it != ValueToNode.end()) {
                Node<T>* nodeptr = it->second;
                return nodeptr->ord; 
            }
            return -1;  
        }


        private:


        Node<T>* getOrCreateNode(T value) {
            Node<T>*& nodeptr = ValueToNode[value];
            if (nodeptr==NULL) {//creates a node if it is not in the nodes dictionary
                int ord = ordinv.size();
                nodeptr = new Node<T>(ord, value);
                nodeptr->children.reserve(8);
                ordinv.push_back(nodeptr);
            }

            
            /*nt nodeindex=nodeptr->ord;
            if(used && nodeindex>=usednodes){
                if(nodeindex!=usednodes){
                    Node<T>*& unusednodeptr=ord[usednodes];
                    allocate(unusednodeptr,nodeindex);//swap the nodes
                    allocate(nodeptr,usednodes);
                }
                usednodes++;
            }*/
            return nodeptr;
        }

 
       inline void markNodeAsUsed(Node<T>& nodeptr) {
            int nodeindex = nodeptr.ord;
            if (nodeindex >= usednodes) {//wenn die node nicht schon eine usednode ist
                if (nodeindex != usednodes) {
                    Node<T>& unusednodeptr = *ordinv[usednodes];//get first unusednode
                    allocate(unusednodeptr, nodeindex);//swap the nodes so the node we want is the first unusednode
                    allocate(nodeptr, usednodes);
                }
                usednodes++;//make the first unusednode the last usednode
            }
        }

        
        void addedge(Edge<T> newedge){

            markNodeAsUsed(*newedge.start);
            //markNodeAsUsed(*newedge.end);


            if(newedge.start->ord>newedge.end->ord){
                newedges.push_back(newedge);
            }

            
            newedge.start->children.push_back(newedge.end);

        }

        public:
        void insertedges() {
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
                    if (!Q.empty()) {//TODO Remove after testing
                        throw std::runtime_error("Q isn't empty D:");
                    }

                    discover(newedges.begin() + s, newedges.begin() + i, Q, invallidedges, lowerbound, upperbound);
                    shift(lowerbound, Q);
                    s = i;
                    upperbound = x->ord;
                }

                lowerbound = std::min(y->ord, lowerbound);
            }

            if (!Q.empty()) {//TODO Remove after testing
                throw std::runtime_error("Q isn't empty D:");
            }

            discover(newedges.begin() + s, newedges.end(), Q, invallidedges, lowerbound, upperbound);
            shift(lowerbound, Q);

            newedges.clear();

            if (!invallidedges.empty()) {
                throw CycleDetectedException<T>(invallidedges);
            }
        }
        
        private:
        inline void allocate(Node<T>& v,int i){
            v.ord=i;
            ordinv[i]=&v;
        }

       
        void shift(int frontidx, Qtype& Q) {
            if(Q.empty())return;//neded in case of cycle
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


        void dfs(Node<T>& v , int ub,Qtype& Q) {
            v.vacant = true;
            v.onStack = true;
            //cout << "visiting" << v->id<<endl;
            for (Node<T>* s : v.children)
            {

                if (s->onStack||s->ord==ub)//this is not in the paper but neccesary for cycle detection
                    throw std::runtime_error("cycle");
                if ((!s->vacant) && (s->ord < ub))
                    dfs(*s, ub,Q);
            }
            v.onStack = false;

            //Q.emplace_back(&v, ordinv[ub]);
            Q.emplace_back(&v, ub);
        }




    void discover(typename std::vector<Edge<T>>::iterator start,
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
                    for (int i = lb; i <= ub; ++i) ordinv[i]->onStack = false;

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

};