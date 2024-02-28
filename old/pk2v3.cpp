#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <iterator>  
#include <thread>
#include <mutex>
using namespace std;




struct Node
{
    bool vacant=false;
    bool onStack=false;
    int ord;
    vector<Node*> children;//{}
    vector<Node*> childreninv;
    int id;
};
struct Edge{
    Node* start;
    Node* end;
    Edge(Node* start,Node* end):start(start),end(end){}
};


class topologicalordering{
    public://TODO change to private
        //vector<Node*> nodes=vector<Node*>();
        vector<Node*> ordinv=vector<Node*>();
        vector<Edge> newedges=vector<Edge>();
        
        
    public:
        
        bool validate(){
            //cout << ordinv.size()<< endl;
            for(int i=0;i<ordinv.size();i++){
                Node* n=ordinv[i];
                if(n->ord!=i)
                    return false;
                
                for(Node* childnode:n->children){
                    if(n->ord>=childnode->ord)
                    return false;
                }
            }
            return true;
        }
        Node* addnode(int id){
            int len=ordinv.size();
            Node* n=new Node();
            n->id=id;
            n->ord=len;
            n->children.reserve(8);
            n->childreninv.reserve(8);

            //nodes.push_back(n);
            ordinv.push_back(n);
            return n;
        }

        /*void addedgeindex(int start,int end){
            addedge(Edge(nodes[start],nodes[end]));
        }*/

        /*void parallelAddEdges(const std::vector<Edge>& edges) {
            const size_t numEdges = edges.size();

            const size_t numThreads = 2;std::thread::hardware_concurrency();
            const size_t nodeChunkSize = (numEdges + numThreads - 1) / numThreads;//ceil diff

            std::vector<std::thread> threads;
            threads.reserve(numThreads);

            auto addEdgesInRange = [this, &edges](size_t startNode, size_t endNode) {
                for (size_t i = startNode; i < endNode; ++i) {
                    addedge(edges[i]);
                }
            };

            for (size_t i = 0; i < numThreads; ++i) {
                size_t startNode = i * nodeChunkSize;
                size_t endNode = std::min((i + 1) * nodeChunkSize, numEdges);

                threads.emplace_back(addEdgesInRange, startNode, endNode);
            }

            for (auto& thread : threads) {
                thread.join();
            }
        }*/
        

        /*void parallelAddEdges(const std::vector<Edge>& edges) {//not faster :(
            //const size_t numEdges = edges.size();

            cout <<std::thread::hardware_concurrency();
            //const size_t nodeChunkSize = (numEdges + numThreads - 1) / numThreads;//ceil diff

            std::vector<std::thread> threads;
            //threads.reserve(numThreads);
            threads.reserve(3);

            vector<Edge> edges2(edges);

            auto addedges = [this, &edges]() {
                for(int i=0;i<edges.size();++i){
                    Edge e=edges[i];
                    if(e.start->ord>e.end->ord)
                        newedges.push_back(e);
                    e.start->children.push_back(e.end);
                    //cout <<i<<" 1"<<endl;
                }
            };
            
            threads.emplace_back(addedges);
                for(int i=0;i<edges2.size();++i){
                    Edge e=edges2[i];
                    e.end->childreninv.push_back(e.start);
                    //cout <<i<<" 2"<<endl;
                }

            
            
            //threads.emplace_back(addchildren);
            //threads.emplace_back(addchildreninv);
            threads[0].join();

        }*/
 

        void addedge(Node* start,Node* end){//todo add direct cycle detection
            addedge(Edge(start,end));
        }

        void addedge(Edge newedge){


            //cout << " newedge(" << newedge.start->ord<<","<<newedge.end->ord<<")" <<" ";
            if(newedge.start->ord>newedge.end->ord){
                newedges.push_back(newedge);
                //cout << " added(" << newedge.start<<","<<newedge.end<<")" <<" ";
            }
            
            newedge.start->children.push_back(newedge.end);
            newedge.end->childreninv.push_back(newedge.start);
            //newedges.emplace_back(&nodes[start],&nodes[end]);
        }
        void insertedges(){
            if (newedges.empty()) return;

            std::sort(newedges.begin(), newedges.end(),
                    [](const Edge& e1, const Edge& e2) {
                        return e1.start->ord > e2.start->ord;
                    });

            int lowerbound = ordinv.size();
            int s = 0;
            vector<pair<Node*, Node*>> Q;

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
            
        }

