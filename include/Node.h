/**
* Authored by Adam Caulfield (ac7717). 
* October 25 2021.
*/


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

/**
* enum NodeType has three types: x, y, leaf
* x --> x-nodes contain the point p and its two children correspond to points lying to left & right of p
* y --> y-nodes contain pointer to line segment of the subdivision, left and right are regions above or below the line segment
* leaf --> leaf-node represents a trapezoid, it has no children
*/
enum NodeType {x, y, leaf}; 

class Node {
public:
    Node(Tree *, Segment *, NodeType);
    
    /**
       * returns a pointer to the left child of this node
       * @param void
       * @return Node *
    */
    Node * getLeft();
    
    /**
       * returns a pointer to the right child of this node
       * @param void
       * @return Node *
    */
    Node * getRight();
    
    /**
       * returns a pointer to the Segment object which this Node references.
       * @param void
       * @return Segment *
    */
    Segment * getSegment();

    /**
       * Sets a the left child of the current node. Used during the DAG/Tree construction
       * @param Node *
       * @return void
    */
    void setLeft(Node *);
    
    /**
       * Sets a the right child of the current node. Used during the DAG/Tree construction
       * @param Node *
       * @return void
    */
    void setRight(Node *);
    
    /**
       * Sets a the Segment object of the current node. Used during the DAG/Tree construction
       * @param Node *
       * @return void
    */
    void setSegment(Segment *);

    /**
       * Return the pointer to the tree which contains this Node
       * @param void
       * @return Tree *
    */
    Tree * getTree();
    
    /**
       * Returns the index of this node in the adjacency matrix
       * @param void
       * @return int
    */
    int getMatrixIndex();
    
    /**
       * Returns the label of this node in the adjacency matrix
       * @param void
       * @return std::string
    */
    std::string getMatrixLabel();
    
    /**
       * Sets this Node's parameters for use in the adjacency matrix
       * @param int, std::string
       * @return void
    */
    void setMatrixParams(int, std::string);

    /**
       * Returns the 'value' parameter for this node. For a X-node, this is the x-value. For a Y-Node or Leaf-Node, this is the ID.
       * @param void
       * @return int
    */
    int getValue();

    /**
       * Sets the 'value' parameter for this node. For a X-node, this is the x-value. For a Y-Node or Leaf-Node, this is the ID.
       * @param int
       * @return void
    */
    void setValue(int);
    
    /**
       * Returns the NodeType value for this node. The enum NodeType has three possible values: x, y, or leaf. The node is set based upon its type in the DAG 
       * @param void
       * @return NodeType
    */
    NodeType getNodeType();
    
    /**
       * Sets the NodeType value for this node. The enum NodeType has three possible values: x, y, or leaf. The node is set based upon its type in the DAG 
       * @param NodeType
       * @return void
    */
    void setNodeType(NodeType);


    /**
       * Gets the boolean variable isRight, which is true or false based upon if the Node is the right (true) or left (false) child of another node.
       * This is a helper function when constructing the adjacency matrix.
       * @param void
       * @return bool
    */
    bool getIsRight();

    /**
       * Sets the boolean variable isRight, which is true or false based upon if the Node is the right (true) or left (false) child of another node.
       * This is a helper function when constructing the adjacency matrix.
       * @param bool
       * @return void
    */
    void setIsRight(bool);

    /**
       * Takes a std::string and accumulates to the path label This is a helper function when constructing the adjacency matrix.
       * @param std::string
       * @return void
    */
    void addPathLabel(std::string);
    
    /**
       * Returns the path labels. Only used for leaf nodes. Returns a vector of strings in the case there are multiple paths to the leaf.
       * @param void
       * @return std::vector<std::string>
    */
    std::vector<std::string> getPathLabels();

    /**
       * Evaluate this node. This is the function called in parallel for parallel-processing of all Y and X nodes.
       * Stores the temporary result in a helib::Ctxt, and this object has a pointer to that Ctxt as a parameter.
       * It takes in encryptor and privPointUtil objects in order to make use of the functions
       * It takes in the ctxt encoding & encrypting the user's point
       * It takes two integers representing the bitlength and the number of slots (indices) in the ciphertext (underlying vector).
       * @param Encryptor & encryptor, PrivPointUtil *, helib::Ctxt, int, int 
       * @return void
    */
    void evaluateNode(Encryptor &, PrivPointUtil *, helib::Ctxt, int, int);
    
    /**
       * This function is for when the paths are being evaluated. Assuming the node has already been evaluated, this returns the helib::Ctxt object which contains the evaluation result.
       * @param void
       * @return helib::Ctxt
    */
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