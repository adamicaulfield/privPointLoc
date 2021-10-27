// By Adam Caulfield (ac7717)
// October 25 2021

#include "Tree.h"
#include "Node.h"
#include <cmath>

privPointLoc::Tree::Tree(){
	size = 0;
	root = NULL;
}

// Get node at the top of the tree
privPointLoc::Node * privPointLoc::Tree::getRoot(){
	return root;
}

int privPointLoc::Tree::getTreeSize(){
	return size;
}

void privPointLoc::Tree::insertNode(privPointLoc::Node * node){
	if(root==nullptr){
		// printf("\tNode is first, now \'root\'\n");
		root = node;
	} else{
		privPointLoc::Node * checkNode = root;
		privPointLoc::Node * lastNode = root;
		while(checkNode != nullptr){
			lastNode = checkNode;
			if(node->getValue() <= checkNode->getValue()){
				checkNode->leftCount++;
				checkNode = checkNode->getLeft();

				// printf("moving left\n");

			} else {
				checkNode->rightCount++;
				checkNode = checkNode->getRight();
				// printf("moving right\n");
			}
		}

		if(node->getValue() <= lastNode->getValue()){
			lastNode->setLeft(node);
			// printf("\tAdding to left\n");

		} else{
			lastNode->setRight(node);
			// printf("\tAdding to right\n");
		}
	}
	size++;
	// printf("Balancing tree...\n");
	balanceTree(root);
	printf("---------- After insert ----------\n");
	printTree(root);
	printf("----------------------------------\n");
}

void privPointLoc::Tree::balanceTree(Node * z){
	/*
	* Inspiration from: https://www.geeksforgeeks.org/avl-tree-set-1-insertion/
	* In this explanation, z and y are nodes. Node z identifies there is unbalance,
	* and Node y determines the type of rotation needed.
	*/

	int bf = z->rightCount - z->leftCount;

	if(bf > 1){ //right
		printf("--RIGHT UNBALANCED--\n");
		Node * y = z->getRight();
		int bfy = y->rightCount - y->leftCount;		
		if(bfy > 0){ // right right case
			printf("--handling right-right case--\n");
			// Rotate the tree to the left pivoting on Y
			Node * yLeft = y->getLeft();
			z->setRight(yLeft);
			y->setLeft(z);

			//if z was previously the root, update the root to be y;
			if(root == z){
				root = y;
			}

		} 
		// else if(bfr < -1){ // left right case
					
		// }
	} 
	// else if(bf < -1){ // left
	// 	Node * y = z->getRight();
	// 	int bfl = y->rightCount - z->leftCount;
	// 	if(bfl>1){ // right left case
			
	// 	} else if(bfl < -1){ // left left case
			
	// 	}
	// } else{
	// 	return;
	// }
}


void privPointLoc::Tree::printTree(Node * startNode, int depth){
	for(int i=0; i<depth; i++){
		printf("\t");
	}
	depth++;
	printf("Node(val=%d,left=%d,right=%d)\n", startNode->getValue(), startNode->leftCount, startNode->rightCount);
	if(startNode->getRight()!=nullptr){
		printf("right: ");
		printTree(startNode->getRight(), depth+1);
	}

	if(startNode->getLeft()!=nullptr){
		printf("left: ");
		printTree(startNode->getLeft(), depth+1);
	}
}