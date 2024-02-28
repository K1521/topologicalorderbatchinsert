#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <iterator>  
#include <unordered_map>  
//#include <thread>
//#include <mutex>
using namespace std;


template <typename T>
class topologicalordering;  // Forward declaration

template <typename T>
class Node {
public:
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
};


template <typename T>
class topologicalordering{
    public://TODO change to private
        //vector<Node*> nodes=vector<Node*>();
        vector<Node<T>*> ordinv;
        vector<Edge<T>> newedges;
        std::unordered_map< T, Node<T>* > ValueToNode;
        
        
    public:        
        inline int size(){
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
                Node<T>* n=ordinv[i];
                if(n->ord!=i)
                    return false;
                
                for(Node<T>* childnode:n->children){
                    if(n->ord>=childnode->ord)
                    return false;
                }
                if(n->vacant||n->onStack)return false;
                    
            }
            return true;
        }

        void addNode(T value){
            getOrCreateNode(value);
        }


        void addedge(T start,T end){//todo add direct cycle detection
            addedge(Edge( getOrCreateNode(start),getOrCreateNode(end)));
        }

        size_t size() const {
            return ordinv.size();
        }

        T operator[](size_t index) const {//TODO throw index out of bounds
            return ordinv.at(index)->value;
        }

        private:

        /*void addedge(T start,T end){//todo add direct cycle detection
            addedge(Edge( ValueToNode.at(start),ValueToNode.at(end)));
        }*/

        /*void addNode(T value){
            int ord=ordinv.size();
            Node<T>* n=new Node<T>(ord,value);
            n->children.reserve(8);


            //nodes.push_back(n);
            ordinv.push_back(n);
            ValueToNode.insert({ value, n });
        }*/

        Node<T>* getOrCreateNode(T value) {
            Node<T>*& nodeptr = ValueToNode[value];
            if (nodeptr==NULL) {
                int ord = ordinv.size();
                nodeptr = new Node<T>(ord, value);
                nodeptr->children.reserve(8);
                ordinv.push_back(nodeptr);
            }
            return nodeptr;
        }

 
       

        
        void addedge(Edge<T> newedge){


            //cout << " newedge(" << newedge.start->ord<<","<<newedge.end->ord<<")" <<" ";
            if(newedge.start->ord>newedge.end->ord){
                newedges.push_back(newedge);
                //cout << " added(" << newedge.start<<","<<newedge.end<<")" <<" ";
            }
            
            newedge.start->children.push_back(newedge.end);
            //newedges.emplace_back(&nodes[start],&nodes[end]);
        }
        public:
        void insertedges(){
            if (newedges.empty()) return;

            std::sort(newedges.begin(), newedges.end(),
                    [](const Edge<T>& e1, const Edge<T>& e2) {
                        return e1.start->ord > e2.start->ord;
                    });

            int lowerbound = ordinv.size();
            int s = 0;
            vector<pair<Node<T>*, Node<T>*>> Q;

            for (int i = 0; i < newedges.size(); i++) {
                auto [x, y] = newedges[i];
                if (x->ord < lowerbound && i != 0) {
                    Q.clear();
                    discover(newedges.begin() + s, newedges.begin() + i,Q);
                    shift(lowerbound, Q);
                    s = i;
                }

                lowerbound = min(y->ord, lowerbound);
            }
            Q.clear();
            //Q.reserve(newedges.size()-s);
            discover(newedges.begin() + s, newedges.end(),Q);
            shift(lowerbound, Q);


            newedges.clear();//<-lol i forgot this line and then i spent 2 day debugging
            
        }
        private:
        inline void allocate(Node<T>* v,int i){
            v->ord=i;
            ordinv[i]=v;
        }

        /*void shift(int i, std::vector<pair<Node<T>*, Node<T>*>>& Q) {
            int n = 0;

            auto [v, d] = Q.back();
            Q.pop_back();

            while (true) {
                Node<T>* w = ordinv.at(i);

                if (w->vacant) {
                    ++n;
                    w->vacant= false;
                } else {
                    allocate(w, i - n);
                }

                while (w == d) {
                    n--;
                    allocate(v, i - n);

                    if (Q.empty()) return;
                    std::tie(v, d) = Q.back();//TODO maybe replace?
                    Q.pop_back();
                }

                i++;
            }
        }*/
        /*void shift(int i, std::vector<pair<Node<T>*, Node<T>*>>& Q) {
            int n = 0;

            
            

            while (!Q.empty()) {
                Node<T>* w = ordinv.at(i);

                if (w->vacant) {
                    ++n;
                    w->vacant= false;
                } else {
                    allocate(w, i - n);
                }
                auto [v, d] = Q.back();

                while (!Q.empty() && w == d) {
                    n--;
                    allocate(v, i - n);

                    Q.pop_back();
                    std::tie(v, d) = Q.back();//TODO maybe replace?
                    
                }

                i++;
            }
        }*/
        /*void shift(int frontidx, std::vector<pair<Node<T>*, Node<T>*>>& Q) {
            int fillidx = frontidx;
            //int frontidx = i;

            
            
            
            while(true){
                Node<T>* frontnode = ordinv.at(frontidx);
                if(!frontnode->vacant){//shift non vacant nodes
                    allocate(frontnode,fillidx++);
                }else{
                    frontnode->vacant=false;
                }

                auto [v, d] = Q.back();
                while (frontnode==d)//insert nodes from Q
                {
                    allocate(v,fillidx++);
                    Q.pop_back();
                    std::tie(v, d) = Q.back();
                    if(Q.empty())return;
                }
                frontidx++;
            }
            

            
        }*/

        /*void shift(int frontidx, std::vector<pair<Node<T>*, Node<T>*>>& Q) {
            int fillidx = frontidx;
            //int frontidx = i;

            
            
            auto [v, d] = Q.back();
            while(true){
                Node<T>* frontnode = ordinv.at(frontidx);
                if(!frontnode->vacant){//shift non vacant nodes
                    allocate(frontnode,fillidx++);
                }else{
                    frontnode->vacant=false;
                }
                while (frontnode==d)//insert nodes from Q
                {
                    allocate(v,fillidx++);
                    Q.pop_back();
                    if(Q.empty())return;
                    std::tie(v, d) = Q.back();
                }
                frontidx++;
            }
            

            
        }*/
        void shift(int frontidx, std::vector<pair<Node<T>*, Node<T>*>>& Q) {
            int fillidx = frontidx;
            auto [v, d] = Q.back();Q.pop_back(); //i am more familiar with pythons pop
            
            while(true){
                Node<T>* frontnode = ordinv.at(frontidx);
                if(!frontnode->vacant){//shift non vacant nodes
                    allocate(frontnode,fillidx++);
                }else{
                    frontnode->vacant=false;
                }
                while (frontnode==d)//insert nodes from Q
                {
                    allocate(v,fillidx++);
                    if(Q.empty())return;
                    std::tie(v, d) = Q.back();Q.pop_back();
                }
                frontidx++;
            }
            

            
        }


        



        void dfs(Node<T>* v , int ub,vector<pair<Node<T>*, Node<T>*>>& Q) {
            v->vacant = true;
            v->onStack = true;
            //cout << "visiting" << v->id<<endl;
            for (Node<T>* s : v->children)
            {
                //cout << "child" << v->id<<endl;
                if (s->onStack)
                    throw std::runtime_error("cycle");
                if ((!s->vacant) && (s->ord < ub))
                    dfs(s, ub,Q);
            }
            v->onStack = false;
            //Q.push_back({v, ordinv[ub]});
            Q.emplace_back(v, ordinv[ub]);
        }


        void discover(typename std::vector<Edge<T>>::iterator start,
                      typename std::vector<Edge<T>>::iterator end,
                      std::vector<std::pair<Node<T>*, Node<T>*>>& Q) {

            //end not included
            sort(start,end, [](const Edge<T>& edge1, const Edge<T>& edge2) {
                return edge1.start->ord > edge2.start->ord;
            });//ich bin relativ sicher das dieser sort vermeidbar ist

            //vector<pair<Node*, Node*>> Q=vector<pair<Node*, Node*>>();//TODO return Q by refference
            for (auto it = start; it != end; ++it) 
            {
                Edge<T> edge=*it;
                if (!edge.end->vacant)
                {
                    dfs(edge.end, edge.start->ord,Q);
                }
            }

            // Q contains the discovered nodes

        }



        

    
};

