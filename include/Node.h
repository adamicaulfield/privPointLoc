// By Adam Caulfield (ac7717)
// October 25 2021

#ifndef PRIVPOINTLOC_NODE_H
#define PRIVPOINTLOC_NODE_H

#include <iostream>
#include <fstream>

#include "Tree.h"

namespace privPointLoc {

    class Tree; // handle cyclic dependency betewen Node and Tree

    class Node {
    public:
        Node(Tree *, int);
        Node * getLeft();
        Node * getRight();
        void setLeft(Node *);
        void setRight(Node *);
        Tree * getTree();
        int getValue();
        void setValue(int);
        int leftCount;
        int rightCount;
    private:
        Node * leftNode;
        Node * rightNode;
        Tree * tree;
        int value;
    };

}

#endif //PRIVPOINTLOC_NODE_H
