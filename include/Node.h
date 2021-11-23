// By Adam Caulfield (ac7717)
// October 25 2021

#ifndef PRIVPOINTLOC_NODE_H
#define PRIVPOINTLOC_NODE_H

#include <iostream>
#include <fstream>
#include <string>
#include "Tree.h"

class Tree;
class Segment;

enum NodeType {x, y, leaf}; 

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

    int leftCount;
    int rightCount;
    // x-nodes contain the point p and its two children correspond to points lying to left & right of p
    // y-nodes contain pointer to  line segment of the subdivision, left and right are regions above or below the line segment
private:
    Node * left;
    Node * right;
    Segment * s;
    Tree * tree;
    int value;
    NodeType type;
    bool isRight; // if NodeType x, is it right or left? Needed for printing the adjacency map
    int matrixIndex;
    std::string matrixLabel;
};

#endif /*PRIVPOINTLOC_NODE_H*/