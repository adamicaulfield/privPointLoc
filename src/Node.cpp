// By Adam Caulfield (ac7717)
// October 25 2021

#include "Node.h"

Node::Node(Tree * t, Segment * seg, NodeType ty){
	left = nullptr;
	right = nullptr;
	s = seg;
	tree = t;
	type = ty;
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


void Node::addPathLabel(std::string s){
	pathLabels.push_back(s);
}

std::vector<std::string> Node::getPathLabels(){
	return pathLabels;
}

void Node::evaluateNode(Encryptor &encryptor, PrivPointUtil * privUtil, helib::Ctxt pointCtxt, int maxBits, int nSlots){
    // Needed for comparisons
	result = new helib::Ctxt(*(encryptor.getPublicKey()));

	if(type == NodeType::x){
		printf("\t Evaluating X-Node id=%s, x=%d\n", matrixLabel.c_str(), value);
		// Convert vectex X Point to bin
		std::vector<long> xVertex = privUtil->encodePoint(maxBits, value, nSlots);
		helib::Ptxt<helib::BGV> xVertexPtxt (*(encryptor.getContext()));
		// printf("xVertex Bits: ");
		for(int i=0; i<2*maxBits; i++){
			xVertexPtxt[i] = xVertex[i];
			// printf("%ld ", xVertex[i]);
		}
		// printf("\n");

		// encryptor.decryptAndPrintCondensed("pointCtxt <= xVertex", tmpResult, maxBits);
		*(result) = privUtil->secureGT(encryptor, maxBits, nSlots, pointCtxt, xVertexPtxt);
		// encryptor.decryptAndPrintCondensed("gtCtxt", gtCtxt, maxBits);

	} else if(type == NodeType::y){
		printf("\t Evaluating Y-Node id=%s\n", matrixLabel.c_str());
		// Convert dy, dx, and dx*intercept to bin
		int dx = s->getDx();
		int dy = s->getDy();
		int dxb =  dx * s->getIntercept();

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

		// tmpResult.multiplyBy(privUtil->secureLT(encryptor, maxBits, nSlots, dxy, c1));
		// encryptor.decryptAndPrintCondensed("pointCtxt <= yVertex", tmpResult, maxBits);
		*(result) = privUtil->secureGT(encryptor, maxBits, nSlots, dxy, c1);
	}
}

helib::Ctxt Node::getNodeResult(){
	return *(result);
}