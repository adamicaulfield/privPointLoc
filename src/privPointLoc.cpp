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
#include <chrono>

int main(int argc, char **argv) {
    PrivPointUtil * privUtil = new PrivPointUtil();
    int mode = atoi(argv[1]);
    if (mode==0){
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
        // tree->writeAdjacencyMatrixToFile("data/adjMatrix.csv");

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
    else if(mode == 1){
        unsigned long plaintext_prime_modulus = 2;
        unsigned long phiM = 21845;
        unsigned long lifting = 1;
        unsigned long numOfBitsOfModulusChain = 256;
        unsigned long numOfColOfKeySwitchingMatrix = 2;  
        
        printf("------ Initialize Encryptor Object ------\n");
        Encryptor encryptor("/tmp/sk.txt", "/tmp/pk.txt", plaintext_prime_modulus, phiM, lifting, numOfBitsOfModulusChain, numOfColOfKeySwitchingMatrix);
        printf("------ Done ------\n\n");
        int nSlots = encryptor.getEncryptedArray()->size();
        std::cout << "Slot count: " << nSlots << std::endl;

        int maxBits = 4;

        int point = atoi(argv[2]);
        int regionVertex = atoi(argv[3]);

        std::vector<long> pointBits = privUtil->encodePoint(maxBits, point, nSlots);
        helib::Ctxt pointCtxt (*(encryptor.getPublicKey()));
        encryptor.getEncryptedArray()->encrypt(pointCtxt, *(encryptor.getPublicKey()), pointBits);

        std::vector<long> regionVertexBits = privUtil->encodePoint(maxBits, regionVertex, nSlots);

        printf("x=%d in bits: ", point);
        for(int i=0; i<maxBits*2; i++){
            std::cout << pointBits[i] << " "; 
        }    
        std::cout << std::endl;

        helib::Ptxt<helib::BGV> regionVertextPtxt (*(encryptor.getContext())); 
        printf("Vertex=%d in bits: ", regionVertex);
        for(int i=0; i<2*maxBits; i++){
            regionVertextPtxt[i] = regionVertexBits[i];
            std::cout << regionVertexBits[i] << " "; 
        }
        std::cout << "\n" << std::endl;

        helib::Ctxt result = privUtil->binaryMult(encryptor, maxBits, nSlots, pointCtxt, regionVertextPtxt, 0);
        encryptor.decryptAndPrintCondensed("FINAL: prod", result, 2*maxBits);

        printf("COMPARE A=%d and B=%d\n", point, regionVertex);
        result = privUtil->secureGT(encryptor, maxBits, nSlots, pointCtxt, regionVertextPtxt);
        encryptor.decryptAndPrintCondensed("A>B", result, 2*maxBits);

        result = privUtil->secureLT(encryptor, maxBits, nSlots, pointCtxt, regionVertextPtxt);
        encryptor.decryptAndPrintCondensed("A<=B", result, 2*maxBits);
    } else if(mode==2){
        printf("-------------------- START MAIN -------------------- \n");
        
        Tree * tree = new Tree();

        printf("----- PHASE 1: Read from file, insert into Tree/DAG -----\n");
        tree->readSegmentsFile("data/ac7717.txt");
        
        printf("----- Print Tree/DAG after read file and insert segments -----\n");
        tree->printTree(tree->getRoot());

        printf("------ Initialize Encryptor Object ------\n");
        unsigned long plaintext_prime_modulus = 2;
        unsigned long phiM = 21845;
        unsigned long lifting = 1;
        unsigned long numOfBitsOfModulusChain = 1024;
        unsigned long numOfColOfKeySwitchingMatrix = 2;  
    
        Encryptor encryptor("/tmp/sk.txt", "/tmp/pk.txt", plaintext_prime_modulus, phiM, lifting, numOfBitsOfModulusChain, numOfColOfKeySwitchingMatrix);
        
        int nSlots = encryptor.getEncryptedArray()->size();
        std::cout << "Slot count: " << nSlots << std::endl;
        printf("------ Done ------\n\n");

        int maxBits = 4;
        int xCoord = atoi(argv[2]);
        int yCoord = atoi(argv[3]);

        std::vector<long> xPointBits = privUtil->encodePoint(maxBits, xCoord, nSlots);
        std::vector<long> yPointBits = privUtil->encodePoint(maxBits, yCoord, nSlots);

        std::vector<long> pointBits(nSlots);
        std::vector<long> ones(nSlots);
        for(int i=0; i<maxBits; i++){
            
            pointBits[i] = xPointBits[i];
            pointBits[i+maxBits] = yPointBits[i];    
            
            ones[i] = 1;
        }

        printf("LOCATE PRIVATE POINT\n");
        // for(int i=0; i<maxBits*2; i++){
        //     std::cout << pointBits[i] << " "; 
        // }    
        // std::cout << std::endl;
        printf("---- User side ---- \n");

        helib::Ctxt pointCtxt (*(encryptor.getPublicKey()));
        helib::Ctxt tmpResult(*(encryptor.getPublicKey()));
        helib::Ctxt resultCtxt(*(encryptor.getPublicKey()));

        printf("\tEncrypting point.... ");
        encryptor.getEncryptedArray()->encrypt(pointCtxt, *(encryptor.getPublicKey()), pointBits);
        encryptor.getEncryptedArray()->encrypt(tmpResult, *(encryptor.getPublicKey()), ones);
        printf(" Done\n");
        // encryptor.decryptAndPrintCondensed("init tmpResult", tmpResult, maxBits);
        // encryptor.getEncryptedArray()->encrypt(resultCtxt, *(encryptor.getPublicKey()), ones);
        printf("\tSending to Cloud for Planar Point Location\n");

        printf("---- Cloud Side ---- \n");
        printf("\tBeginning location....\n");
        auto start = std::chrono::high_resolution_clock::now();
        tree->findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult, tree->getRoot(), maxBits, nSlots);
        auto stop = std::chrono::high_resolution_clock::now();            
        printf("\tComplete. Sending back to user ....\n");

        printf("---- User Side ---- \n");
        printf("\tReceiving ciphertext form cloud. Decrypting: \n");
        printf("\t");
        encryptor.decryptAndPrintCondensed("Result", resultCtxt, 4);
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
        printf("\tSeconds to find point: %0.2f\n", duration.count()/1000.0);
    }
}