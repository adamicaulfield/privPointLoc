// By Adam Caulfield (ac7717)
// October 25 2021

#include "Node.h"

privPointLoc::Node::Node(Tree * t, int v){
	leftNode = nullptr;
	rightNode = nullptr;
	tree = t;
	value = v;
	leftCount = 0;
	rightCount = 0;
}

void privPointLoc::Node::setValue(int v){
	value = v;
}

int privPointLoc::Node::getValue(){
	return value;
}

// Get pointer to  left node
privPointLoc::Node * privPointLoc::Node::getLeft(){
	return leftNode;
}

// Get pointer to right node
privPointLoc::Node * privPointLoc::Node::getRight(){
	return rightNode;
}

// Get pointer to  left node
void privPointLoc::Node::setLeft(privPointLoc::Node * node){
	leftNode = node;
}

// Get pointer to right node
void privPointLoc::Node::setRight(privPointLoc::Node * node){
	rightNode = node;
}


// Get pointer to tree, which this node is in
privPointLoc::Tree * privPointLoc::Node::getTree(){
	return tree;
}

