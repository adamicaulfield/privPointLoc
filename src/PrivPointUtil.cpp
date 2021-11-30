#include "PrivPointUtil.h"

PrivPointUtil::PrivPointUtil(){

}

// std::vector<long> PrivPointUtil::encodePoint(int maxBits, std::vector<int> point, int nSlots){
//   std::vector<long> pointBits(nSlots,0);
  
//   for(int i=0; i<maxBits; i++){
//     pointBits[i] = long(point[0]/(pow(2,i)))%2;
//     pointBits[i+maxBits] = long(point[1]/(pow(2,i)))%2; // y coordinate
//   }

//   return pointBits;
// }

std::vector<long> PrivPointUtil::encodePoint(int maxBits, int point, int nSlots){
  std::vector<long> pointBits(nSlots,0);
  
  for(int i=0; i<maxBits; i++){
    pointBits[i] = long(point/(pow(2,i)))%2;
  }

  return pointBits;
}

helib::Ctxt PrivPointUtil::secureGT(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b){
    printf("--------Starting secureGT-------");
    
    helib::Ptxt<helib::BGV> notOp (*(encryptor.getContext())); // plaintext vector set to ones for 'not' operation 
    for(int i=0; i<maxBits; i++){
        notOp[i] = 1;
    }

    // Equal to
    helib::Ctxt eq(a); // eq = A
    eq.addConstant(b); // eq = XOR(A,B)
    eq.addConstant(notOp); // eq = XNOR(A,B)

    // Compares if ctxt > ptxt
    // Binary A > B --> A and not B

    b += notOp; // b now contains not B
    a.multByConstant(b); // a contains A and not B
    // Individual gt results are in each slot of a.
    
    // Assuming 4-bit example
    // After getting bitwise results G <-- A and not B, E <-- A == B
    // Whole result is R <-- G4 or E4G3 or E4E3G2 or E4E3E2G1
    helib::Ctxt tmp(eq);

    // Unused slots should = 1 in the tmp result, because rotation and mult by zero will effect result
    // Consider as AND-ing with extra 1's (identity op)
    for(int i=0; i<nSlots; i++){
        if(i<maxBits){
            notOp[i] = 0;
        } else {
            notOp[i] = 1;
        }
    }
    tmp.addConstant(notOp);

    encryptor.getEncryptedArray()->rotate(tmp, -1);

    for(int i=1; i<maxBits; i++){
        a.multiplyBy(tmp);
        encryptor.getEncryptedArray()->rotate(tmp, -1);
        // encryptor.decryptAndPrintCondensed("tmp", tmp, 2*maxBits);
    }
    helib::totalSums(*encryptor.getEncryptedArray(), a);
    // encryptor.decryptAndPrintCondensed("ctxt > ptxt", a, maxBits);
    printf("Done-------\n");
    return a;
}

helib::Ctxt PrivPointUtil::secureLT(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b){
    printf("--------Starting secureLT-------");
    
    helib::Ptxt<helib::BGV> notOp (*(encryptor.getContext())); // plaintext vector set to ones for 'not' operation 
    helib::Ptxt<helib::BGV> firstSlot (*(encryptor.getContext())); // filter out first slot only

    firstSlot[0] = 1;
    for(int i=0; i<maxBits; i++){
        notOp[i] = 1;
    }

    // Compares if ctxt <= ptxt
    // Binary A < B --> not A and B
    // Binary A == B --> XNOR(A,B) = not XOR(A,B);

    // Equal to
    helib::Ctxt eq(a); // eq = A
    eq.addConstant(b); // eq = XOR(A,B)
    eq.addConstant(notOp); // eq = XNOR(A,B)

    // Less Than
    a.addConstant(notOp); // a now contains not a
    a.multByConstant(b); // a contains not A and B (Less Than)

    // Individual LT results are in each slot of a, individual EQ results ar ein each slot of eq
    helib::Ctxt tmp(eq);
    
    // Unused slots should = 1 in the tmp result, because rotation and mult by zero will effect result
    // Consider as AND-ing with extra 1's (identity op)
    for(int i=0; i<nSlots; i++){
        if(i<maxBits){
            notOp[i] = 0;
        } else {
            notOp[i] = 1;
        }
    }
    tmp.addConstant(notOp);

    encryptor.getEncryptedArray()->rotate(tmp, -1);

    // For a 4-bit example
    // Given bitwise results L <-- A < B, E <-- A == B
    // Final result R <-- A <= B <-- L4 or E4L3 or E4E3L2 or E4E3E2L1 or E4E3E2E1 
    for(int i=1; i<maxBits-1; i++){
        a.multiplyBy(tmp);
        eq.multiplyBy(tmp);
        encryptor.getEncryptedArray()->rotate(tmp, -1);
        // encryptor.decryptAndPrintCondensed("tmp", tmp, 2*maxBits);
    } // Now has L4 or E4L3 or E4E3L2 or E4E3E2L1 at each slot
    eq.multByConstant(firstSlot); // Add in the case where the two are equal (E4E3E2E1)
    a += eq; // slots are (E4E3E2L1 or E4E3E2E1), E4E3L2, E4L3, L4
    helib::totalSums(*encryptor.getEncryptedArray(), a); // sum all results

    // encryptor.decryptAndPrintCondensed("ctxt < ptxt", a, maxBits);
    printf("Done-------\n");
    return a;
}

helib::Ctxt PrivPointUtil::binaryMult(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b, int y){
    printf("--------Starting binaryMult-------");

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

    // printf("\n");
    // encryptor.decryptAndPrintCondensed("FINAL: binaryProd", binaryProd, 2*maxBits);
    printf("Done-------\n");
    return binaryProd;
}
