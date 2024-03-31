


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
    std::unordered_set<int> nodes; 

    
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
            //std::cerr << "Cycle detected. Handling exception..." << std::endl;
            // Insert cycle edges into invallidedges
            for (const auto& edge : e.getCycleEdges()) {
                invallidedges.push_back(edge);
            }

            // Remove cycle edges from edges
           for (const auto& cycleEdge : e.getCycleEdges()) {
            edges.erase(std::remove(edges.begin(), edges.end(), cycleEdge), edges.end());
                /* auto it = std::find(edges.rbegin(), edges.rend(), cycleEdge);
                if (it != edges.rend()) {
                    edges.erase((it + 1).base()); // Erase the element pointed to by the reverse iterator
                }else  {
                    // Throw an exception if the cycle edge is not found
                    throw std::runtime_error("Cycle edge not found in the edges vector");
                }*/
            }
        }
    }

    void addnode(int node){
        t.addNode(node);
        nodes.insert(node);
    }

    bool iscorrect(){
        if(!t.validate()){
            /*for(int i=0;i<t.size();i++)
                std::cout<<t[i];
            std::cout<<std::endl;*/
            
            return false;
        }
            
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

void randomtopgraphtest(const std::mt19937 &gen)
{
    std::vector<int> insertsintervalls = {1, 2, 3, 4, 8, 16, 32, 64, 512, 1024};

    
    for (int test = 0; test < 100; ++test) {
        auto [randnodes, randedges] = makeGraph(10, INT32_MAX, gen);
        for (int insertsintervall : insertsintervalls) {
            twrapper t;
            if(test%2==0)for(int n:randnodes)t.addnode(n);
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
            if (!t.iscorrect() || t.invallidedges.size() != 0)//t has to be correct and there cant be invallid edges
                throw std::runtime_error("incorrect");

        }
    }
}

void addcycles(Tedges& edges,std::vector<int>& nodes,std::mt19937 &gen,int m){
    
    //adds m non intersecting cycles to a graph
    int startnode=0;
    int accnode=0;
    std::uniform_int_distribution<int> distribution(0, 5);
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
void testcyclesonly(std::mt19937 &gen)
{
    std::vector<int> insertsintervalls = {1, 2, 3, 4, 8, 16, 32, 64};
    for (int m = 1; m < 5; m++)
    { // num cycles
        for (int j = 0; j < 30; j++)
        { // num tests
            Tedges randedges;
            std::vector<int> randnodes;
            addcycles(randedges, randnodes, gen, m);
            for (int insertsintervall : insertsintervalls)
            {
                twrapper t;
                if(j%2==0)for(int n:randnodes)t.addnode(n);
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
                if (!t.iscorrect() || t.invallidedges.size() != m)//t has to be correct and the number of invallid edges must equal the number of cycles
                    throw std::runtime_error("incorrect");
            }
        }
    }
}
void testgraphwithcycles(std::mt19937 &gen)
{
    std::vector<int> insertsintervalls = {1, 2, 3, 4, 8, 16, 32, 64};
    for (int m = 1; m < 5; m++)
    { // num cycles
        for (int j = 0; j < 30; j++)
        { // num tests
            auto [randnodes, randedges] = makeGraph(20, INT32_MAX, gen);
            addcycles(randedges, randnodes, gen, m);
            std::shuffle(randedges.begin(), randedges.end(), gen);
            for (int insertsintervall : insertsintervalls)
            {
                twrapper t;
                if(j%2==0)for(int n:randnodes)t.addnode(n);
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
                if (!(t.iscorrect() && t.invallidedges.size() >= m))//t has to be correct and the number of invallid edges must be greater than the number of cycles
                    throw std::runtime_error("incorrect");
            }
        }
    }
}

void testgraph2(std::mt19937 &gen)
{
    std::vector<int> insertsintervalls = {1, 2, 3, 4, 8, 16, 32, 64};
    for (int i = 0; i < 10; i++)
    { // num cycles
        auto [randnodes, randedges] = makeGraph(20, INT32_MAX, gen);
        auto [randnodes2, randedges2] = makeGraph(20, INT32_MAX, gen);
        auto [randnodes3, randedges3] = makeGraph(20, INT32_MAX, gen);
        auto [randnodes4, randedges4] = makeGraph(20, INT32_MAX, gen);
        randedges.insert( randedges.end(), randedges2.begin(), randedges2.end() );
        randedges.insert( randedges.end(), randedges3.begin(), randedges3.end() );
        randedges.insert( randedges.end(), randedges4.begin(), randedges4.end() );
        std::shuffle(randedges.begin(), randedges.end(), gen);
        for (int insertsintervall : insertsintervalls)
        {
            twrapper t;
            if(i%2==0)for(int n:randnodes)t.addnode(n);//add nodes in half of testcases

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
            if (!t.iscorrect())//t has to be correct and the number of invallid edges must be greater than the number of cycles
                throw std::runtime_error("incorrect");
        }
    }
}
int main(int argc, char const *argv[])
{
    std::random_device rd;
    auto r=rd();
    //r=1188961176;
    std::mt19937 gen(r);
    std::cout<<r<<std::endl;


    /*twrapper t;
    t.addedge(3,2);
    t.addedge(4,5);

    t.insertedges();
    std::cout<<"?"<<t.iscorrect()<<std::endl;
    for(int i=0;i<t.t.size();i++){std::cout<<t.t[i];}std::cout<<std::endl;

    t.addedge(2,4);
    t.addedge(2,3);
    
    t.insertedges();
    std::cout<<"?"<<t.iscorrect()<<std::endl;
    for(int i=0;i<t.t.size();i++){
        std::cout<<t.t[i];
    }
    std::cout<<std::endl;

    

    return 0;*/
    int i=0;
    
    while(true){
    randomtopgraphtest(gen);
    std::cout<<"randomtopgraphtest success"<<std::endl;

    testcyclesonly(gen);
    std::cout<<"testcyclesonly success"<<std::endl;

    testgraphwithcycles(gen);
    std::cout<<"testgraphwithcycles success"<<std::endl;

    testgraph2(gen);
    std::cout<<"testgraph2 success"<<std::endl;

    std::cout<<i++<<std::endl;

    }



    /*twrapper t;



    //t.addedge(0,1);
    //t.addedge(3,0);
    //t.addedge(3,4);
    //t.addedge(5,6);
    //t.addedge(2,1);
    //t.addedge(8,0);
    //t.addedge(2,0);
    //t.addedge(2,4);
    //t.addedge(4,5);
    //t.addedge(0,1);
    //t.addedge(4,2);
    //t.addedge(3,4);
    //t.addedge(3,1);
    
    /*t.addedge(1,2);
    t.addedge(2,1);
    t.addedge(3,3);
    t.addedge(4,5);
    t.addedge(4,5);*/

    /*//t.addedge(0,4);
    t.addedge(2,3);
    t.addedge(0,1);
    t.addedge(1,0);
    t.addedge(1,2);
    //t.addedge(1,4);
    //t.addedge(3,0);
    //t.addedge(2,4);
    t.addedge(2,0);
    t.addedge(1,2);
    //t.addedge(2,3);
    //t.addedge(3,4);
    //t.addedge(1,3);
    //t.addedge(0,3);
    t.insertedges();
    std::cout<<t.iscorrect()<<std::endl;*/

    /*for(int i=0;i<t.size();i++){
        std::cout<<t[i];
    }
    std::cout<<std::endl;*/
    //t.insertedges();*/


    

    


    /* code */
    return 0;
}


