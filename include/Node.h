// By Adam Caulfield (ac7717)
// October 25 2021

#ifndef PRIVPOINTLOC_NODE_H
#define PRIVPOINTLOC_NODE_H

#include <iostream>
#include <fstream>
#include <string>
#include "Tree.h"
#include <vector>
#include <helib/helib.h>
#include "Encryptor.h"
#include "PrivPointUtil.h"

class Tree;
class Segment;

enum NodeType {x, y, leaf}; 
// x-nodes contain the point p and its two children correspond to points lying to left & right of p
// y-nodes contain pointer to line segment of the subdivision, left and right are regions above or below the line segment
// leaf-node represents a trapezoid, it has no children

class Node {
public:
    Node(Tree *, Segment *, NodeType);
    
    Node * getLeft();
    Node * getRight();
    Segment * getSegment();

    void setLeft(Node *);
    void setRight(Node *);
    void setSegment(Segment *);

    Tree * getTree();
    
    int getMatrixIndex();
    std::string getMatrixLabel();
    void setMatrixParams(int, std::string);

    int getValue();
    void setValue(int);
    
    NodeType getNodeType();
    void setNodeType(NodeType);

    bool getIsRight();
    void setIsRight(bool);

    void addPathLabel(std::string);
    std::vector<std::string> getPathLabels();

    void evaluateNode(Encryptor &, PrivPointUtil *, helib::Ctxt, int, int);
    helib::Ctxt getNodeResult();
private:
    Node * left;
    Node * right;
    Segment * s;
    Tree * tree;
    int value;
    NodeType type;
    bool isRight; // if NodeType x, is it right or left? Needed for printing the adjacency map
    int matrixIndex;
    std::string matrixLabel; // for printing the adjacency matrix
    std::vector<std::string> pathLabels; // for parallel processing of privPointLoc
    helib::Ctxt * result;
};

#endif /*PRIVPOINTLOC_NODE_H*/