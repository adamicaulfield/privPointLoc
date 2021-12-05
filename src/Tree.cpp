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
#include "omp.h"
#include <chrono>

Tree::Tree(){
	size = 0;
	root = NULL;
	totalTrapezoids = 0;
}

/*********** Getters ***********/
Node * Tree::getRoot(){
	return root;
}

int Tree::getTreeSize(){
	return size;
}

int Tree::getTotalTrapezoids(){
	return totalTrapezoids;
}

std::vector<std::string> Tree::getAllPaths(){
	return allPaths;
}

/*********** Tree/DAG Setup functions ***********/
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

	if(size == 0){ // No segments added yet
		printf("\tAdding seg to tree, tree has size = 0\n");
		// printf("LEFT ENDPOINT: (%d,%d)\n", xl, yl);
		// printf("RIGHT ENDPOINT: (%d,%d)\n", xr, yr);
		// Add point nodes
		if(yl > yr){

			root = l;
			l->setRight(r);

			// Add segment node
			r->setLeft(seg);
		} else{
			root = r;
			r->setLeft(l);

			// Add segment node
			l->setRight(seg);
		}
		addLeafNodes(root);
		// printTree(root);

		minX = xl;
		maxX = xr;
	} 
	else if(xr < minX){ // Whole segment is to left of current map
		printf("\tAdding seg to tree: seg left of whole map\n");
		Node * searchNode = root;
		Node * prevNode;
		while(searchNode->getNodeType() != NodeType::leaf){
			prevNode = searchNode;
			searchNode = searchNode->getLeft();
		}
		if(yl > yr){
			l->setRight(r);
			r->setLeft(seg);
			prevNode->setLeft(l);
			delete(searchNode);
			addLeafNodes(l);
		} else {
			r->setLeft(l);
			l->setRight(seg);
			prevNode->setLeft(r);
			delete(searchNode);
			addLeafNodes(r);
		}
		minX = xr;
	}
	else if(xl > maxX){ // whole segment is to the right of current map
		printf("\tAdding seg to tree: seg right of whole map\n");
		Node * searchNode = root;
		Node * prevNode;
		while(searchNode->getNodeType() != NodeType::leaf){
			prevNode = searchNode;
			searchNode = searchNode->getRight();
		}
		if(yl > yr){
			l->setRight(r);
			r->setLeft(seg);
			prevNode->setRight(l);
			delete(searchNode);
			addLeafNodes(l);
		} else {
			r->setLeft(l);
			l->setRight(seg);
			prevNode->setRight(r);
			delete(searchNode);
			addLeafNodes(r);
		}
		maxX = xl;	
	}

	else{ // segment falls into the middle of the current map
		printf("\tAdding seg to tree, tree has size > 0\n");
		// Locate trapezoid leaf node which holds the left endpoint
		Node * searchNode = root;
		Node * prevNode;
		NodeType searchNodeType = searchNode->getNodeType();
		bool dir = true;
		int value;
		// printf("Starting LEFT while loop...\n");
		while(searchNodeType != NodeType::leaf){
			prevNode = searchNode;
			switch(searchNodeType){	
				case NodeType::x:
					value = searchNode->getValue();
					if(xl <= searchNode->getValue()){
						printf("\tX: %d <= %d-->\n", xl, value);
						searchNode = searchNode->getLeft();
						dir = true;
					} else{
						printf("\tX: %d > %d-->\n", xl, value);
						searchNode = searchNode->getRight();
						dir = false;
					}
					break;
				
				case NodeType::y:
					value = searchNode->getSegment()->getYonSeg(xl);
					if(yl <= searchNode->getSegment()->getYonSeg(xl)){
						printf("\tY: %d <= %d-->\n", yl, value);
						searchNode = searchNode->getLeft();
						dir = true;
					} else{
						printf("\tY: %d > %d-->\n", yl, value);
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
		// printf("Replacing leaf node with new LEFT x-node\n");
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
					value = searchNode->getValue();
					if(xr <= searchNode->getValue()){
						// printf("\tX: %d <= %d-->\n", xr, value);
						searchNode = searchNode->getLeft();
						dir = true;
					} else{
						// printf("\tX: %d > %d-->\n", xr, value);
						searchNode = searchNode->getRight();
						dir = false;
					}
					break;
				
				case NodeType::y:
					value = searchNode->getSegment()->getYonSeg(xr);
					if(yr <= searchNode->getSegment()->getYonSeg(xr)){
						// printf("\tY: %d <= %d-->\n", yr, value);
						searchNode = searchNode->getLeft();
						dir = true;
					} else{
						// printf("\tY: %d > %d-->\n", yr, value);
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
	// printf("Done insert()\n");
	// printTree(root);
}

void Tree::addLeafNodes(Node * node){
	if(node != nullptr){
		// printf("At node:%d\n", node->getValue());
		if(node->getLeft() == nullptr){
			// printf("Adding leaf at left...\n");
			Node * leaf = new Node(this, nullptr, NodeType::leaf);
			leaf->setValue(totalTrapezoids);
			totalTrapezoids++;
			node->setLeft(leaf);
		} else{
			if(node->getLeft()->getNodeType() != NodeType::leaf){
				// printf("moving left...\n");
				addLeafNodes(node->getLeft());	
			}
		}

		if(node->getRight() == nullptr){
			// printf("Adding leaf at right...\n");
			Node * leaf = new Node(this, nullptr, NodeType::leaf);
			leaf->setValue(totalTrapezoids);
			totalTrapezoids++;
			node->setRight(leaf);
		} else{
			if(node->getRight()->getNodeType() != NodeType::leaf){
				// printf("moving right...\n");
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

/*********** UTILITY FUNCTIONS ***********/

// Process Input file, feed segments into tree
void Tree::readSegmentsFile(std::string filename){
	printf("starting readSegmentsFile()...\n");
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
				// Ensures smaller x is treated as left endpoint
				if(xl < xr){
					printf("ADDING SEGMENT: (%d, %d)-(%d, %d)\n", xl, yl, xr, yr);
					insert(new Segment(xl, yl, xr, yr));
				} else {
					printf("ADDING SEGMENT: (%d, %d)-(%d, %d)\n", xr, yr, xl, yl);
					insert(new Segment(xr, yr, xl, yl));
				}
				
			}
			lineNum++;
		}
		segFile.close();
	}
	else{
		std::cout << "Could not open file" << std::endl;
	}
	printf("done readSegmentsFile()...\n");
}

// Prints trees
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

// Resets tree labels to be continuous
void Tree::cleanTree(Node * startNode){
	setupLists(startNode);
	totalTrapezoids = tList.size();
	for(int i=0; i<totalTrapezoids; i++){
		tList[i]->setValue(i);
	}
}

// Prints path labels for each leaf node, assumes call to setupLists() already occurred.
void Tree::printPathLabels(){
	printf("Path Labels for each Trapezoid (%d):\n", totalTrapezoids);
	for(int i=0; i<tList.size(); i++){
		std::cout << tList[i]->getMatrixLabel();
		std::vector<std::string> pathLabels = tList[i]->getPathLabels();
		std::cout << " (" << int(pathLabels.size()) << "): ";
		for(int j=0; j<pathLabels.size(); j++){
			std::cout << pathLabels[j] << " ";
		}
		std::cout << std::endl;
	}
}

// Setup path labels as a string represeting right(1) and left(1) turns starting from root. EX: string "001" means left-left-right
void Tree::setupPathLabels(Node * startNode, std::string label){
	if(startNode != nullptr){
		NodeType nt = startNode->getNodeType();
		if(nt == NodeType::leaf){
			startNode->addPathLabel(label);
			allPaths.push_back(label);
		} else{
			// Traverse Left
			setupPathLabels(startNode->getLeft(), label+"0");

			// Traverse Right
			setupPathLabels(startNode->getRight(), label+"1");
		}
	} else {
		return;
	}
}

void Tree::printLists(){
	printf("P-List: ");
	for(int i=0; i<pList.size(); i++){
		printf("%d ", pList[i]->getValue());
	}
	printf("\n");

	printf("Q-List: ");
	for(int i=0; i<qList.size(); i++){
		printf("%d ", qList[i]->getValue());
	}
	printf("\n");

	printf("S-List: ");
	for(int i=0; i<sList.size(); i++){
		printf("%d ", sList[i]->getValue());
	}
	printf("\n");

	printf("T-List: ");
	for(int i=0; i<tList.size(); i++){
		printf("%d ", tList[i]->getValue());
	}
	printf("\n");
}

/*********** ADJACENCY MATRIX FUNCTIONS ***********/
void Tree::setupLists(Node * startNode){
	if(startNode != nullptr){
		NodeType nt = startNode->getNodeType();
		bool notInList = true;
		switch(nt){
			case NodeType::x:
				if(startNode->getIsRight()){
					for(int t=0; t<pList.size(); t++){
						notInList = notInList && (startNode != pList[t]);
					}
					if(notInList){
						pList.push_back(startNode);	
					}
				} else {
					for(int t=0; t<qList.size(); t++){
						notInList = notInList && (startNode != qList[t]);
					}
					if(notInList){
						qList.push_back(startNode);	
					}
				}
				break;
			case NodeType::y:
				for(int t=0; t<sList.size(); t++){
					notInList = notInList && (startNode != sList[t]);
				}
				if(notInList){
					sList.push_back(startNode);	
				}
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
			std::cout << adjMatrix[i][j] << "    ";
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

/*********** POINT LOCATION ***********/

// Plaintext
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

// Encrypted -- Serial
void Tree::findPrivatePoint(Encryptor &encryptor, PrivPointUtil * privUtil, helib::Ctxt pointCtxt, helib::Ctxt &resultCtxt, helib::Ctxt tmpResult, Node * startNode, int maxBits, int nSlots){
	
	if(startNode != nullptr){

	    // Needed for comparisons
	    helib::Ptxt<helib::BGV> ones(*(encryptor.getContext()));
        for(int i=0; i<maxBits; i++){
            ones[i] = 1;
        }

		NodeType nt = startNode->getNodeType();
		if(nt == NodeType::x){
			printf("\t Checking X-Node\n");
			// Convert vectex X Point to bin
			std::vector<long> xVertex = privUtil->encodePoint(maxBits, startNode->getValue(), nSlots);
			helib::Ptxt<helib::BGV> xVertexPtxt (*(encryptor.getContext()));
			// printf("xVertex Bits: ");
			for(int i=0; i<2*maxBits; i++){
				xVertexPtxt[i] = xVertex[i];
				// printf("%ld ", xVertex[i]);
			}
			// printf("\n");

			// Do secure Comparison --> tmpGT = secureGT(..), tmpLT = secureLT(..)
			helib::Ctxt tmpResult2(tmpResult);

			// helib::Ctxt ltCtxt = privUtil->secureLT(encryptor, maxBits, nSlots, pointCtxt, xVertexPtxt);
			// encryptor.decryptAndPrintCondensed("ltCtxt", ltCtxt, maxBits);
			// tmpResult.multiplyBy(ltCtxt);

			// encryptor.decryptAndPrintCondensed("pointCtxt <= xVertex", tmpResult, maxBits);
			helib::Ctxt gtCtxt = privUtil->secureGT(encryptor, maxBits, nSlots, pointCtxt, xVertexPtxt);
			// encryptor.decryptAndPrintCondensed("gtCtxt", gtCtxt, maxBits);
			tmpResult2.multiplyBy(gtCtxt);
			// encryptor.decryptAndPrintCondensed("pointCtxt > xVertex", tmpResult2, maxBits);

			helib::Ctxt ltCtxt = gtCtxt;
			ltCtxt.addConstant(ones);
            tmpResult.multiplyBy(ltCtxt);

			// Move to left
			// printf("\t Checking Left\n");
			findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult, startNode->getLeft(), maxBits, nSlots);

			// Move to right
			// printf("\t Checking Right\n");
			findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult2, startNode->getRight(), maxBits, nSlots);

		} else if(nt == NodeType::y){
			printf("\t Checking Y-Node\n");
			// Convert dy, dx, and dx*intercept to bin
			int dx = startNode->getSegment()->getDx();
			int dy = startNode->getSegment()->getDy();
			int dxb =  dx * startNode->getSegment()->getIntercept();

			std::vector<long> dxBits = privUtil->encodePoint(maxBits, dx, nSlots);
			std::vector<long> dyBits = privUtil->encodePoint(maxBits, dy, nSlots);
			std::vector<long> dxbBits = privUtil->encodePoint(maxBits, dxb, nSlots);

			helib::Ptxt<helib::BGV> dxPtxt (*(encryptor.getContext())); 
			helib::Ptxt<helib::BGV> dyPtxt (*(encryptor.getContext())); 
			helib::Ptxt<helib::BGV> dxbPtxt (*(encryptor.getContext())); 
			for(int i=0; i<maxBits; i++){
			    dxPtxt[i] = dxBits[i];
			    dyPtxt[i] = dyBits[i];
			    dxbPtxt[i] = dxbBits[i];
			}			

			// printf("dxPtxt (%d): ", dx);
			// for(int i=0; i<maxBits; i++){
			//     printf("%ld ", dxBits[i]);
			// }
			// printf("\n");	

			// printf("dyPtxt: (%d)", dy);
			// for(int i=0; i<maxBits; i++){
			//     printf("%ld ", dyBits[i]);
			// }
			// printf("\n");	

			// printf("dxbPtxt: (%d)", dxb);
			// for(int i=0; i<maxBits; i++){
			//     printf("%ld ", dxbBits[i]);
			// }
			// printf("\n");	


			// Compute dy*[x]+dxb
			helib::Ctxt dyx = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, dyPtxt, 0);
			// printf("\t");
			// encryptor.decryptAndPrintCondensed("dyx", dyx, maxBits);

			// Compute dx*[y]
			helib::Ctxt dxy = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, dxPtxt, 1);
			// printf("\t");
			// encryptor.decryptAndPrintCondensed("dxy", dxy, maxBits);

			// Move to left if dx[y] <= (c1=dy[x]+dxb)
			helib::Ctxt c1 = privUtil->binaryAdd(encryptor, maxBits, nSlots, dyx, dxbPtxt);
			// printf("\t");
			// encryptor.decryptAndPrintCondensed("c1", c1, maxBits);

			// Do secure Comparison --> tmpGT = secureGT(..), tmpLT = secureLT(..)
			helib::Ctxt tmpResult2(tmpResult);

			// tmpResult.multiplyBy(privUtil->secureLT(encryptor, maxBits, nSlots, dxy, c1));
			// encryptor.decryptAndPrintCondensed("pointCtxt <= yVertex", tmpResult, maxBits);
			helib::Ctxt gtCtxt = privUtil->secureGT(encryptor, maxBits, nSlots, dxy, c1);
			tmpResult2.multiplyBy(gtCtxt);
			// encryptor.decryptAndPrintCondensed("pointCtxt > yVertex", tmpResult, maxBits);
            helib::Ctxt ltCtxt = gtCtxt;
            ltCtxt.addConstant(ones);
            tmpResult.multiplyBy(ltCtxt);

			// printf("\t Checking Left\n");
			// Move to left
			findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult, startNode->getLeft(), maxBits, nSlots);

			// printf("\t Checking Right\n");
			// Move to right
			findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult2, startNode->getRight(), maxBits, nSlots);

		} else{ // Leaf Node
			// Mask result based on T_ID
			int tID = startNode->getValue();
			// printf("\tGot to TID=%d\n", tID);
			// encryptor.decryptAndPrintCondensed("tmpResult", tmpResult, maxBits);
			helib::Ptxt<helib::BGV> mask (*(encryptor.getContext())); 
			mask[tID] = 1;
			tmpResult.multByConstant(mask);
			
			// resultCtxt = resultCtxt OR tmpResult = xor(r,t) + and(r,t)
			helib::Ctxt andCtxt(tmpResult); //and
			andCtxt.multiplyBy(resultCtxt);

			resultCtxt += tmpResult;	//xor
			resultCtxt += andCtxt;		//xor + and
			// printf("\t Accumulated result\n");

		}
	}
}

// Encrypted -- Parallel v1
void Tree::findPrivatePoint2(Encryptor &encryptor, PrivPointUtil * privUtil, helib::Ctxt pointCtxt, helib::Ctxt &resultCtxt, helib::Ctxt tmpResult, Node * startNode, int maxBits, int nSlots){
	if(startNode != nullptr){
		NodeType nt = startNode->getNodeType();
		if(nt == NodeType::x){
			printf("\t Checking X-Node\n");
			// Convert vectex X Point to bin
			std::vector<long> xVertex = privUtil->encodePoint(maxBits, startNode->getValue(), nSlots);
			helib::Ptxt<helib::BGV> xVertexPtxt (*(encryptor.getContext()));
			// printf("xVertex Bits: ");
			for(int i=0; i<2*maxBits; i++){
				xVertexPtxt[i] = xVertex[i];
				// printf("%ld ", xVertex[i]);
			}
			// printf("\n");

			// Do secure Comparison --> tmpGT = secureGT(..), tmpLT = secureLT(..)
			helib::Ctxt tmpResult2(tmpResult);

			#pragma omp parallel
			{
				if(omp_get_thread_num()==0){
					helib::Ctxt ltCtxt = privUtil->secureLT(encryptor, maxBits, nSlots, pointCtxt, xVertexPtxt);
					// encryptor.decryptAndPrintCondensed("ltCtxt", ltCtxt, maxBits);
					tmpResult.multiplyBy(ltCtxt);
					// encryptor.decryptAndPrintCondensed("pointCtxt <= xVertex", tmpResult, maxBits);
				}
				if(omp_get_thread_num()==1){
					helib::Ctxt gtCtxt = privUtil->secureGT(encryptor, maxBits, nSlots, pointCtxt, xVertexPtxt);
					// encryptor.decryptAndPrintCondensed("gtCtxt", gtCtxt, maxBits);
					tmpResult2.multiplyBy(gtCtxt);
					// encryptor.decryptAndPrintCondensed("pointCtxt > xVertex", tmpResult2, maxBits);
				}
			}

			// Move to left
			// printf("\t Checking Left\n");
			findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult, startNode->getLeft(), maxBits, nSlots);

			// Move to right
			// printf("\t Checking Right\n");
			findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult2, startNode->getRight(), maxBits, nSlots);

		} else if(nt == NodeType::y){
			printf("\t Checking Y-Node\n");
			// Convert dy, dx, and dx*intercept to bin
			int dx = startNode->getSegment()->getDx();
			int dy = startNode->getSegment()->getDy();
			int dxb =  dx * startNode->getSegment()->getIntercept();

			std::vector<long> dxBits = privUtil->encodePoint(maxBits, dx, nSlots);
			std::vector<long> dyBits = privUtil->encodePoint(maxBits, dy, nSlots);
			std::vector<long> dxbBits = privUtil->encodePoint(maxBits, dxb, nSlots);

			helib::Ptxt<helib::BGV> dxPtxt (*(encryptor.getContext())); 
			helib::Ptxt<helib::BGV> dyPtxt (*(encryptor.getContext())); 
			helib::Ptxt<helib::BGV> dxbPtxt (*(encryptor.getContext())); 
			for(int i=0; i<maxBits; i++){
			    dxPtxt[i] = dxBits[i];
			    dyPtxt[i] = dyBits[i];
			    dxbPtxt[i] = dxbBits[i];
			}			

			// printf("dxPtxt (%d): ", dx);
			// for(int i=0; i<maxBits; i++){
			//     printf("%ld ", dxBits[i]);
			// }
			// printf("\n");	

			// printf("dyPtxt: (%d)", dy);
			// for(int i=0; i<maxBits; i++){
			//     printf("%ld ", dyBits[i]);
			// }
			// printf("\n");	

			// printf("dxbPtxt: (%d)", dxb);
			// for(int i=0; i<maxBits; i++){
			//     printf("%ld ", dxbBits[i]);
			// }
			// printf("\n");	

			helib::Ctxt dyx(*(encryptor.getPublicKey()));
			helib::Ctxt dxy(*(encryptor.getPublicKey()));

			#pragma omp parallel
			{
				if(omp_get_thread_num()==0){
					dyx = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, dyPtxt, 0);
				}

				if(omp_get_thread_num()==1){
					dxy = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, dxPtxt, 1);
				}
			}

			// Compute dy*[x]+dxb
			// helib::Ctxt dyx = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, dyPtxt, 0);
			// printf("\t");
			// encryptor.decryptAndPrintCondensed("dyx", dyx, maxBits);

			// Compute dx*[y]
			// helib::Ctxt dxy = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, dxPtxt, 1);
			// printf("\t");
			// encryptor.decryptAndPrintCondensed("dxy", dxy, maxBits);

			// Move to left if dx[y] <= (c1=dy[x]+dxb)
			helib::Ctxt c1 = privUtil->binaryAdd(encryptor, maxBits, nSlots, dyx, dxbPtxt);
			// printf("\t");
			// encryptor.decryptAndPrintCondensed("c1", c1, maxBits);

			// Do secure Comparison --> tmpGT = secureGT(..), tmpLT = secureLT(..)
			helib::Ctxt tmpResult2(tmpResult);

			#pragma omp parallel
			{
				if(omp_get_thread_num()==0){
					tmpResult.multiplyBy(privUtil->secureLT(encryptor, maxBits, nSlots, dxy, c1));
				}

				if(omp_get_thread_num()==1){
					tmpResult2.multiplyBy(privUtil->secureGT(encryptor, maxBits, nSlots, dxy, c1));
				}
			}

			// tmpResult.multiplyBy(privUtil->secureLT(encryptor, maxBits, nSlots, dxy, c1));
			// encryptor.decryptAndPrintCondensed("pointCtxt <= yVertex", tmpResult, maxBits);
			// tmpResult2.multiplyBy(privUtil->secureGT(encryptor, maxBits, nSlots, dxy, c1));
			// encryptor.decryptAndPrintCondensed("pointCtxt > yVertex", tmpResult, maxBits);

			// printf("\t Checking Left\n");
			// Move to left
			findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult, startNode->getLeft(), maxBits, nSlots);

			// printf("\t Checking Right\n");
			// Move to right
			findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult2, startNode->getRight(), maxBits, nSlots);

		} else{ // Leaf Node
			// Mask result based on T_ID
			int tID = startNode->getValue();
			// printf("\tGot to TID=%d\n", tID);
			// encryptor.decryptAndPrintCondensed("tmpResult", tmpResult, maxBits);
			helib::Ptxt<helib::BGV> mask (*(encryptor.getContext())); 
			mask[tID] = 1;
			tmpResult.multByConstant(mask);
			
			// resultCtxt = resultCtxt OR tmpResult = xor(r,t) + and(r,t)
			helib::Ctxt andCtxt(tmpResult); //and
			andCtxt.multiplyBy(resultCtxt);

			resultCtxt += tmpResult;	//xor
			resultCtxt += andCtxt;		//xor + and
			// printf("\t Accumulated result\n");

		}
	} 
}

