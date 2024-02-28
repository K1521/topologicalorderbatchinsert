#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

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
    std::random_shuffle(nodes.begin(), nodes.end());

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

int main() {
    // Example usage:
    auto result = makeGraph(10);
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