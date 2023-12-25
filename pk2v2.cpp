#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;




struct Node
{
    bool vacant=false;
    bool onStack=false;
    int ord;
    vector<Node*> children=vector<Node*>();//{}
    vector<Node*> childreninv=vector<Node*>();
    int id;
};
struct Edge{
    Node* start;
    Node* end;
    Edge(Node* start,Node* end):start(start),end(end){}
};


class topologicalordering{
    public://TODO change to private
        vector<Node*> nodes=vector<Node*>();
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
            int len=nodes.size();
            Node* n=new Node();
            n->id=id;
            n->ord=len;

            nodes.push_back(n);
            ordinv.push_back(n);
            return n;
        }

        void addedgeindex(int start,int end){
            addedge(Edge(nodes[start],nodes[end]));
        }

        void addedge(Node* start,Node* end){//todo add direct cycle detection
            addedge(Edge(start,end));
        }

        void addedge(Edge newedge){


            cout << " newedge(" << newedge.start->ord<<","<<newedge.end->ord<<")" <<" ";
            if(newedge.start->ord>newedge.end->ord){
                newedges.push_back(newedge);
                cout << " added(" << newedge.start<<","<<newedge.end<<")" <<" ";
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
            for (Node* s : v->children)
            {
                if (v->onStack)
                    throw std::runtime_error("cycle");
                if (!s->vacant && s->ord < ub)
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


int main()
{
    vector<string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const string& word : msg)
    {
        cout << word << " ";
    }
    cout << endl;

    topologicalordering t=topologicalordering();
    /*for(int i=0;i<5;i++){
        t.addnode(i);
    }*/
    vector<Node*> nodes=vector<Node*>(5);


    /*t.addnode(1);cant work because addedge uses indicees
    t.addnode(4);
    t.addnode(3);
    t.addnode(0);
    t.addnode(2);*/
    /*for (int n : {1, 4, 3, 0, 2}) {
       nodes[n]=t.addnode(n);
    }*/

    for (int n : {0, 1, 2, 3, 4}) {
       nodes[n]=t.addnode(n);
    }

    t.addedge(nodes[1],nodes[4]);
    t.addedge(nodes[0],nodes[3]);
    t.addedge(nodes[1],nodes[2]);
    t.addedge(nodes[4],nodes[2]);
    for(Edge n:t.newedges){
        cout << "(" << n.start<<","<<n.end<<")" <<" ";
    }
    cout<<endl;
    for(Node* n:t.ordinv){
        cout << n->id <<" ";
    }
    cout<<endl;
    t.insertedges();
    cout << t.validate() << endl;

    for(Node* n:t.ordinv){
        cout << n->id <<" ";
    }
    cout<<endl;
    
}


/*
['1', '4', '3', '0', '2'] [('1', '4'), ('0', '3'), ('1', '3'), ('4', '2')]
[(0, 3)]
BS []
[1, 4, 0, 3, 2]
['1', '4', '0', '3', '2']
[('1', '4'), ('0', '3'), ('1', '3'), ('4', '2')]
*/