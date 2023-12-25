#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <iterator>  
#include <unordered_map>  
//#include <thread>
//#include <mutex>
using namespace std;




template <typename T>
class Node {
public:
    bool vacant = false;
    bool onStack = false;
    int ord;
    std::vector<Node<T>*> children;
    std::vector<Node<T>*> childreninv;
    T value;

    Node(int ord,T value) : ord(ord), value(value) {}

    friend class topologicalordering<T>;
};



template <typename T>
class topologicalordering{
    public://TODO change to private
        //vector<Node*> nodes=vector<Node*>();
        vector<Node<T>*> ordinv;
        std::unordered_map< T, Node<T>* > ValueToNode;
};


int main()
{
    topologicalordering<int> t;
}   
