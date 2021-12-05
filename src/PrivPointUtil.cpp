#include "PrivPointUtil.h"
#include <cmath>
#include <chrono>

PrivPointUtil::PrivPointUtil(){
}

// ---------------- CLOUD OWNED FUNCTIONS ------------------- //
std::vector<long> PrivPointUtil::encodePoint(int maxBits, int p, int nSlots){
    bool negative = (p < 0);
    int point = std::abs(p);
    std::vector<long> pointBits(nSlots,0);

    for(int i=0; i<maxBits; i++){
        pointBits[i] = long(point/(pow(2,i)))%2;

        // Flip bit for two's compliment
        if(negative){
            pointBits[i] = (pointBits[i]+1)%2;            
        }
    }

    // Add 1 to complete two's compliment
    if(negative){
        int cin = 1;
        int tmp;
        for(int i=0; i<maxBits; i++){
            tmp = (pointBits[i] + cin)%2; // sum
            cin = pointBits[i] * cin; //carry out
            pointBits[i] = tmp;
        }
    }

    return pointBits;
}

// Compare CTXT(A) to PTXT(B)
helib::Ctxt PrivPointUtil::secureGT(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b){
    // printf("\tStarting secureGT(ctxt,ptxt)...");
    // printf("\n");
    auto start = std::chrono::high_resolution_clock::now();
    helib::Ptxt<helib::BGV> bitFilter (*(encryptor.getContext()));
    for(int i=0; i<maxBits; i++){
        bitFilter[i] = 1;
    }
    a.multByConstant(bitFilter);

    // printf("\n");
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("a", a, 2*maxBits);

    helib::Ptxt<helib::BGV> notOp (*(encryptor.getContext())); // plaintext vector set to ones for 'not' operation 
    // printf("\t\t");
    // printf("b: ");
    for(int i=0; i<maxBits; i++){
        notOp[i] = 1;
        // std::cout << b[i] << " ";
    }
    // std::cout << std::endl;

    // Equal to
    helib::Ctxt eq(a); // eq = A
    eq.addConstant(b); // eq = XOR(A,B)
    eq.addConstant(notOp); // eq = XNOR(A,B)
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("eq", eq, 2*maxBits);
    // Compares if ctxt > ptxt
    // Binary A > B --> A and not B

    b += notOp; // b now contains not B
    a.multByConstant(b); // a contains A and not B
    // Individual gt results are in each slot of a.
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("A and not B", a, 2*maxBits);

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
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("before total sums", a, 2*maxBits);
    helib::totalSums(*encryptor.getEncryptedArray(), a);
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("a > b", a, 2*maxBits);

     // auto stop = std::chrono::high_resolution_clock::now();
     // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
     // printf("Done (%0.2fs)\n", duration.count()/1000.0);
    return a;
}

helib::Ctxt PrivPointUtil::secureLT(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b){
    // printf("\tStarting secureLT(ctxt,ptxt)...");
    // printf("\n");
    auto start = std::chrono::high_resolution_clock::now();
    helib::Ptxt<helib::BGV> bitFilter (*(encryptor.getContext()));
    for(int i=0; i<maxBits; i++){
        bitFilter[i] = 1;
    }
    a.multByConstant(bitFilter);

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
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    // printf("\tDone (%0.2fs)\n", duration.count()/1000.0);
    return a;
}

// COMPARE CTXT(A) to CTXT(A)
helib::Ctxt PrivPointUtil::secureGT(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ctxt b){
    // printf("\tStarting secureGT(ctxt,ctxt)...");
    // printf("\n");
    auto start = std::chrono::high_resolution_clock::now();
    helib::Ptxt<helib::BGV> bitFilter (*(encryptor.getContext()));
    for(int i=0; i<maxBits; i++){
        bitFilter[i] = 1;
    }
    a.multByConstant(bitFilter);
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("a", a, maxBits);
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("b", b, maxBits);

    helib::Ptxt<helib::BGV> notOp (*(encryptor.getContext())); // plaintext vector set to ones for 'not' operation 
    for(int i=0; i<maxBits; i++){
        notOp[i] = 1;
    }

    // Equal to
    helib::Ctxt eq(a); // eq = A
    eq += b; // eq = XOR(A,B)
    eq.addConstant(notOp); // eq = XNOR(A,B)
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("eq", eq, maxBits);

    // Compares if ctxt > ptxt
    // Binary A > B --> A and not B

    b.addConstant(notOp); // b now contains not B
    a.multiplyBy(b); // a contains A and not B
    // Individual gt results are in each slot of a.
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("A and not B", a, maxBits);

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
    }
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("before total sums", a, 2*maxBits);
    helib::totalSums(*encryptor.getEncryptedArray(), a);
    // printf("\t\t");
    // encryptor.decryptAndPrintCondensed("ctxt > ptxt", a, maxBits);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    // printf("\tDone (%0.2fs)\n", duration.count()/1000.0);
    return a;
}

