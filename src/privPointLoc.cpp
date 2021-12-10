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
#include <string>

int main(int argc, char **argv) {
    PrivPointUtil * privUtil = new PrivPointUtil();
    
    
    printf("-------------------- DEFAULT VERSION -------------------- \n");
    
    std::string mode = argv[1];

    Tree * tree = new Tree();

    printf("----- Read from file, insert into Tree/DAG -----\n");
    tree->readSegmentsFile("data/ac7717.txt");

    printf("----- Print Tree/DAG after read file and insert segments -----\n");
    tree->cleanTree(tree->getRoot());
    tree->printTree(tree->getRoot());

    printf("----- Write to Adjacency Matrix -----\n");
    tree->setupLists(tree->getRoot());
    tree->initAdjacencyMatrix();
    tree->writeAdjacencyMatrix(tree->getRoot());
    tree->writeSumsAdjacencyMatrix();

    printf("----- Path Labels -----\n");
    tree->setupPathLabels(tree->getRoot(), "");
    tree->printPathLabels();

    if(mode=="ptxt"){
        return 0;
    }

    std::vector<std::string> allPaths = tree->getAllPaths();

    int maxBits = atoi(argv[2]);
    int xCoord = atoi(argv[3]);
    int yCoord = atoi(argv[4]);

    printf("\nPLAINTEXT: FINDING POINT: (%d,%d)\n", xCoord,yCoord);
    tree->findPoint(xCoord, yCoord, tree->getRoot());

    printf("-------------------- SETUP ENCRYPTOR OBJECT -------------------- \n");
    unsigned long plaintext_prime_modulus = 2;
    unsigned long phiM = 21845;
    unsigned long lifting = 1;
    unsigned long numOfBitsOfModulusChain = 512+512;
    unsigned long numOfColOfKeySwitchingMatrix = 2;  
    
    Encryptor encryptor("/tmp/sk.txt", "/tmp/pk.txt", plaintext_prime_modulus, phiM, lifting, numOfBitsOfModulusChain, numOfColOfKeySwitchingMatrix);
    
    int nSlots = encryptor.getEncryptedArray()->size();
    std::cout << "Slot count: " << nSlots << std::endl;
    printf("-------------------- Done --------------------\n\n");

    std::vector<long> xPointBits = privUtil->encodePoint(maxBits, xCoord, nSlots);
    std::vector<long> yPointBits = privUtil->encodePoint(maxBits, yCoord, nSlots);

    std::vector<long> pointBits(nSlots);
    std::vector<long> ones(nSlots);
    for(int i=0; i<nSlots; i++){
        
        if(i < maxBits){
            pointBits[i] = xPointBits[i];
            pointBits[i+maxBits] = yPointBits[i];        
        }
        
        ones[i] = 1;
    }

    printf("LOCATE PRIVATE POINT (%d, %d)\n", xCoord, yCoord);
    // for(int i=0; i<maxBits*2; i++){
    //     std::cout << pointBits[i] << " "; 
    // }    
    // std::cout << std::endl;
    printf("---- User side ---- \n");

    helib::Ctxt pointCtxt (*(encryptor.getPublicKey()));
    
    helib::Ctxt resultCtxt(*(encryptor.getPublicKey()));

    printf("\tEncrypting point.... ");
    encryptor.getEncryptedArray()->encrypt(pointCtxt, *(encryptor.getPublicKey()), pointBits);

    printf(" Done\n");

    if(mode=="main"){
        
        printf("\tSending to Cloud for Planar Point Location\n");

        printf("---- Cloud Side .... PARALLEL MODE---- \n");
        
        printf("\tEvaluating all nodes....\n");
        
        auto start = std::chrono::high_resolution_clock::now();
        helib::Ctxt finalResult = tree->evaluateAllNodes(encryptor, privUtil, pointCtxt, maxBits, nSlots);
        auto stop = std::chrono::high_resolution_clock::now();            
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
        int tID = privUtil->processResult(encryptor, finalResult, nSlots, tree->getTotalTrapezoids()+1);
        printf("\tProcessed Result: Point located in Trapezoid ID=%d\n", tID);
        printf("\tSeconds to find point: %0.2f\n", duration.count()/1000.0);        

        
    }
    else if(mode=="path"){
        
        int pathID = atoi(argv[5]);
        std::string pathLabel = allPaths[pathID];

        printf("\tSending to Cloud for Planar Point Location\n");
        printf("---- Cloud Side .... SINGLE PATH MODE ---- \n");
        
        printf("\tBeginning location along path=%d\n", pathID);
        auto start = std::chrono::high_resolution_clock::now();
        
        helib::Ctxt tmpResult(*(encryptor.getPublicKey()));
        encryptor.getEncryptedArray()->encrypt(tmpResult, *(encryptor.getPublicKey()), ones);
        
        tree->evaluateOnePath(encryptor, privUtil, pointCtxt, tmpResult, maxBits, nSlots, pathLabel);

        //result <-- result OR tmpResult <-- XOR(R,T)+AND(R,T)
        helib::Ctxt andCtxt(tmpResult); //and
        andCtxt.multiplyBy(resultCtxt);
        resultCtxt += tmpResult; //xor
        resultCtxt += andCtxt;       //xor + and
        printf("\tPath %d done\n", pathID);
        
        auto stop = std::chrono::high_resolution_clock::now();            
        printf("\tComplete. Sending back to user ....\n");

        printf("---- User Side ---- \n");
        printf("\tReceiving ciphertext form cloud. Decrypting: \n");
        printf("\t");
        encryptor.decryptAndPrintCondensed("Result", resultCtxt, tree->getTotalTrapezoids()+1);
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
        printf("\tSeconds to find point: %0.2f\n", duration.count()/1000.0);
    } 
    else if (mode=="serial"){
        helib::Ctxt tmpResult(*(encryptor.getPublicKey()));
        encryptor.getEncryptedArray()->encrypt(tmpResult, *(encryptor.getPublicKey()), ones);

        printf("\tSending to Cloud for Planar Point Location\n");

        printf("---- Cloud Side .... SERIAL MODE ---- \n");
        
        printf("\tBeginning location....\n");
        
        auto start = std::chrono::high_resolution_clock::now();
        tree->findPrivatePoint(encryptor, privUtil, pointCtxt, resultCtxt, tmpResult, tree->getRoot(), maxBits, nSlots);
        auto stop = std::chrono::high_resolution_clock::now();            

        printf("\tComplete. Sending back to user ....\n");

        printf("---- User Side ---- \n");
        printf("\tReceiving ciphertext form cloud. Decrypting: \n");
        printf("\t");
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
        printf("\tSeconds to find point: %0.2f\n", duration.count()/1000.0);
        encryptor.decryptAndPrintCondensed("Result", resultCtxt, tree->getTotalTrapezoids()+1);
    }
}