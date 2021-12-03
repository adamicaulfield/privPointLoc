// By Adam Caulfield (ac7717)
// October 25 2021

#ifndef PRIVPOINTLOC_TREE_H
#define PRIVPOINTLOC_TREE_H

#include <iostream>
#include <fstream>

#include "Node.h"
#include "Segment.h"
#include "Encryptor.h"
#include "PrivPointUtil.h"

// handle cyclic dependencies
class Node; 
class Segment;

class Tree {
public:
	Tree();
    void readSegmentsFile(std::string);
    Node * getRoot();
    int getTreeSize();
    int getTotalTrapezoids();
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
    void findPrivatePoint(Encryptor &, PrivPointUtil *, helib::Ctxt , helib::Ctxt &, helib::Ctxt, Node *, int, int);
    void findPrivatePoint2(Encryptor &, PrivPointUtil *, helib::Ctxt , helib::Ctxt &, helib::Ctxt, Node *, int, int);
    // void findPrivatePoint2(Encryptor &, PrivPointUtil *, helib::Ctxt , helib::Ctxt &, helib::Ctxt, int, int);
    void cleanTree(Node *);
    void setupPathLabels(Node *, std::string);
    void printPathLabels();
    std::vector<std::string> getAllPaths();
    void evaluatePath(Encryptor &, PrivPointUtil *, helib::Ctxt, helib::Ctxt &, int, int, std::string);
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
    std::vector<std::string> allPaths;
};

#endif /*PRIVPOINTLOC_TREE_H*/