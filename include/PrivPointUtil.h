// By Adam Caulfield (ac7717)
// November 3 2021

#ifndef PRIVPOINTLOC_PRIVPOINTUTIL_H
#define PRIVPOINTLOC_PRIVPOINTUTIL_H

#include "Encryptor.h"

class PrivPointUtil {
public:
    PrivPointUtil();
    // std::vector<long> encodePoint(int, std::vector<int>, int);
    std::vector<long> encodePoint(int, int, int);
    helib::Ctxt secureLT(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>);
    helib::Ctxt secureGT(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>);
    helib::Ctxt secureLT(Encryptor &, int, int, helib::Ctxt, helib::Ctxt);
    helib::Ctxt secureGT(Encryptor &, int, int, helib::Ctxt, helib::Ctxt);
    helib::Ctxt binaryMult(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>, int);
    helib::Ctxt binaryAdd(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>);
};

#endif /*PRIVPOINTLOC_PRIVPOINTUTIL_H*/