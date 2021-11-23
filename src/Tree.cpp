// By Adam Caulfield (ac7717)
// October 25 2021

#include "Tree.h"
#include "Node.h"
#include <cmath>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

Tree::Tree(){
	size = 0;
	root = NULL;
	totalTrapezoids = 0;
}

// Process Input file, feed segments into tree
void Tree::readSegmentsFile(std::string filename){
	std::ifstream segFile(filename);
	std::string line;
	std::string delim = " ";
	std::string coord;
	int lineNum = 0;
	int xl, yl, xr, yr;
	if(segFile.is_open()){
		while(getline(segFile, line)){
			if(lineNum >= 2){
				std::stringstream ss;
				ss << line;
				ss >> xl >> yl >> xr >> yr;
				printf("ADDING SEGMENT: (%d, %d)-(%d, %d)\n", xl, yl, xr, yr);
				insert(new Segment(xl, yl, xr, yr));
			}
			lineNum++;
		}
		segFile.close();
	}
	else{
		std::cout << "Could not open file" << std::endl;
	}
}

// TREE / DAG FUNCTIONS
Node * Tree::getRoot(){
	return root;
}

int Tree::getTreeSize(){
	return size;
}

void Tree::insert(Segment * s){
	printf("beginning insert()....\n");

	//printf("getting left and right endpoints from segment\n");
	std::vector<int> leftEndPoint = s->getLeftEndpoint();
	std::vector<int> rightEndPoint = s->getRightEndpoint();

	int xl = leftEndPoint[0];
	int yl = leftEndPoint[1];
	int xr = rightEndPoint[0];
	int yr = rightEndPoint[1];


	Node * r = new Node(this, s, NodeType::x);
	Node * l = new Node(this, s, NodeType::x);

	r->setIsRight(true);
	l->setIsRight(false);

	l->setValue(xl);		
	r->setValue(xr);

	Node * seg = new Node(this, s, NodeType::y);
	seg->setValue(totalSegments);
	s->setSegID(totalSegments);

	if(size == 0){
		printf("\tAdding seg to tree, tree has size = 0\n");
		printf("LEFT ENDPOINT: (%d,%d)\n", xl, yl);
		printf("RIGHT ENDPOINT: (%d,%d)\n", xr, yr);
		// Add point nodes
		if(yl > yr){

			root = l;
			l->setRight(r);

			// Add segment node
			r->setLeft(seg);
			addLeafNodes(root);
		} else{
			root = r;
			r->setLeft(l);

			// Add segment node
			l->setRight(seg);
			addLeafNodes(root);
		}

	} else{
		printf("\tAdding seg to tree, tree has size > 0\n");
		// Locate trapezoid leaf node which holds the left endpoint
		Node * searchNode = root;
		Node * prevNode;
		NodeType searchNodeType = searchNode->getNodeType();
		bool dir = true;
		// printf("Starting LEFT while loop...\n");
		while(searchNodeType != NodeType::leaf){
			prevNode = searchNode;
			switch(searchNodeType){	
				case NodeType::x:
					if(xl <= searchNode->getValue()){
						searchNode = searchNode->getLeft();
						dir = true;
					} else{
						searchNode = searchNode->getRight();
						dir = false;
					}
					break;
				
				case NodeType::y:
					if(yl <= searchNode->getSegment()->getYonSeg(xl)){
						searchNode = searchNode->getLeft();
						dir = true;
					} else{
						searchNode = searchNode->getRight();
						dir = false;
					}
					break;
			}
			searchNodeType = searchNode->getNodeType();
		}
		// printf("Finished LEFT while loop...\n");
		// searchNode now holds leaf node, prevNode now holds its parent

		// replace leaf node with the new x-node
		if(dir){ //left
			prevNode->setLeft(l);
			delete(searchNode);

		} else{
			prevNode->setRight(l);
			delete(searchNode);
		}
		l->setRight(seg);
		addLeafNodes(l);

		//Locate trapezoid leaf node which holds the right endpoint
		searchNode = root;
		searchNodeType = searchNode->getNodeType();
		dir = true;
		// printf("Starting RIGHT while loop...\n");
		while(searchNodeType != NodeType::leaf){
			prevNode = searchNode;
			switch(searchNodeType){	
				case NodeType::x:
					if(xr < searchNode->getValue()){
						searchNode = searchNode->getLeft();
						dir = true;
					} else{
						searchNode = searchNode->getRight();
						dir = false;
					}
					break;
				
				case NodeType::y:
					if(yr < searchNode->getSegment()->getYonSeg(xr)){
						searchNode = searchNode->getLeft();
						dir = true;
					} else{
						searchNode = searchNode->getRight();
						dir = false;
					}
					break;
			}
			searchNodeType = searchNode->getNodeType();
		}
		// printf("Finished RIGHT while loop...\n");

		if(dir){ //left
			prevNode->setLeft(r);
			delete(searchNode);
		} else{
			prevNode->setRight(r);
			delete(searchNode);
		}
		// Node * seg = new Node(this, s, NodeType::y);
		// seg->setValue(totalSegments);
		// s->setSegID(totalSegments);
		r->setLeft(seg);
		addLeafNodes(r);
		// searchNode now holds leaf node, prevNode now holds its parent

	}
	// printf("beginning addLeafNodes()....\n");
	// addLeafNodes(root);
	// printf("finished addLeafNodes()....\n");
	// printf("finished insert()....\n");
	
	totalSegments++;
	size++;
	printf("Done\n");
}

