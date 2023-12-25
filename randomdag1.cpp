#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <set>

std::pair<std::vector<std::string>, std::vector<std::pair<std::string, std::string>>>
makeGraph(int numNodes = 1000, int connsPerNode = 1) {
    std::vector<std::string> nodes;
    for (int i = 0; i < numNodes; ++i) {
        nodes.push_back(std::to_string(i));
    }
    std::random_shuffle(nodes.begin(), nodes.end());

    std::vector<std::pair<std::string, std::string>> connections;
    std::random_device rd;
    std::mt19937 gen(rd());

    for (int i = 0; i < connsPerNode * numNodes; ++i) {
        int a = std::uniform_int_distribution<int>(0, numNodes - 2)(gen);
        int b = std::uniform_int_distribution<int>(a + 1, numNodes - 1)(gen);

        connections.push_back(std::make_pair(nodes[a], nodes[b]));
    }

    // Remove duplicates using std::set
    std::set<std::pair<std::string, std::string>> uniqueConnections(connections.begin(), connections.end());
    connections.assign(uniqueConnections.begin(), uniqueConnections.end());

    std::random_shuffle(connections.begin(), connections.end());
    std::random_shuffle(nodes.begin(), nodes.end());

    return std::make_pair(nodes, connections);
}

int main() {
    // Example usage:
    auto result = makeGraph(20);
    const auto& nodes = result.first;
    const auto& connections = result.second;

    // Print nodes
    std::cout << "Nodes: ";
    for (const auto& node : nodes) {
        std::cout << node << " ";
    }
    std::cout << std::endl;

    // Print connections
    std::cout << "Connections: ";
    for (const auto& connection : connections) {
        std::cout << "(" << connection.first << ", " << connection.second << ") ";
    }
    std::cout << std::endl;

    return 0;
}