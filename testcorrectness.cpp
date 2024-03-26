


#include "randgraph.cpp"
#include "topord.cpp"


#include <iostream>
#include <vector>
#include <random>
#include <string>
#include "testcorrectness.h"

using Tedges=std::vector<std::pair<int, int>>;


class twrapper{
    public:
    topologicalordering<int> t;
    Tedges edges;
    Tedges invallidedges;
    std::set<int> nodes; 

    
    void addedge(int start,int end){
        t.addedge(start,end);
        edges.emplace_back(start,end);
        nodes.insert(start);
        nodes.insert(end);
    }

    void insertedges(){
        //t.insertedges();
        try {
            t.insertedges(); // Attempt to insert edges
        } catch (const CycleDetectedException<int>& e) {
            std::cerr << "Cycle detected. Handling exception..." << std::endl;
            // Insert cycle edges into invallidedges
            for (const auto& edge : e.getCycleEdges()) {
                invallidedges.push_back(edge);
            }
            // Remove cycle edges from edges
            for (const auto& cycleEdge : e.getCycleEdges()) {
                auto it = std::find(edges.rbegin(), edges.rend(), cycleEdge);
                if (it != edges.rend()) {
                    edges.erase((it + 1).base()); // Erase the element pointed to by the reverse iterator
                }else  {
                    // Throw an exception if the cycle edge is not found
                    throw std::runtime_error("Cycle edge not found in the edges vector");
                }
            }
        }
    }

    void addnode(int node){
        t.addNode(node);
        nodes.insert(node);
    }

    bool iscorrect(){
        if(nodes.size()!=t.size())
            return false;

        for (const auto& pair : edges) {//check all edges
            int start = pair.first;
            int end = pair.second;
            int indexstart=t.ord(start);
            int indexend=t.ord(end);
            if(!(indexstart>=0 && indexend>=0 && indexstart<indexend &&indexend<t.size()&&indexend<t.size()))//check if start and end exist and indexstart<indexend and start and end are in bounds
                return false;
        }
        
        for(int i=0;i<t.size();i++){//check ord
            int node=t.at(i);
            int ord=t.ord(node);
            bool contains= *nodes.find(node)==node;//check if nodes contains ...

            if(!(t[i]==node &&ord==i&&contains))//check if ord/t[] works and noder exists
                return false;
        }
        return true;
    }

    

};
//std::shuffle(randedges.begin(), randedges.end(),gen);

void randomgraphtest(const std::mt19937 &gen)
{
    std::vector<int> insertsintervalls = {1, 2, 3, 4, 8, 16, 32, 64, 512, 1024};

    
    for (int test = 0; test < 100; ++test) {
        auto [randnodes, randedges] = makeGraph(50, INT32_MAX, gen);
        for (int insertsintervall : insertsintervalls) {
            twrapper t;
            for (int i = 0; i < randedges.size(); ++i)
            {
                if (i % insertsintervall == 0)
                {
                    t.insertedges();
                    if (!t.iscorrect())
                        throw std::runtime_error("incorrect");

                }

                auto [start, stop] = randedges[i];
                t.addedge(start, stop);
            }

            t.insertedges();
            if (!t.iscorrect() || t.invallidedges.size() != 0)
                throw std::runtime_error("incorrect");

        }
    }
}

void addcycles(Tedges& edges,std::vector<int>& nodes,std::mt19937 &gen,int m){
    
    //adds m non intersecting cycles to a graph
    int startnode=0;
    int accnode=0;
    std::uniform_int_distribution<int> distribution(0, 10);
    for(int j=0;j<m;j++){
        int n=distribution(gen);
        for(int i=0;i<n;i++){//this generates a cycle
            if((int)nodes.size()-1<accnode)nodes.push_back(accnode);
            edges.emplace_back(accnode,accnode+1);
            accnode++;
        }
        edges.emplace_back(accnode,startnode);
        if((int)nodes.size()-1<accnode)nodes.push_back(accnode);
        accnode++;
        startnode=accnode;
    }
    std::shuffle(edges.begin(), edges.end(), gen);
    std::shuffle(nodes.begin(), nodes.end(), gen);

}

int main(int argc, char const *argv[])
{
    std::random_device rd;
    std::mt19937 gen(rd());

    randomgraphtest(gen);
    std::cout<<"randomgraphtest succses"<<std::endl;

    testcyclesonly(gen);
    std::cout<<"testcyclesonly succses"<<std::endl;

    /*topologicalordering<int> t;
    t.addedge(0,1);
    t.addedge(1,2);
    t.addedge(2,0);
    for(int i=0;i<t.size();i++){
        std::cout<<t[i];
    }
    t.insertedges();
    for(int i=0;i<t.size();i++){
        std::cout<<t[i];
    }
    std::cout<<std::endl;
    //t.insertedges();*/


    

    


    /* code */
    return 0;
}

void testcyclesonly(std::mt19937 &gen)
{
    std::vector<int> insertsintervalls = {1, 2, 3, 4, 8, 16, 32, 64};
    for (int m = 1; m < 5; m++)
    { // num cycles
        for (int j = 0; j < 30; j++)
        { // num tests
            Tedges randedges;
            std::vector<int> nodes;
            addcycles(randedges, nodes, gen, m);
            for (int insertsintervall : insertsintervalls)
            {
                twrapper t;
                for (int i = 0; i < randedges.size(); ++i)
                {
                    if (i % insertsintervall == 0)
                    {
                        t.insertedges();
                        if (!t.iscorrect())
                            throw std::runtime_error("incorrect");
                    }

                    auto [start, stop] = randedges[i];
                    t.addedge(start, stop);
                }

                t.insertedges();
                if (!t.iscorrect() || t.invallidedges.size() != m)
                    throw std::runtime_error("incorrect");
            }
        }
    }
}
