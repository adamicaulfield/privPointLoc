#include "PrivPointUtil.h"

PrivPointUtil::PrivPointUtil(){

}

std::vector<long> PrivPointUtil::encodePoint(int maxBits, std::vector<int> point, int nSlots){
  std::vector<long> pointBits(nSlots,0);
  
  for(int i=0; i<maxBits; i++){
    pointBits[i] = long(point[0]/(pow(2,i)))%2;
    pointBits[i+maxBits] = long(point[1]/(pow(2,i)))%2; // y coordinate
  }

  return pointBits;
}

void PrivPointUtil::secureGT(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b, int y){
    printf("--------Starting secureGT-------\n");
    helib::Ptxt<helib::BGV> mask (*(encryptor.getContext()));
    for(int i=0; i<maxBits; i++){
        mask[i+maxBits*y] = 1;
    }
    // Compares if ctxt > ptxt
    // Binary A > B --> A and not B
    helib::Ptxt<helib::BGV> notOp (*(encryptor.getContext())); // plaintext vector set to ones for 'not' operation 
    for(int i=0; i<nSlots; i++){
        notOp[i] = 1;
    }
    a.multByConstant(mask);
    b.multiplyBy(mask);

    // encryptor.decryptAndPrintCondensed("pointCtxt", a, 2*maxBits);

    b += notOp; // b now contains not B
    a.multByConstant(b); // a contains A and not B
    // encryptor.decryptAndPrintCondensed("individual ctxt >= ptxt", a, 2*maxBits);
    
    // Individual gt results are in each slot of a.
    helib::Ctxt tmp = a;
    tmp.addConstant(notOp); // not of results'

    encryptor.getEncryptedArray()->rotate(tmp, -1);
    // encryptor.decryptAndPrintCondensed("tmp", tmp, 2*maxBits);
    for(int i=1; i<maxBits; i++){
        a.multiplyBy(tmp);
        encryptor.getEncryptedArray()->rotate(tmp, -1);
        // encryptor.decryptAndPrintCondensed("tmp", tmp, 2*maxBits);
    }
    helib::totalSums(*encryptor.getEncryptedArray(), a);
    encryptor.decryptAndPrintCondensed("ctxt > ptxt", a, maxBits);
    printf("--------Done-------\n\n");
}

void PrivPointUtil::secureLT(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b, int y){
    printf("--------Starting secureLT-------\n");
    helib::Ptxt<helib::BGV> mask (*(encryptor.getContext()));

    for(int i=0; i<maxBits; i++){
        mask[i+maxBits*y] = 1;
    }
    // Compares if ctxt > ptxt
    // Binary A > B --> A and not B
    helib::Ptxt<helib::BGV> notOp (*(encryptor.getContext())); // plaintext vector set to ones for 'not' operation 
    for(int i=0; i<nSlots; i++){
        notOp[i] = 1;
    }
    a.multByConstant(mask);
    b.multiplyBy(mask);

    // encryptor.decryptAndPrintCondensed("pointCtxt", a, 2*maxBits);

    a.addConstant(notOp); // a now contains not a
    a.multByConstant(b); // a contains not A and B
    // encryptor.decryptAndPrintCondensed("individual ctxt >= ptxt", a, 2*maxBits);
    
    // Individual gt results are in each slot of a.
    helib::Ctxt tmp = a;
    tmp.addConstant(notOp); // not of results'

    encryptor.getEncryptedArray()->rotate(tmp, -1);
    // encryptor.decryptAndPrintCondensed("tmp", tmp, 2*maxBits);
    for(int i=1; i<maxBits; i++){
        a.multiplyBy(tmp);
        encryptor.getEncryptedArray()->rotate(tmp, -1);
        // encryptor.decryptAndPrintCondensed("tmp", tmp, 2*maxBits);
    }
    helib::totalSums(*encryptor.getEncryptedArray(), a);
    encryptor.decryptAndPrintCondensed("ctxt < ptxt", a, maxBits);
    printf("--------Done-------\n\n");
}

void PrivPointUtil::binaryMult(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b, int y){
    printf("--------Starting binaryMult-------\n");

    helib::Ptxt<helib::BGV> pointMask (*(encryptor.getContext()));
    helib::Ptxt<helib::BGV> zeros (*(encryptor.getContext()));

    helib::Ctxt binaryProd(*(encryptor.getPublicKey()));

    helib::Ctxt carry(*(encryptor.getPublicKey()));

    for(int i=0; i<maxBits; i++){
        pointMask[i+maxBits*y] = 1;
    }

    a.multByConstant(pointMask); // filter only the x coordinate

    // Incremental Binary Multiplication
    for(int i=0; i<maxBits; i++){

        helib::Ptxt<helib::BGV> mask (*(encryptor.getContext()));
        helib::Ctxt tmp(a);
        // tmp += a;
        for(int j=0; j<maxBits; j++){
            mask[j] = b[i];
        }
        tmp.multByConstant(mask);
        encryptor.getEncryptedArray()->rotate(tmp, i);
        
        // FULL ADDER
        // encryptor.decryptAndPrintCondensed("   c_in   ", carry, 2*maxBits);
        // encryptor.decryptAndPrintCondensed("rot([tmp])", tmp, 2*maxBits);
        // encryptor.decryptAndPrintCondensed("binaryProd", binaryProd, 2*maxBits);

        helib::Ctxt tmp1(binaryProd);
        helib::Ctxt tmp2(carry);
        helib::Ctxt tmp3(binaryProd);

        tmp1.multiplyBy(tmp);
        carry.multiplyBy(tmp);
        tmp3.multiplyBy(tmp2);

        //Sum = binaryProd + C_in + tmp
        binaryProd += tmp2;
        binaryProd += tmp;
        
        // printf("\t");
        // encryptor.decryptAndPrintCondensed("bp AND tmp", tmp1, 2*maxBits);
        // printf("\t");
        // encryptor.decryptAndPrintCondensed("c_in AND tmp", carry, 2*maxBits);
        // printf("\t");
        // encryptor.decryptAndPrintCondensed("c_in AND bp", tmp3, 2*maxBits);
        
        carry += tmp3;
        carry += tmp1;  
        // encryptor.decryptAndPrintCondensed("   c_out   ", carry, 2*maxBits);
        
        // Rotate forward so it is used as C_in for next round
        encryptor.getEncryptedArray()->rotate(carry, 1);
        
        // printf("\n");
    }
    // Add final Carry
    binaryProd += carry;

    printf("\n");

    encryptor.decryptAndPrintCondensed("FINAL: binaryProd", binaryProd, 2*maxBits);
    printf("--------Done-------\n\n");
}