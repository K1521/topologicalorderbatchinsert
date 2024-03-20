// my_cpp_program.cpp
#include <iostream>
#include <chrono>
#include <vector>
#include <set>
#include <random>
#include "topologicalordering2.cpp"
//#include <gperftools/profiler.h>
// Your C++ code goes here...






std::pair<std::vector<int>, std::vector<std::pair<int, int>>> 
makefullgraph(int nodenum){
    std::vector<int> randnodes;
    randnodes.reserve(nodenum);
    for(int i=0;i<nodenum;i++)
        randnodes.emplace_back(i);
    std::random_shuffle(randnodes.begin(), randnodes.end());

    std::vector<std::pair<int, int>> randedges;
    randedges.reserve(nodenum*(nodenum-1)/2);
    for(int i=0;i<nodenum;i++){//this code generates a "full" graph (i mean it generetes a graph wich has a topological order with the maximum node number) 
        for(int j=i+1;j<nodenum;j++){
            randedges.emplace_back(std::make_pair(randnodes[i], randnodes[j]));
        }
    }
    std::random_shuffle(randedges.begin(), randedges.end());
    std::random_shuffle(randnodes.begin(), randnodes.end());
    return std::make_pair(randnodes, randedges);
}


std::pair<std::vector<int>, std::vector<std::pair<int, int>>> 
makeGraph(int numNodes = 1000, int numEdges = INT_MAX) {
    

    if(numEdges>(long long)numNodes*(numNodes-1)/4){
        //std::cout <<((long long)numNodes)*(numNodes-1)/4<<std::endl;
        auto [nodes,edges]=makefullgraph(numNodes);
        edges.resize(std::min(numEdges,(int)edges.size()));
        return std::make_pair(nodes,edges);
    }

    std::vector<int> randnodes;
    for (int i = 0; i < numNodes; ++i) {
        randnodes.push_back(i);
    }
    std::random_shuffle(randnodes.begin(), randnodes.end());

    
    std::set<std::pair<int, int>> edgesSet;
    std::mt19937 gen(std::rand());

    while (edgesSet.size() < numEdges) {
        int a = std::uniform_int_distribution<int>(0, numNodes - 2)(gen);
        int b = std::uniform_int_distribution<int>(a + 1, numNodes - 1)(gen);

        edgesSet.insert(std::make_pair(randnodes[a], randnodes[b]));
    }
    std::vector<std::pair<int, int>> randedges( edgesSet.begin(), edgesSet.end());



    std::random_shuffle(randedges.begin(), randedges.end());
    std::random_shuffle(randnodes.begin(), randnodes.end());

    return std::make_pair(randnodes, randedges);
}

std::pair<std::vector<int>, std::vector<long long>> 
testwithdict(int nodenum,int insertsintervall,int maxedgenum,int samplesintervall,bool insertnodes){
        auto [randnodes, randedges]=makeGraph(nodenum,maxedgenum);

    // Example: Measure time using std::chrono
    
    std::vector<int> timingsx;
    std::vector<long long> timingsy;
    auto tstart = std::chrono::high_resolution_clock::now();

    topologicalordering<int> t;

    if(insertnodes){
        t.reserve(randnodes.size());
        for(int i=0;i<randnodes.size();i++)
            t.addNode(randnodes[i]);
    }
    /**/

    
    for(int i = 0; i < randedges.size(); ++i) {
        if (i % samplesintervall == 0) {
            auto tnow = std::chrono::high_resolution_clock::now();
            auto tdelta = std::chrono::duration_cast<std::chrono::microseconds>(tnow - tstart).count();

            timingsx.push_back(i);
            timingsy.push_back(tdelta);
        }

        if (i % insertsintervall == 0) {
            t.insertedges();
        }

        auto [start, stop] = randedges[i];
        t.addedge(start, stop);

        
    }

    {
        t.insertedges();
        auto tnow = std::chrono::high_resolution_clock::now();
        auto tdelta = std::chrono::duration_cast<std::chrono::microseconds>(tnow - tstart).count();

        timingsx.push_back(randedges.size());
        timingsy.push_back(tdelta);

        if(!t.validate()){throw std::runtime_error("cycle");}
    }

    return std::make_pair(timingsx, timingsy);
}