helib::Ctxt PrivPointUtil::secureLT(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ctxt b){
    // printf("\tStarting secureLT(ctxt,ctxt)...");
    // printf("\n");
    auto start = std::chrono::high_resolution_clock::now();
    helib::Ptxt<helib::BGV> bitFilter (*(encryptor.getContext()));
    for(int i=0; i<maxBits; i++){
        bitFilter[i] = 1;
    }
    a.multByConstant(bitFilter);


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
    eq += b; // eq = XOR(A,B)
    eq.addConstant(notOp); // eq = XNOR(A,B)

    // Less Than
    a.addConstant(notOp); // a now contains not a
    a.multiplyBy(b); // a contains not A and B (Less Than)

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
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    // printf("\tDone (%0.2fs)\n", duration.count()/1000.0);
    // encryptor.decryptAndPrintCondensed("ctxt < ptxt", a, maxBits);
    return a;
}

helib::Ctxt PrivPointUtil::binaryMult(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b, int y){
    // printf("\tStarting binaryMult(ctxt, ptxt)...\n");
    // auto start = std::chrono::high_resolution_clock::now();
    helib::Ptxt<helib::BGV> pointMask (*(encryptor.getContext()));
    helib::Ptxt<helib::BGV> zeros (*(encryptor.getContext()));

    helib::Ctxt binaryProd(*(encryptor.getPublicKey()));

    helib::Ctxt carry(*(encryptor.getPublicKey()));

    helib::Ptxt<helib::BGV> bitFilter (*(encryptor.getContext()));
    for(int i=0; i<maxBits; i++){
        pointMask[i+maxBits*y] = 1;
        bitFilter[i] = 1;
    }

    a.multByConstant(pointMask); // filter only the x coordinate

    // Multiplying the Y coordinate, need to rotate into position
    if(y==1){
        encryptor.getEncryptedArray()->rotate(a, -1*maxBits);
    }
    // encryptor.decryptAndPrintCondensed("rotate(a,-maxBits)", a, 2*maxBits);

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
    binaryProd.multByConstant(bitFilter); // Ensure result is only maxBits length
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    // printf("\tDone (%0.2fs)\n", duration.count()/1000.0);
    return binaryProd;
}

helib::Ctxt PrivPointUtil::binaryAdd(Encryptor &encryptor, int maxBits, int nSlots, helib::Ctxt a, helib::Ptxt<helib::BGV> b){
    // printf("\tStarting binaryAdd(ctxt, ptxt)...\n");
    // auto start = std::chrono::high_resolution_clock::now();
    helib::Ptxt<helib::BGV> bitFilter (*(encryptor.getContext()));
    for(int i=0; i<maxBits; i++){
        bitFilter[i] = 1;
    }
    a.multByConstant(bitFilter);

    helib::Ctxt carry(*(encryptor.getPublicKey()));
    helib::Ptxt<helib::BGV> accum(*(encryptor.getContext()));;
    helib::Ctxt sum(a);

    for(int i=0; i<maxBits; i++){
        for(int j=0; j<nSlots; j++){
            if(j==i){
                accum[i] = b[i];
            } else {
                accum[i] = 0;
            }
        }


        helib::Ctxt tmp1(sum);
        helib::Ctxt tmp2(carry);
        helib::Ctxt tmp3(sum);

        tmp1.multByConstant(accum);
        carry.multByConstant(accum);
        tmp3.multiplyBy(tmp2);

        //Sum = a + C_in + tmp
        sum += tmp2;
        sum += accum;

        carry += tmp3;
        carry += tmp1;  
        // encryptor.decryptAndPrintCondensed("   c_out   ", carry, 2*maxBits);
        
        // Rotate forward so it is used as C_in for next round
        encryptor.getEncryptedArray()->rotate(carry, 1);
    }
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop-start);
    // printf("\tDone (%0.2fs)\n", duration.count()/1000.0);
    return sum;
}

// ---------------- USER OWNED FUNCTIONS ------------------- //
int PrivPointUtil::processResult(Encryptor &encryptor, helib::Ctxt resultCtxt, int nSlots, int total){
    std::vector<long> resultPtxt(encryptor.getEncryptedArray()->size());
    encryptor.getEncryptedArray()->decrypt(resultCtxt, *(encryptor.getSecretKey()), resultPtxt);
    std::cout << "\tDecrypt(Result):";
    int tID = 0;
    for(int i=0; i<total; i++){
        std::cout << resultPtxt[i] << " ";
        if(resultPtxt[i]==1){
            tID = i;
        }
    }
    std::cout << std::endl;
    return tID;
}