// Evaluate one path (for debugging)
void Tree::evaluatePath(Encryptor &encryptor, PrivPointUtil * privUtil, helib::Ctxt pointCtxt, helib::Ctxt &resultCtxt, int maxBits, int nSlots, std::string pathLabel){
	Node * startNode = root;
	helib::Ctxt zeros(*(encryptor.getPublicKey()));
	printf("\t\t");
	encryptor.decryptAndPrintCondensed("resultCtxt", resultCtxt, totalTrapezoids+1);
	for(int s=0; s<pathLabel.size(); s++){
		if(startNode != nullptr){
			NodeType nt = startNode->getNodeType();
			if(nt == NodeType::x){
				printf("\t Checking X-Node\n");
				// Convert vectex X Point to bin
				std::vector<long> xVertex = privUtil->encodePoint(maxBits, startNode->getValue(), nSlots);
				helib::Ptxt<helib::BGV> xVertexPtxt (*(encryptor.getContext()));
				// printf("xVertex Bits: ");
				for(int i=0; i<2*maxBits; i++){
					xVertexPtxt[i] = xVertex[i];
					// printf("%ld ", xVertex[i]);
				}
				
				helib::Ctxt comp = privUtil->secureGT(encryptor, maxBits, nSlots, pointCtxt, xVertexPtxt);
				// printf("\t Checking Left\n");
				if(pathLabel.at(s) == '0'){
					// Move to left
					// Needed for comparisons
					helib::Ptxt<helib::BGV> ones(*(encryptor.getContext()));
					for(int i=0; i<nSlots; i++){
					    ones[i] = 1;
					}
					// helib::Ctxt ltCtxt = privUtil->secureLT(encryptor, maxBits, nSlots, pointCtxt, xVertexPtxt);
					comp.addConstant(ones);
					// encryptor.decryptAndPrintCondensed("pointCtxt <= xVertex", tmpResult, maxBits);
					// encryptor.decryptAndPrintCondensed("ltCtxt", ltCtxt, maxBits);
					resultCtxt.multiplyBy(comp);
					// encryptor.decryptAndPrintCondensed("pointCtxt <= xVertex", resultCtxt, maxBits);
					startNode = startNode->getLeft();
				} else if(pathLabel.at(s) == '1'){
					// encryptor.decryptAndPrintCondensed("pointCtxt <= xVertex", tmpResult, maxBits);
					
					// encryptor.decryptAndPrintCondensed("gtCtxt", gtCtxt, maxBits);
					resultCtxt.multiplyBy(comp);
					// encryptor.decryptAndPrintCondensed("pointCtxt > xVertex", resultCtxt, maxBits);
					startNode = startNode->getRight();
				}

			} else if(nt == NodeType::y){
				printf("\t Checking Y-Node\n");
				// Convert dy, dx, and dx*intercept to bin
				int dx = startNode->getSegment()->getDx();
				int dy = startNode->getSegment()->getDy();
				int dxb =  dx * startNode->getSegment()->getIntercept();

				std::vector<long> dxBits = privUtil->encodePoint(maxBits, dx, nSlots);
				std::vector<long> dyBits = privUtil->encodePoint(maxBits, dy, nSlots);
				std::vector<long> dxbBits = privUtil->encodePoint(maxBits, dxb, nSlots);

				helib::Ptxt<helib::BGV> dxPtxt (*(encryptor.getContext())); 
				helib::Ptxt<helib::BGV> dyPtxt (*(encryptor.getContext())); 
				helib::Ptxt<helib::BGV> dxbPtxt (*(encryptor.getContext())); 
				
				printf("\t\tdxb: ");
				for(int i=0; i<maxBits; i++){
				    dxPtxt[i] = dxBits[i];
				    dyPtxt[i] = dyBits[i];
				    dxbPtxt[i] = dxbBits[i];
				    printf("%ld ", dxbBits[i]);
				}			
				printf("\n");

				// Compute dy*[x]+dxb
				helib::Ctxt dyx = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, dyPtxt, 0);
				printf("\t\t");
				encryptor.decryptAndPrintCondensed("dyx", dyx, maxBits);

				// Compute dx*[y]
				helib::Ctxt dxy = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, dxPtxt, 1);
				printf("\t\t");
				encryptor.decryptAndPrintCondensed("dxy", dxy, maxBits);

				// Move to left if dx[y] <= (c1=dy[x]+dxb)
				helib::Ctxt c1 = privUtil->binaryAdd(encryptor, maxBits, nSlots, dyx, dxbPtxt);
				printf("\t\t");
				encryptor.decryptAndPrintCondensed("c1", c1, maxBits);

				helib::Ctxt comp = privUtil->secureGT(encryptor, maxBits, nSlots, dxy, c1);
				if(pathLabel.at(s) == '0'){
					// printf("\t Checking Left\n");
					helib::Ptxt<helib::BGV> ones(*(encryptor.getContext()));
					for(int i=0; i<nSlots; i++){
					    ones[i] = 1;
					}
					comp.addConstant(ones);
					printf("\t\t");
					encryptor.decryptAndPrintCondensed("comp", comp, maxBits);
					resultCtxt.multiplyBy(comp);
					startNode = startNode->getLeft();
					// Move to left
				} else if(pathLabel.at(s) == '1'){
					// printf("\t Checking Right\n");
					// Move to right
					resultCtxt.multiplyBy(comp);	
					startNode = startNode->getRight();
				}
			}
		}
		printf("\t\t");
		encryptor.decryptAndPrintCondensed("resultCtxt", resultCtxt, totalTrapezoids+1);
	}
	// Leaf Node
	// Mask result based on T_ID
	int tID = startNode->getValue();
	// printf("\tGot to Leaf %d\n", tID);
	std::string msg = "Got to Leaf " + std::to_string(tID);
	printf("\t");
	encryptor.decryptAndPrintCondensed(msg, resultCtxt, maxBits*2);
	helib::Ptxt<helib::BGV> mask (*(encryptor.getContext())); 
	mask[tID] = 1;
	printf("\tmask:");
	for(int i=0; i<maxBits*2; i++){
		std::cout << mask[i] << " ";
	}
	printf("\n");
	resultCtxt.multByConstant(mask);
	printf("\t");
	encryptor.decryptAndPrintCondensed("After masking", resultCtxt, totalTrapezoids+1);
	// resultCtxt = resultCtxt OR tmpResult = xor(r,t) + and(r,t) 
	// Not done for one path, move to other function.

	// helib::Ctxt andCtxt(tmpResult); //and
	// andCtxt.multiplyBy(resultCtxt);

	// resultCtxt += tmpResult;	//xor
	// resultCtxt += andCtxt;		//xor + and
	// printf("\t Accumulated result\n");
}