std::pair<std::vector<int>, std::vector<long long>> 
testhandles(int nodenum,int insertsintervall,int maxedgenum,int samplesintervall){
        auto [randnodes, randedges]=makeGraph(nodenum,maxedgenum);

    // Example: Measure time using std::chrono
    
    std::vector<int> timingsx;
    std::vector<long long> timingsy;
    auto tstart = std::chrono::high_resolution_clock::now();

    topologicalordering<int> t;

    std::vector<NodeHandle<int>> nodes(randnodes.size());
    t.reserve(randnodes.size());
    for(int i=0;i<randnodes.size();i++){
        int val=randnodes[i];
        nodes[val]=t.addNode(val);
    }
    
    /**/

    
    for(int i = 0; i < randedges.size(); ++i) {
        if (i % samplesintervall == 0) {
            auto tnow = std::chrono::high_resolution_clock::now();
            auto tdelta = std::chrono::duration_cast<std::chrono::microseconds>(tnow - tstart).count();

            timingsx.push_back(i);
            timingsy.push_back(tdelta);
        }

        if (i % insertsintervall == 0) {
            t.insertedges();
        }

        auto [start, stop] = randedges[i];
        //t.addedge(start, stop);
        nodes[start].addedge(nodes[stop]);


        
    }

    {
        t.insertedges();
        auto tnow = std::chrono::high_resolution_clock::now();
        auto tdelta = std::chrono::duration_cast<std::chrono::microseconds>(tnow - tstart).count();

        timingsx.push_back(randedges.size());
        timingsy.push_back(tdelta);

        if(!t.validate()){throw std::runtime_error("cycle");}
    }

    return std::make_pair(timingsx, timingsy);
}

std::pair<std::vector<int>, std::vector<long long>> 
test(int nodenum,int insertsintervall,int maxedgenum,int samplesintervall,int insertnodes){
    if(insertnodes==0){
        return testwithdict(nodenum, insertsintervall, maxedgenum, samplesintervall,false);
    }else if(insertnodes==1){
        return testwithdict(nodenum, insertsintervall, maxedgenum, samplesintervall,true);
    }else{
        return testhandles(nodenum, insertsintervall, maxedgenum, samplesintervall);
    }
}






int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc != 8) {
        std::cerr << "Usage: " << argv[0] << " <nodenum> <randseed> <insertsintervall> <repeats> <edgenum> <samplesintervall> <insertnodes>" << std::endl;
        return 1;
    }

    int nodenum = std::stoi(argv[1]);
    int randseed = std::stoi(argv[2]);
    int insertsintervall = std::stoi(argv[3]);
    int repeats = std::stoi(argv[4]);  // Number of times to repeat the test
    int edgenum = std::stoi(argv[5]);
    int samplesintervall = std::stoi(argv[6]);
    bool insertnodes = std::stoi(argv[7]);

    
    /*int nodenum = 1000000;
    int randseed = 42;
    int insertsintervall = 1000;
    //int repeats = std::stoi(argv[4]);
    int edgenum = nodenum;
    int samplesintervall = nodenum/1000;*/

    std::srand(randseed);

    auto [timingsx, averagedTimings] = test(nodenum, insertsintervall, edgenum, samplesintervall,insertnodes);

    // Repeat the test 'repeats' times and accumulate the results
    for (int i = 1; i < repeats; ++i) {
        auto [_, timingsy] = test(nodenum, insertsintervall, edgenum, samplesintervall,insertnodes);

        // Accumulate timingsy
        for (std::size_t j = 0; j < timingsy.size(); ++j) {
            averagedTimings[j] += timingsy[j];
        }
    }

    // Average the accumulated timingsy
    for (std::size_t i = 0; i < averagedTimings.size(); ++i) {
        averagedTimings[i] /= repeats;
    }

    // Print timingsx
    for (auto timex : timingsx) {
        std::cout << timex << " ";
    }
    std::cout << std::endl;

    // Print averaged timingsy
    for (auto timey : averagedTimings) {
        std::cout << timey << " ";
    }
    std::cout << std::endl;

    return 0;
}