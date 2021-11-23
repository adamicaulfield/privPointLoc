// By Adam Caulfield (ac7717)
// October 25 2021

#ifndef PRIVPOINTLOC_TREE_H
#define PRIVPOINTLOC_TREE_H

#include <iostream>
#include <fstream>

#include "Node.h"
#include "Segment.h"

// handle cyclic dependencies
class Node; 
class Segment;

class Tree {
public:
	Tree();
    void readSegmentsFile(std::string);
    Node * getRoot();
    int getTreeSize();
    void insert(Segment *);
    void printTree(Node *, int = 0);
    void addLeafNodes(Node *);
    void deleteLeafNodes(Node *);
    void setupLists(Node *);
    void initAdjacencyMatrix();
    void writeAdjacencyMatrix(Node *);
    void wrireSumsAdjacencyMatrix();
    void printAdjacencyMatrix();
    void writeAdjacencyMatrixToFile(std::string);
    void findPoint(int, int, Node *);
private:
    Node * root;
    int size; //total nodes
    int totalTrapezoids;
    int totalSegments;
    std::vector<Node *> pList;
    std::vector<Node *> qList;
    std::vector<Node *> sList;
    std::vector<Node *> tList;
    std::vector<std::vector<std::string>> adjMatrix;
};

#endif /*PRIVPOINTLOC_TREE_H*/