void Tree::evaluatePath2(Encryptor &encryptor, PrivPointUtil * privUtil, helib::Ctxt pointCtxt, helib::Ctxt &resultCtxt, int maxBits, int nSlots, std::string pathLabel){
	Node * startNode = root;

	helib::Ctxt zeros(*(encryptor.getPublicKey()));

    // Needed for comparisons
    helib::Ptxt<helib::BGV> ones(*(encryptor.getContext()));
    for(int i=0; i<maxBits; i++){
        ones[i] = 1;
    }

	for(int s=0; s<pathLabel.size(); s++){
		if(startNode != nullptr){
			NodeType nt = startNode->getNodeType();
			if(nt != NodeType::leaf){
				// Same processes for Y or X node. 
				helib::Ctxt comp = startNode->getNodeResult(); // Get pre-processed result -- stores greater than (>) result
				// std::string msg = startNode->getMatrixLabel()+" (val=" + std::to_string(startNode->getValue()) + ") result";
				if(pathLabel.at(s) == '0'){
					// Path this thread is testing specifies move to left
					comp.addConstant(ones); // Left refers to less than equal to (<=), so need (<=) result. LTE =  not GT --> LTE = GT + 1
					// encryptor.decryptAndPrintCondensed(msg, resultCtxt, maxBits*2);
					resultCtxt.multiplyBy(comp);
					startNode = startNode->getLeft();
				} else if(pathLabel.at(s) == '1'){
					// Path this thread is testing specifies move to right
					// encryptor.decryptAndPrintCondensed(msg, resultCtxt, maxBits*2);
					resultCtxt.multiplyBy(comp);
					startNode = startNode->getRight();
				}

			}
		}
	}

	// Reached leaf node, filer the ctxt so it is =1 only at the tID-th slot.
	int tID = startNode->getValue(); 
	helib::Ptxt<helib::BGV> mask(*(encryptor.getContext()));
	mask[tID] = 1;
	resultCtxt.multByConstant(mask);
	// Return with result of path in resultCtxt
}
// Do HE evaluation of each node based on pointCtxt
// Assumes the lists for adjacency matrix have already been made
helib::Ctxt Tree::evaluateAllNodes(Encryptor &encryptor, PrivPointUtil * privUtil, helib::Ctxt pointCtxt, int maxBits, int nSlots){
	// Concatenate all p, q, and s nodes
	std::vector<Node *> pqNodes = pList; // Insert p Nodes
	pqNodes.insert(pqNodes.end(), qList.begin(), qList.end()); // Insert q Nodes
	pqNodes.insert(pqNodes.end(), sList.begin(), sList.end()); // insert s Nodes

	// for initializing pathResultCtxt
	helib::Ptxt<helib::BGV> ones(*(encryptor.getContext()));
	for(int i=0; i<nSlots; i++){
	    ones[i] = 1;
	}
	helib::Ctxt finalResult(*(encryptor.getPublicKey()));

	printf("\tEvaluating all nodes.\n");
	auto start = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for
	for (int i=0; i<pqNodes.size(); i++){
		// std::cout <<"\t" << pqNodes[i]->getMatrixLabel() << std::endl;
		pqNodes[i]->evaluateNode(encryptor, privUtil, pointCtxt, maxBits, nSlots);
	}
	auto stop = std::chrono::high_resolution_clock::now();            
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
	printf("\tTime to evaluate all nodes: %0.2fs\n", duration.count()/1000.0);

	printf("\tAccumulating Results (total paths=%d)\n", (int)allPaths.size());
	start = std::chrono::high_resolution_clock::now();
	#pragma omp parallel for
	for (int i=0; i<allPaths.size(); i++){
	// int i=1;
		helib::Ctxt pathResultCtxt(*(encryptor.getPublicKey()));

		pathResultCtxt.addConstant(ones); // initialized to one because will be accumulating AND in evaluetePath2. Initialized to zero will force zero regardless of actual result.
		evaluatePath2(encryptor, privUtil, pointCtxt, pathResultCtxt, maxBits, nSlots, allPaths[i]);

		// finalResult <-- finalResult OR pathResult
		#pragma omp critical
		{
			// printf("Path %d entered critical region.\n", i);
			helib::Ctxt andCtxt(pathResultCtxt); //and
			andCtxt.multiplyBy(finalResult);
			finalResult += pathResultCtxt; //xor
			finalResult += andCtxt;       //xor + and
			// encryptor.decryptAndPrintCondensed("finalResult", finalResult, maxBits*2);	
		}
	}
	stop = std::chrono::high_resolution_clock::now();            
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
	printf("\tTiime to accumulate reulsts: %0.2fs\n", duration.count()/1000.0);
	return finalResult;
}