void Tree::addLeafNodes(Node * node){
	if(node != nullptr){
		if(node->getLeft() == nullptr){
			Node * leaf = new Node(this, nullptr, NodeType::leaf);
			leaf->setValue(totalTrapezoids);
			totalTrapezoids++;
			node->setLeft(leaf);
		} else{
			if(node->getLeft()->getNodeType() != NodeType::leaf){
				addLeafNodes(node->getLeft());	
			}
		}

		if(node->getRight() == nullptr){
			Node * leaf = new Node(this, nullptr, NodeType::leaf);
			leaf->setValue(totalTrapezoids);
			totalTrapezoids++;
			node->setRight(leaf);
		} else{
			if(node->getRight()->getNodeType() != NodeType::leaf){
				addLeafNodes(node->getRight());
			}
		}
	} else{
		return;
	}
}

void Tree::deleteLeafNodes(Node * node){
	if(node != nullptr){
		if(node->getLeft() != nullptr){
			if(node->getLeft()->getNodeType() == NodeType::leaf){
				Node * leaf = node->getLeft();
				node->setLeft(nullptr);
				delete(leaf);
			} else {
				deleteLeafNodes(node->getLeft());
				totalTrapezoids--;
			}
		} else{
			return;
		}

		if(node->getRight() != nullptr){
			if(node->getRight()->getNodeType() == NodeType::leaf){
				Node * leaf = node->getRight();
				node->setRight(nullptr);
				delete(leaf);
			} else {
				deleteLeafNodes(node->getRight());
				totalTrapezoids--;
			}
		} else{
			return;
		}
	}
}

void Tree::printTree(Node * startNode, int depth){
	depth++;
	switch(startNode->getNodeType()){
		case NodeType::x:
			printf("Node(type:x, val:%d)\n", startNode->getValue());
			break;
		case NodeType::y:
			printf("Node(type:y, segID:%d)\n", startNode->getValue());
			break;
		case NodeType::leaf:
			printf("Node(type:leaf, tpzdID=%d)\n", startNode->getValue());
			break;
	}
	
	if(startNode->getRight()!=nullptr){
		for(int i=0; i<depth; i++){
			printf("\t");
		}
		printf("R: ");
		printTree(startNode->getRight(), depth+1);
	}

	if(startNode->getLeft()!=nullptr){
		for(int i=0; i<depth; i++){
			printf("\t");
		}
		printf("L: ");
		printTree(startNode->getLeft(), depth+1);
	}
}


// Functions to convert tree into adjacency matrix, and write to file
void Tree::setupLists(Node * startNode){
	if(startNode != nullptr){
		NodeType nt = startNode->getNodeType();
		bool notInList = true;
		switch(nt){
			case NodeType::x:
				if(startNode->getIsRight()){
					pList.push_back(startNode);
				} else {
					qList.push_back(startNode);
				}
				break;
			case NodeType::y:
				sList.push_back(startNode);
				break;
			case NodeType::leaf:
				for(int t=0; t<tList.size(); t++){
					notInList = notInList && (startNode != tList[t]);
				}
				if(notInList){
					tList.push_back(startNode);	
				}
				break;
			default:
				break;				
		}

		if(startNode->getLeft() != nullptr){
			setupLists(startNode->getLeft());
		}

		if(startNode->getRight() != nullptr){
			setupLists(startNode->getRight());
		}
	}
}

