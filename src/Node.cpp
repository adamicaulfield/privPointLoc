// By Adam Caulfield (ac7717)
// October 25 2021

#include "Node.h"

Node::Node(Tree * t, Segment * seg, NodeType ty){
	left = nullptr;
	right = nullptr;
	s = seg;
	tree = t;
	type = ty;
	leftCount = 0;
	rightCount = 0;
}

void Node::setValue(int v){
	value = v;
}

int Node::getValue(){
	return value;
}

void Node::setNodeType(NodeType t){
	type = t;
}

NodeType Node::getNodeType(){
	return type;
}

// Get pointer to  left node
Node * Node::getLeft(){
	return left;
}

// Get pointer to right node
Node * Node::getRight(){
	return right;
}

// Get pointer to  left node
void Node::setLeft(Node * node){
	left = node;
}

// Get pointer to right node
void Node::setRight(Node * node){
	right = node;
}

Segment * Node::getSegment(){
	return s;
}

// Get pointer to tree, which this node is in
Tree * Node::getTree(){
	return tree;
}

void Node::setIsRight(bool b){
	isRight = b;
}

bool Node::getIsRight(){
	return isRight;
}

void Node::setMatrixParams(int v, std::string s){
	matrixIndex = v;
	matrixLabel = s;
}

int Node::getMatrixIndex(){
	return matrixIndex;
}

std::string Node::getMatrixLabel(){
	return matrixLabel;
}