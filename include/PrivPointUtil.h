/**
* Authored by Adam Caulfield (ac7717). 
* November 3 2021.
*/

#ifndef PRIVPOINTLOC_PRIVPOINTUTIL_H
#define PRIVPOINTLOC_PRIVPOINTUTIL_H

#include "Encryptor.h"

class PrivPointUtil {
public:
    PrivPointUtil();
    
    /**
       * Given the integer, the max number of bits, and the number of ciphertext slots, encodes the integer into a vector of binary.
       * It is encoded such that the ith index has the ith bit.
       * @param int value: the integer
       * @param int maxBits: bit-length
       * @param int nSlots: ciphertext slots
       * @return std::vector<long> 
    */
    std::vector<long> encodePoint(int, int, int);

    /**
       * Conducts secure less than or equal to operation between a ciphertext and a plaintext vector (for X-Node). Implements the secure comparison by Aloufi et. al.
       * @param Encryptor &encryptor - encryptor object
       * @param int maxBits - bit-length
       * @param int nSlots - ciphertext slots
       * @param helib::Ctxt - binary value A, encrypted as a ciphertext
       * @param helib::Ptxt<helib::BGV> - binary value B, encoded as a plaintext vector, specifying the BGV scheme
       * @return helib::Ctxt - ciphertext representing A <= B
    */
    helib::Ctxt secureLT(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>);

    /**
       * Conducts greater than operation between a ciphertext and a plaintext vector (for X-Node). Implements the secure comparison by Aloufi et. al.
       * @param Encryptor &encryptor - encryptor object
       * @param int maxBits - bit-length
       * @param int nSlots - ciphertext slots
       * @param helib::Ctxt - binary value A, encrypted as a ciphertext
       * @param helib::Ptxt<helib::BGV> - binary value B, encoded as a plaintext vector, specifying the BGV scheme
       * @return helib::Ctxt - ciphertext representing A > B
    */
    helib::Ctxt secureGT(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>);

    /**
       * Conducts secure less than or equal to operation between two ciphertexts (for Y-Node). Implements the secure comparison by Aloufi et. al.
       * @param Encryptor &encryptor - encryptor object
       * @param int maxBits - bit-length
       * @param int nSlots - ciphertext slots
       * @param helib::Ctxt - binary value A, encrypted as a ciphertext
       * @param helib::Ctxt - binary value B, encrypted as a ciphertext
       * @return helib::Ctxt - ciphertext representing A <= B
    */
    helib::Ctxt secureLT(Encryptor &, int, int, helib::Ctxt, helib::Ctxt);

    /**
       * Conducts secure greater than operation between two ciphertexts (for Y-Node). Implements the secure comparison by Aloufi et. al.
       * @param Encryptor &encryptor - encryptor object
       * @param int maxBits - bit-length
       * @param int nSlots - ciphertext slots
       * @param helib::Ctxt - binary value A, encrypted as a ciphertext
       * @param helib::Ctxt - binary value B, encrypted as a ciphertext
       * @return helib::Ctxt - ciphertext representing A > B
    */
    helib::Ctxt secureGT(Encryptor &, int, int, helib::Ctxt, helib::Ctxt);


    /**
       * Conducts binary addition (full adder) between binary integers in a ciphertext and plaintext
       * @param Encryptor &encryptor - encryptor object
       * @param int maxBits - bit-length
       * @param int nSlots - ciphertext slots
       * @param helib::Ctxt - binary value A, encrypted as a ciphertext
       * @param helib::Ctxt - binary value B, encoded as a plaintext vector, specifying the BGV scheme
       * @return helib::Ctxt - returns the binary value of A + B
    */
    helib::Ctxt binaryAdd(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>);

    /**
       * Conducts binary multiplier between binary integers in a ciphertext and plaintext
       * @param Encryptor &encryptor - encryptor object
       * @param int maxBits - bit-length
       * @param int nSlots - ciphertext slots
       * @param helib::Ctxt - binary value A, encrypted as a ciphertext
       * @param helib::Ctxt - binary value B, encoded as a plaintext vector, specifying the BGV scheme
       * @param int y - multiplying the y (1) or x (0) value of the point.
       * @return helib::Ctxt - returns the binary value of A * B
    */
    helib::Ctxt binaryMult(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>, int);    

    /**
       * Takes a ciphertext expected to contain the result. Decrypts it and converts the binary number into a decimal integer.
       * @param Encryptor &encryptor - encryptor object
       * @param helib::Ctxt - result ciphertext
       * @param int nSlots - ciphertext slots
       * @param int total - total bits to print
       * @return int - returns the decimal integer of the result
    */
    int processResult(Encryptor &, helib::Ctxt, int, int);
};

#endif /*PRIVPOINTLOC_PRIVPOINTUTIL_H*/