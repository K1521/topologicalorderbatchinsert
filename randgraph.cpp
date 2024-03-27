#include <set>
#include <random>
#include <iostream>
#include <algorithm> // for std::shuffle

std::pair<std::vector<int>, std::vector<std::pair<int, int>>> 
makefullgraph(int nodenum,std::mt19937 gen){
    std::vector<int> randnodes;
    randnodes.reserve(nodenum);
    for(int i=0;i<nodenum;i++)
        randnodes.emplace_back(i);
    std::shuffle(randnodes.begin(), randnodes.end(),gen);

    std::vector<std::pair<int, int>> randedges;
    randedges.reserve(nodenum*(nodenum-1)/2);
    for(int i=0;i<nodenum;i++){//this code generates a "full" graph (i mean it generetes a graph wich has a topological order with the maximum node number) 
        for(int j=i+1;j<nodenum;j++){
            randedges.emplace_back(std::make_pair(randnodes[i], randnodes[j]));
        }
    }
    std::shuffle(randedges.begin(), randedges.end(),gen);
    std::shuffle(randnodes.begin(), randnodes.end(),gen);
    return std::make_pair(randnodes, randedges);
}


std::pair<std::vector<int>, std::vector<std::pair<int, int>>> 
makeGraph(int numNodes , int numEdges,std::mt19937 gen) {
    

    if(numEdges>(long long)numNodes*(numNodes-1)/4){
        //std::cout <<((long long)numNodes)*(numNodes-1)/4<<std::endl;
        auto [nodes,edges]=makefullgraph(numNodes,gen);
        edges.resize(std::min(numEdges,(int)edges.size()));
        return std::make_pair(nodes,edges);
    }

    std::vector<int> randnodes;
    for (int i = 0; i < numNodes; ++i) {
        randnodes.push_back(i);
    }
    std::shuffle(randnodes.begin(), randnodes.end(),gen);

    
    std::set<std::pair<int, int>> edgesSet;

    while (edgesSet.size() < numEdges) {
        int a = std::uniform_int_distribution<int>(0, numNodes - 2)(gen);
        int b = std::uniform_int_distribution<int>(a + 1, numNodes - 1)(gen);

        edgesSet.insert(std::make_pair(randnodes[a], randnodes[b]));
    }
    std::vector<std::pair<int, int>> randedges( edgesSet.begin(), edgesSet.end());



    std::shuffle(randedges.begin(), randedges.end(),gen);
    std::shuffle(randnodes.begin(), randnodes.end(),gen);

    return std::make_pair(randnodes, randedges);
}