void Tree::initAdjacencyMatrix(){
	
	int dim = pList.size()+qList.size()+sList.size()+tList.size();
	
	std::vector<std::vector<std::string>> matrix(dim+2);
	for(int i=0; i<dim+2; i++){
		std::vector<std::string> tmp(dim+2);
		for(int j=0; j<dim+2; j++){
			tmp[j] = "0";
		}
		matrix[i] = tmp;
	}
	matrix[0][0] = " "; //empty
	// printf("Matrix dims: (%d x %d)\n", (int)matrix.size(), (int)matrix[0].size());

	// printf("\tSet header row ...\n");
	// Set header row

	
	int id;
	int offset = 1;

	std::string label = "P";
	for(int i=0; i<pList.size(); i++){
		id = pList[i]->getSegment()->getSegID();
		
		matrix[0][i+offset] = label+std::to_string(id);
		matrix[i+offset][0] = label+std::to_string(id);

		pList[i]->setMatrixParams(i+offset, label+std::to_string(id));
	}
	offset += pList.size();

	label = "Q";
	for(int i=0; i<qList.size(); i++){
		id = qList[i]->getSegment()->getSegID();
		
		matrix[0][i+offset] = label+std::to_string(id);
		matrix[i+offset][0] = label+std::to_string(id);

		qList[i]->setMatrixParams(i+offset, label+std::to_string(id));
	}
	offset += qList.size();

	label = "S";
	for(int i=0; i<sList.size(); i++){
		id = sList[i]->getValue();
		
		matrix[0][i+offset] = label+std::to_string(id);
		matrix[i+offset][0] = label+std::to_string(id);

		sList[i]->setMatrixParams(i+offset, label+std::to_string(id));
	}
	offset += sList.size();

	label = "T";
	for(int i=0; i<tList.size(); i++){
		id = tList[i]->getValue();
		
		matrix[0][i+offset] = label+std::to_string(id);
		matrix[i+offset][0] = label+std::to_string(id);

		tList[i]->setMatrixParams(i+offset, label+std::to_string(id));
	}

	matrix[0][dim+1] = "Sum";
	matrix[dim+1][0] = "Sum";

	adjMatrix = matrix;
}

void Tree::writeAdjacencyMatrix(Node * startNode){
	if(startNode != nullptr){
		int curNodeIdx = startNode->getMatrixIndex();
		if(startNode->getLeft() != nullptr){
			int leftNodeIdx = startNode->getLeft()->getMatrixIndex();
			// std::cout << "adjMatrix[" << startNode->getMatrixLabel() << "][" << startNode->getLeft()->getMatrixLabel() << "] = 1" << std::endl;
			// std::cout << "adjMatrix[" << startNode->getLeft()->getMatrixLabel() << "][" << startNode->getMatrixLabel() << "] = 1" << std::endl;
			adjMatrix[curNodeIdx][leftNodeIdx] = "1";
			adjMatrix[leftNodeIdx][curNodeIdx] = "1";
			writeAdjacencyMatrix(startNode->getLeft());
		}

		if(startNode->getRight() != nullptr){
			int rightNodeIdx = startNode->getRight()->getMatrixIndex();
			// std::cout << "adjMatrix[" << startNode->getMatrixLabel() << "][" << startNode->getRight()->getMatrixLabel() << "] = 1" << std::endl;
			// std::cout << "adjMatrix[" << startNode->getRight()->getMatrixLabel() << "][" << startNode->getMatrixLabel() << "] = 1" << std::endl;
			adjMatrix[curNodeIdx][rightNodeIdx] = "1";
			adjMatrix[rightNodeIdx][curNodeIdx] = "1";
			writeAdjacencyMatrix(startNode->getRight());
		}
	}
}

void Tree::wrireSumsAdjacencyMatrix(){
	int size = adjMatrix.size();
	for(int i=1; i<size-1; i++){ //ith row
		int sum = 0;
		for(int j=1; j<adjMatrix[0].size()-1; j++){ //jth column
			sum += stoi(adjMatrix[i][j]);
		}
		adjMatrix[i][size-1] = std::to_string(sum);
		adjMatrix[size-1][i] = std::to_string(sum);
	}
}

void Tree::printAdjacencyMatrix(){
	for(int i=0; i<adjMatrix.size(); i++){
		for(int j=0; j<adjMatrix[0].size(); j++){
			std::cout << adjMatrix[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

void Tree::writeAdjacencyMatrixToFile(std::string filename){
	std::ofstream adjMatFile;
	adjMatFile.open(filename);

	for(int i=0; i<adjMatrix.size(); i++){
		for(int j=0; j<adjMatrix[0].size(); j++){
			adjMatFile << adjMatrix[i][j] << ",";
		}
		adjMatFile << "\n";
	}
	adjMatFile.close();
	std::cout << "SUCCESS: Wrote Adjacency matrix to: \'" << filename << "\'" << std::endl;
}

// PLANAR POINT LOCATION

void Tree::findPoint(int x, int y, Node * startNode){
	if(startNode != nullptr){
		std::cout << startNode->getMatrixLabel(); 
		NodeType nt = startNode->getNodeType();
		if(nt == NodeType::x){
			std::cout << "->";
			int nodeX = startNode->getValue();
			if(x < nodeX){
				findPoint(x,y,startNode->getLeft());
			} else{
				findPoint(x,y,startNode->getRight());
			}
		} else if(nt == NodeType::y){
			std::cout << "->";
			int nodeY = startNode->getSegment()->getYonSeg(x);
			if(y < nodeY){
				findPoint(x,y,startNode->getLeft());
			} else{
				findPoint(x,y,startNode->getRight());
			}
		} else { // nt = NodeType::leaf
			std::cout << std::endl;
			std::cout << "Point (" << x << "," << y << ") is found in " << startNode->getMatrixLabel() << std::endl;
		}
	}
}
