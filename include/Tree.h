/**
* Authored by Adam Caulfield (ac7717). 
* October 25 2021.
*/

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

    /**
       * Read the segments file, create Segments objects, insert into Tree
       * @param std::string filename
       * @return void
    */
    void readSegmentsFile(std::string);


    /**
       * Return a pointer to the Node which is the root of the Tree
       * @param void
       * @return Node *
    */
    Node * getRoot();


    /**
       * Get total Nodes in the tree
       * @param void
       * @return int
    */
    int getTreeSize();


    /**
       * Get the total leaf nodes in the tree
       * @param void
       * @return int
    */
    int getTotalTrapezoids();


    /**
       * Insert a segment into the tree
       * @param Segment *
       * @return void
    */
    void insert(Segment *);


    /**
       * Print the tree in the console
       * @param Node * startNode - starting with this node as a root
       * @param int=0 depth - starting depth
       * @return void
    */
    void printTree(Node *, int = 0);

    /**
       * Traverse the tree recursively after new X and Y nodes have been added, and fill in new leaf nodes
       * @param Node * startNode - starting from this node then traverse to all children
       * @return void
    */
    void addLeafNodes(Node *);
    
    /**
       * Delete leaf nodes in the subtree specified by the Node input
       * @param Node * startNode - starting from this node then traverse to all children
       * @return void
    */
    void deleteLeafNodes(Node *);

    /**
       * Initialize all lists for creating the adjacency matrix and the lists of paths
       * @param Node * startNode - starting from this node then traverse to all children
       * @return void
    */
    void setupLists(Node *);

    /**
       * Print all lists: Q-Nodes, P-Nodes, S-Nodes, T-Nodes
       * @param void
       * @return void
    */
    void printLists();

    /**
       * Initialize the adjacency matrix as a 2D vector based upon the number of nodes
       * @param void
       * @return void
    */
    void initAdjacencyMatrix();

    /**
       * Write to the adjacency matrix the labels of the nodes and connections between nodes
       * @param Node * startNode - starting from this node then traverse to all children
       * @return void
    */
    void writeAdjacencyMatrix(Node *);

    /**
       * Fill in the sum row/column of the adjacency matrix
       * @param void
       * @return void
    */
    void writeSumsAdjacencyMatrix();
    
    /**
       * Print the adjacency matrix
       * @param void
       * @return void
    */
    void printAdjacencyMatrix();

    /**
       * Write the adjacency matrix to a file
       * @param std::string filename
       * @return void
    */
    void writeAdjacencyMatrixToFile(std::string);

    /**
       * Plaintext version of planar point location
       * @param int x 
       * @param int y
       * @param int Node * startNode  
       * @return void
    */
    void findPoint(int, int, Node *);

    /**
       * Privacy Preserving planar point location -- NO Parallelization. Returns a ciphertext with the result. Runs without any parallelization
       * @param Encryptor & - encryptor object
       * @param PrivPointUtil * - privPointUtil object pointer
       * @param helib::Ctxt pointCtxt - ciphertext encoding the point [--x--, --y--]
       * @param helib::Ctxt & resultCtxt - accumulates the result
       * @return void (result updated by reference)
    */
    void findPrivatePoint(Encryptor &, PrivPointUtil *, helib::Ctxt , helib::Ctxt &, helib::Ctxt, Node *, int, int);
    
    /**
       * Clean Tree updates the IDs of the Leaf nodes so they start from 0 and end at totalTrapezoids-1
       * @param Node * startNode - traverse starting from this node as root
       * @return void
    */
    void cleanTree(Node *);

    /**
       * Setup the path labels. Passes a Node pointer and string as parameters. It is a recursive function. If the current node is not a leaf node
       * it calls setupPathLabels(startNode->left, label+"0") for its left child, and setupPathLabels(startNode->right, label+"1") for the right.
       * When reaching a leaf node, add the path label to stored labels. Also adds to allPaths vector stored by the Tree object.
       * @param Node * startNode - traverse starting from this node as root
       * @param std::string label - carries the path label
       * @return void
    */
    void setupPathLabels(Node *, std::string);

    /**
       * Print the path labels for all Leaf nodes
       * @param void
       * @return void
    */
    void printPathLabels();

    /**
       * Returns the allPaths vector, containing all paths in the structure
       * @param void
       * @return std::vector<std::string> allPaths
    */
    std::vector<std::string> getAllPaths();
    
    /**
       * Evaluates just one Path homomorphically - used for debugging one path.
       * @param Encryptor & - encryptor object
       * @param PrivPointUtil * - privPointUtil object pointer
       * @param helib::Ctxt pointCtxt - ciphertext encoding the point [--x--, --y--]
       * @param helib::Ctxt & resultCtxt - accumulates the result
       * @param int maxBits - bit length of encoded binary numbers
       * @param int nSlots - slot count of the ciphertexts (the size of vector underneath the ciphertext)
       * @param std::string - label of the path to test
       * @return void (result updated by reference)
    */
    void evaluateOnePath(Encryptor &, PrivPointUtil *, helib::Ctxt, helib::Ctxt &, int, int, std::string);

    /**
       * Used for parallel processing of the paths. This function is used after all nodes have been evaluated and assumes they store the correct result
       * This function traverses the path accumulating the node results (through AND/*).
       * @param Encryptor & - encryptor object
       * @param PrivPointUtil * - privPointUtil object pointer
       * @param helib::Ctxt pointCtxt - ciphertext encoding the point [--x--, --y--]
       * @param helib::Ctxt & resultCtxt - accumulates the result
       * @param int maxBits - bit length of encoded binary numbers
       * @param int nSlots - slot count of the ciphertexts (the size of vector underneath the ciphertext)
       * @param std::string - label of the path
       * @return void (result updated by reference)
    */
    void getPathResult(Encryptor &, PrivPointUtil *, helib::Ctxt, helib::Ctxt &, int, int, std::string);

    /**
       * Main function to run the parallel point location. First evaluates all the nodes in parallel, then calls getPathResult for each path
       * in parallel. Uses OpenMP to coordinate parallelization and accumulate results in one ciphertext to return to the user.
       * @param Encryptor & - encryptor object
       * @param PrivPointUtil * - privPointUtil object pointer
       * @param helib::Ctxt pointCtxt - ciphertext encoding the point [--x--, --y--]
       * @param int maxBits - bit length of encoded binary numbers
       * @param int nSlots - slot count of the ciphertexts (the size of vector underneath the ciphertext)
       * @return helib::Ctxt - result
    */
    helib::Ctxt evaluateAllNodes(Encryptor &, PrivPointUtil *, helib::Ctxt, int, int);
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
    int minX, maxX;
};

#endif /*PRIVPOINTLOC_TREE_H*/