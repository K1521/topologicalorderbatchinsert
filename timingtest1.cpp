
#include "topologicalordering.cpp"
//#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>
#include <fstream> 
#include <set>

int main()
{

  //for(int j=0;j<10000;++j){

    int nodenum=10000;

    //auto [randnodes, randedges]=makeGraph(nodenum,1000);
    std::srand(2345);

    std::vector<int> randnodes;
    randnodes.reserve(nodenum);
    for(int i=0;i<nodenum;i++)
        randnodes.emplace_back(i);
    std::random_shuffle(randnodes.begin(), randnodes.end());

    std::vector<std::pair<int, int>> randedges;
    randedges.emplace_back(nodenum*(nodenum-1)/2);
    for(int i=0;i<nodenum;i++){//this code generates a "full" graph (i mean it generetes a graph wich has a topological order with the maximum node number) 
        for(int j=i+1;j<nodenum;j++){
            randedges.emplace_back(std::make_pair(randnodes[i], randnodes[j]));
        }
    }
    std::random_shuffle(randedges.begin(), randedges.end());

    std::cout<<"#nodes:"<< randnodes.size()<<" #edge:"<< randedges.size()<<std::endl;


    //std::ofstream MyFile("testresult.txt");
    auto startall = std::chrono::high_resolution_clock::now();
    int myNumbers[12] = {1,2,4,8,16,128,1024,8192,65536,524288,4194304,33554432};
    for (int j = 0; j < 12; j++) {
        int insertsintervall=myNumbers[j];

        


        auto start = std::chrono::high_resolution_clock::now();

        topologicalordering<int> t;
        for(int i=0;i<randnodes.size();i++)
            t.addNode(randnodes[i]);

        
        for(int i=0;i<randedges.size();i++){
            if(i%insertsintervall==0){
                t.insertedges();
                
                //if (!t.validate())std::cout << "invallid" << std::endl;
            }
                
            
            auto [start,stop]=randedges[i];
            t.addedge(start,stop);
            
        }
        t.insertedges();
        auto end = std::chrono::high_resolution_clock::now();

        auto time=std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "time: " << time << " microseconds wich is " << ((float)time)/randnodes.size() <<" per edge    inserts every:"<<insertsintervall<< std::endl;
    }

    

    auto endall = std::chrono::high_resolution_clock::now();
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::microseconds>(endall - startall).count() << " microseconds total"<< std::endl;
    
    startall = std::chrono::high_resolution_clock::now();
    for(int i=0;i<randedges.size();i++){
        std::chrono::high_resolution_clock::now();
    }
    endall = std::chrono::high_resolution_clock::now();
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::microseconds>(endall - startall).count() << " microseconds total"<< std::endl;
    std::cout << "time: " << ((float)std::chrono::duration_cast<std::chrono::microseconds>(endall - startall).count())/randedges.size() << " microseconds total"<< std::endl;
    

    
    //auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(start_time2 - start_time).count();
    //auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time2).count();

    //std::cout << "Execution time1: " << duration1 << " microseconds" << std::endl;
    //std::cout << "Execution time2: " << duration2 << " microseconds" << std::endl;

    
}  



std::pair<std::vector<int>, std::vector<std::pair<int, int>>> 
makeGraph(int numNodes = 1000, int numEdges = INT_MAX,int seed) {
    std::srand(seed);

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
    std::mt19937 gen(seed);

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

std::pair<std::vector<int>, std::vector<std::pair<int, int>>> 
makefullgraph(int nodenum){
    std::vector<int> randnodes;
    randnodes.reserve(nodenum);
    for(int i=0;i<nodenum;i++)
        randnodes.emplace_back(i);
    std::random_shuffle(randnodes.begin(), randnodes.end());

    std::vector<std::pair<int, int>> randedges;
    randedges.emplace_back(nodenum*(nodenum-1)/2);
    for(int i=0;i<nodenum;i++){//this code generates a "full" graph (i mean it generetes a graph wich has a topological order with the maximum node number) 
        for(int j=i+1;j<nodenum;j++){
            randedges.emplace_back(std::make_pair(randnodes[i], randnodes[j]));
        }
    }
    std::random_shuffle(randedges.begin(), randedges.end());
    std::random_shuffle(randnodes.begin(), randnodes.end());
    return std::make_pair(randnodes, randedges);
}