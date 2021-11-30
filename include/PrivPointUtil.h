// By Adam Caulfield (ac7717)
// November 3 2021

#ifndef PRIVPOINTLOC_PRIVPOINTUTIL_H
#define PRIVPOINTLOC_PRIVPOINTUTIL_H

#include "Encryptor.h"

class PrivPointUtil {
public:
    PrivPointUtil();
    std::vector<long> encodePoint(int, std::vector<int>, int nSlots);
    void secureLT(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>, int);
    void secureGT(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>, int);
    void binaryMult(Encryptor &, int, int, helib::Ctxt, helib::Ptxt<helib::BGV>, int);
};

#endif /*PRIVPOINTLOC_PRIVPOINTUTIL_H*/