// my_cpp_program.cpp
//#include "topologicalordering.h"
#include "topord.cpp"
#include "randgraph.cpp"


#include <iostream>
#include <chrono>
//#include <vector>

#include <random>
#include <string>

//#include <algorithm>

//#include <gperftools/profiler.h>
// Your C++ code goes here...






std::pair<std::vector<int>, std::vector<long long>> 
testwithdict(int nodenum,int insertsintervall,int maxedgenum,int samplesintervall,bool insertnodes,std::mt19937 gen){
        auto [randnodes, randedges]=makeGraph(nodenum,maxedgenum,gen);

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
testhandles(int nodenum,int insertsintervall,int maxedgenum,int samplesintervall,std::mt19937 gen){
        auto [randnodes, randedges]=makeGraph(nodenum,maxedgenum,gen);

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
test(int nodenum,int insertsintervall,int maxedgenum,int samplesintervall,int insertnodes,std::mt19937 gen){
    if(insertnodes==0){
        return testwithdict(nodenum, insertsintervall, maxedgenum, samplesintervall,false,gen);
    }else if(insertnodes==1){
        return testwithdict(nodenum, insertsintervall, maxedgenum, samplesintervall,true,gen);
    }else{
        return testhandles(nodenum, insertsintervall, maxedgenum, samplesintervall,gen);
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

    if(repeats<1){
        throw std::invalid_argument("repeats must be greater than or equal to 1");
    }
    /*int nodenum = 1000000;
    int randseed = 42;
    int insertsintervall = 1000;
    //int repeats = std::stoi(argv[4]);
    int edgenum = nodenum;
    int samplesintervall = nodenum/1000;*/

    //std::srand(randseed);
    std::mt19937 gen(randseed);

    auto [timingsx, cumulativeTimings] = test(nodenum, insertsintervall, edgenum, samplesintervall,insertnodes,gen);

    // Repeat the test 'repeats' times and accumulate the results
    for (int i = 1; i < repeats; ++i) {
        auto [_, timingsy] = test(nodenum, insertsintervall, edgenum, samplesintervall,insertnodes,gen);

        // Accumulate timingsy
        for (std::size_t j = 0; j < timingsy.size(); ++j) {
            cumulativeTimings[j] += timingsy[j];
        }
    }

    // Average the accumulated timingsy
    std::vector<float> averagedTimingsFloat(cumulativeTimings.begin(), cumulativeTimings.end());
    for (std::size_t i = 0; i < averagedTimingsFloat.size(); ++i) {
        averagedTimingsFloat[i] /= repeats;
    }

    // Print timingsx
    for (auto timex : timingsx) {
        std::cout << timex << " ";
    }
    std::cout << std::endl;

    // Print averaged timingsy
    for (auto timey : averagedTimingsFloat) {
        std::cout << timey << " ";
    }
    std::cout << std::endl;

    return 0;
}