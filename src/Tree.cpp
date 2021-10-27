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
	printf("Balancing tree...\n");
	balanceTree(root);
}

void privPointLoc::Tree::balanceTree(Node * curNode){
	int bf = curNode->rightCount - curNode->leftCount;

	if(bf > 1){ //right
		Node * nextNode = curNode->getRight();
		int bfr = nextNode->rightCount - curNode->leftCount;
		if(std::abs(bfr)>1){
			balanceTree(nextNode);
		} else{
			// rotate left
		}
	} else if(bf < -1){ // left
		Node * nextNode = curNode->getLeft();
		int bfr = nextNode->rightCount - curNode->leftCount;
		if(std::abs(bfr)>1){
			balanceTree(nextNode);
		} else{
			// rotate right
		}
	}

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