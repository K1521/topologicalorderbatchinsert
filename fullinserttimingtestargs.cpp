// my_cpp_program.cpp
#include <iostream>
#include <chrono>
#include <vector>
#include <set>
#include <random>
#include "topologicalordering.cpp"

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
    

    if(numEdges>numNodes*(numNodes-1)/4){

        auto [nodes,edges]=makefullgraph(numNodes);
        edges.resize(std::min((long)numEdges,(long)edges.size()));
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

std::pair<std::vector<int>, std::vector<long>> 
test(int nodenum,int insertsintervall,int maxedgenum,int samplesintervall){
        auto [randnodes, randedges]=makeGraph(nodenum,maxedgenum);

    // Example: Measure time using std::chrono
    
    std::vector<int> timingsx;
    std::vector<long> timingsy;
    auto tstart = std::chrono::high_resolution_clock::now();

    topologicalordering<int> t;
    /*for(int i=0;i<randnodes.size();i++)
        t.addNode(randnodes[i]);*/

    
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

        timingsx.push_back(t.size());
        timingsy.push_back(tdelta);
    }

    return std::make_pair(timingsx, timingsy);
}









int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <nodenum> <randseed> <insertsintervall> <repeats> <edgenum> <samplesintervall>" << std::endl;
        return 1;
    }

    int nodenum = std::stoi(argv[1]);
    int randseed = std::stoi(argv[2]);
    int insertsintervall = std::stoi(argv[3]);
    //int repeats = std::stoi(argv[4]);
    int edgenum = std::stoi(argv[5]);
    int samplesintervall = std::stoi(argv[6]);


    std::srand(randseed);


    auto [timingsx,timingsy]=test( nodenum, insertsintervall, edgenum, samplesintervall);

    for (auto timex : timingsx) {
        std::cout << timex << " ";
    }
    std::cout<<std::endl;

    for (auto timey : timingsy) {
        std::cout << timey << " ";
    }
    std::cout<<std::endl;

    return 0;
}