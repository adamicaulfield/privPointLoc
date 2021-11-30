// By Adam Caulfield (ac7717)
// October 25 2021

#include "Encryptor.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include "Tree.h"
#include "Node.h"
#include "Segment.h"
#include <stdlib.h>     /* atoi */
#include "PrivPointUtil.h"

int main(int argc, char **argv) {
    PrivPointUtil * privUtil = new PrivPointUtil();

    if (argc<2){
        printf("-------------------- START MAIN -------------------- \n");
        
        Tree * tree = new Tree();

        printf("----- PHASE 1: Read from file, insert into Tree/DAG -----\n");
        tree->readSegmentsFile("data/ac7717.txt");
        
        printf("----- Print Tree/DAG after read file and insert segments -----\n");
        tree->printTree(tree->getRoot());

        // Segment * s0 = new Segment(2,5,5,7);
        // Segment * s1 = new Segment(4,3,8,4);
        
        // tree->insert(s0);
        // tree->insert(s1);

        printf("----- PHASE 2: Write to Adjacency Matrix -----\n");
        tree->setupLists(tree->getRoot());
        tree->initAdjacencyMatrix();
        tree->writeAdjacencyMatrix(tree->getRoot());
        tree->wrireSumsAdjacencyMatrix();
        // tree->printAdjacencyMatrix();
        tree->writeAdjacencyMatrixToFile("data/adjMatrix.csv");

        // printf("---- PHASE 3: Enter a Point in the form \"x y\" to discover which trapezoid it is located in: \n");
        // std::string pointStr;
        // std::cout << "POINT: ";
        // std::getline(std::cin,pointStr);
        // std::cout << pointStr;
        
        // std::stringstream ss;

        // int x,y;

        // ss << pointStr;
        // ss >> x >> y;
        // printf("\nFINDING POINT: (%d,%d)\n", x,y);
        // tree->findPoint(x,y, tree->getRoot());
    } 
    else{
        unsigned long plaintext_prime_modulus = 2;
        unsigned long phiM = 21845;
        unsigned long lifting = 1;
        unsigned long numOfBitsOfModulusChain = 256;
        unsigned long numOfColOfKeySwitchingMatrix = 2;  
        
        printf("------ Initialize Encryptor Object ------\n");
        Encryptor encryptor("/tmp/sk.txt", "/tmp/pk.txt", plaintext_prime_modulus,phiM,lifting,numOfBitsOfModulusChain,numOfColOfKeySwitchingMatrix);
        printf("------ Done ------\n\n");
        int nSlots = encryptor.getEncryptedArray()->size();
        // std::cout << "Slot count: " << nSlots << std::endl;

        int totalPoints = 3;

        int maxBits = 4;

        std::vector<int> point = {5,11};
        std::vector<long> pointBits = privUtil->encodePoint(maxBits, point, nSlots);
        helib::Ctxt pointCtxt (*(encryptor.getPublicKey()));
        encryptor.getEncryptedArray()->encrypt(pointCtxt, *(encryptor.getPublicKey()), pointBits);

        std::vector<int> regionVertex = {10,10};
        std::vector<long> regionVertexBits = privUtil->encodePoint(maxBits, regionVertex, nSlots);

        printf("point(%d,%d) in bits: ",point[0],point[1]);
        for(int i=0; i<maxBits*2; i++){
            std::cout << pointBits[i] << " "; 
        }    
        std::cout << std::endl;

        helib::Ptxt<helib::BGV> regionVertextPtxt (*(encryptor.getContext())); 
        printf("Region Vertex(%d,%d) in bits: ", regionVertex[0], regionVertex[1]);
        for(int i=0; i<2*maxBits; i++){
            regionVertextPtxt[i] = regionVertexBits[i];
            std::cout << regionVertexBits[i] << " "; 
        }
        std::cout << "\n" << std::endl;

        privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, regionVertextPtxt, 0);

        printf("Compare by y coordinate:\n");
        privUtil->secureGT(encryptor, maxBits, nSlots, pointCtxt, regionVertextPtxt, 1);

        privUtil->secureLT(encryptor, maxBits, nSlots, pointCtxt, regionVertextPtxt, 1);
    }   
}