        inline void allocate(Node* v,int i){
            v->ord=i;
            ordinv[i]=v;
        }

        void shift(int i, std::vector<pair<Node*, Node*>>& Q) {
            int n = 0;

            auto [v, d] = Q.back();
            Q.pop_back();

            while (true) {
                Node* w = ordinv[i];

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
        }


        



        void dfs(Node* v, int ub,vector<pair<Node*, Node*>>& Q) {
            v->vacant = true;
            v->onStack = true;
            //cout << "visiting" << v->id<<endl;
            for (Node* s : v->children)
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


        void discover(std::vector<Edge>::iterator start,std::vector<Edge>::iterator end,vector<std::pair<Node*, Node*>>& Q) {
            sort(start,end, [](const Edge& edge1, const Edge& edge2) {
                return edge1.start->ord > edge2.start->ord;
            });

            //vector<pair<Node*, Node*>> Q=vector<pair<Node*, Node*>>();//TODO return Q by refference
            for (auto it = start; it != end; ++it) 
            {
                Edge edge=*it;
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

    auto [randnodes, randedges]=makeGraph(nodenum,4000);
    //std::sort(randedges.begin(), randedges.end());
    if(nodenum<100)
        printVector(randnodes);

    topologicalordering t;
    vector<Node*> nodes(nodenum);
    auto ts = std::chrono::high_resolution_clock::now();
    

    for(int i:randnodes){
        nodes[i]=t.addnode(i);
    }

    auto taddednodes = std::chrono::high_resolution_clock::now();

    
    for(auto [start,stop]:randedges){
        t.addedge(nodes[start],nodes[stop]);
        //cout << start <<","<<stop<< endl;
    }
    /*vector<Edge> newedges;
    newedges.reserve(nodenum*4);
    for(auto [start,stop]:randedges){
        newedges.emplace_back(nodes[start],nodes[stop]);
        //cout << start <<","<<stop<< endl;
    }*/
    auto tnewedges = std::chrono::high_resolution_clock::now();
    //t.parallelAddEdges(newedges);
    
    //taddednodes = std::chrono::high_resolution_clock::now();
    //auto taddededges = std::chrono::high_resolution_clock::now();
    //




    auto taddededges = std::chrono::high_resolution_clock::now();
    t.insertedges();
    auto tend = std::chrono::high_resolution_clock::now();
    //auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(start_time2 - start_time).count();
    //auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time2).count();

    //std::cout << "Execution time1: " << duration1 << " microseconds" << std::endl;
    //std::cout << "Execution time2: " << duration2 << " microseconds" << std::endl;

    std::cout << "Execution taddednodes: " << std::chrono::duration_cast<std::chrono::microseconds>(taddednodes - ts).count() << " microseconds" << std::endl;
    std::cout << "Execution tnewedges: " << std::chrono::duration_cast<std::chrono::microseconds>(tnewedges - taddednodes).count() << " microseconds" << std::endl;
    std::cout << "Execution taddededges: " << std::chrono::duration_cast<std::chrono::microseconds>(taddededges-tnewedges).count() << " microseconds" << std::endl;
    std::cout << "Execution tinsert: " << std::chrono::duration_cast<std::chrono::microseconds>(tend - taddededges).count() << " microseconds" << std::endl;
  
    if(!t.validate()){
        throw std::runtime_error("cycle");
    }



    vector<int> ids(t.ordinv.size());
    //ids.reserve(t.ordinv.size());
    cout << t.ordinv.size()<< endl;
    for(int i=0;i<t.ordinv.size();i++){
        ids[i]=t.ordinv[i]->id;
        if(t.ordinv[i]->id!=i){
            cout<<i<<endl;
        }
    }
    
    printVector(ids);

//}


}   


/*
['1', '4', '3', '0', '2'] [('1', '4'), ('0', '3'), ('1', '3'), ('4', '2')]
[(0, 3)]
BS []
[1, 4, 0, 3, 2]
['1', '4', '0', '3', '2']
[('1', '4'), ('0', '3'), ('1', '3'), ('4', '2')]
*/