template <typename T>
void printVector(const std::vector<T>& vec) {
    std::cout << "[";
    if (!vec.empty()) {
        std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T>(std::cout, ", "));
        std::cout << vec.back();
    }
    std::cout << "]" << std::endl;
}


template <typename T>
void removeDuplicates(std::vector<T>& vec) {
    std::sort(vec.begin(), vec.end());
    auto last = std::unique(vec.begin(), vec.end());
    vec.erase(last, vec.end());
}

std::pair<std::vector<int>, std::vector<std::pair<int, int>>>
makeGraph(int numNodes = 1000, int connsPerNode = 1) {
    std::vector<int> nodes;
    for (int i = 0; i < numNodes; ++i) {
        nodes.push_back(i);
    }
    //std::random_shuffle(nodes.begin(), nodes.end());

    std::vector<std::pair<int, int>> connections;
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < connsPerNode * numNodes; ++i) {
        int a = std::uniform_int_distribution<int>(0, numNodes - 2)(gen);
        int b = std::uniform_int_distribution<int>(a + 1, numNodes - 1)(gen);

        connections.push_back(std::make_pair(nodes[a], nodes[b]));
    }

    // Remove duplicates using std::unique and erase-remove idiom
    removeDuplicates(connections);

    std::random_shuffle(connections.begin(), connections.end());
    std::random_shuffle(nodes.begin(), nodes.end());

    return std::make_pair(nodes, connections);
}

int main()
{
//for(int j=0;j<10000;++j){

    int nodenum=1000;

    auto [randnodes, randedges]=makeGraph(nodenum,100);
    //std::sort(randedges.begin(), randedges.end());
    if(nodenum<100)
        printVector(randnodes);

    




    
    topologicalordering<std::string> t;
    /*t.addNode("y");
    t.addNode("a");
    t.addNode("z");
    t.addNode("b");
    t.addNode("w");
    t.addNode("c");
    t.addNode("x");
    t.addNode("d");

    t.addedge("y","z");
    t.addedge("a","w");
    t.addedge("z","w");
    t.addedge("c","x");
    t.addedge("x","y");
    t.addedge("d","w");*/

    /*t.addNode("a");
    t.addNode("b");
    t.addNode("c");
    t.addNode("d");

    t.addedge("c","a");
    t.addedge("d","b");*/

    t.addNode("f");
    t.addNode("w");
    t.addNode("g");
    t.addNode("x");
    t.addNode("h");
    t.addNode("y");
    t.addNode("i");
    t.addNode("z");
    t.addNode("j");

    t.addedge("x","y");
    t.addedge("i","j");
    t.addedge("h","g");
    t.addedge("j","h");
    t.insertedges();
    for(int i=0;i<t.size();i++) std::cout <<t[i];
    std::cout <<std::endl;

    t.addedge("i","f");
    t.insertedges();
    if(!t.validate()){throw std::runtime_error("cycle");}



    for(int i=0;i<t.size();i++) std::cout <<t[i];
    
    
    /*int myNumbers[5] = {10, 20, 30, 40, 50};
    for (int j = 0; j < 5; j++) {
        int insertsintervall=myNumbers[j];

        auto start = std::chrono::high_resolution_clock::now();
        topologicalordering<int> t;
        for(int i=0;i<randedges.size();i++){
            if(i%insertsintervall==0){
                t.insertedges();
                std::cout << t.validate() << std::endl;
            }
                
            
            auto [start,stop]=randedges[i];
            t.addedge(start,stop);
            
        }
        t.insertedges();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "time: " << std::chrono::duration_cast<std::chrono::microseconds>(taddednodes - ts).count() << " microseconds" <<" inserts every:"<<insertsintervall<< std::endl;
    }*/



    

    
    //auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(start_time2 - start_time).count();
    //auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time2).count();

    //std::cout << "Execution time1: " << duration1 << " microseconds" << std::endl;
    //std::cout << "Execution time2: " << duration2 << " microseconds" << std::endl;

    
  
}   


/*
['1', '4', '3', '0', '2'] [('1', '4'), ('0', '3'), ('1', '3'), ('4', '2')]
[(0, 3)]
BS []
[1, 4, 0, 3, 2]
['1', '4', '0', '3', '2']
[('1', '4'), ('0', '3'), ('1', '3'), ('4', '2')]
*/