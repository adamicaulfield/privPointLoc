//
// Copyright SpiRITlab - The PRIVPOINTLOC project.
// https://github.com/SpiRITlab/PRIVPOINTLOC
//

#ifndef PRIVPOINTLOC_ENCRYPTOR_H
#define PRIVPOINTLOC_ENCRYPTOR_H

#include <helib/helib.h>

namespace privPointLoc {

    class Encryptor {
    public:
        Encryptor(const std::string &, const std::string &, long, long, long, long, long);

        Encryptor(const std::string &, const std::string &, long, long, long, long, long, long);

        Encryptor(const std::string &, const std::string &);

        ~Encryptor();

        void testEncryption();

        void decryptAndPrint(const std::string &, const helib::Ctxt &);

        void decryptAndPrintCondensed(const std::string &, const helib::Ctxt &, int);

        helib::Context *getContext() const;

        helib::SecKey *getSecretKey() const;

        helib::PubKey *getPublicKey() const;

        helib::EncryptedArray *getEncryptedArray() const;

        int getSlotCount();

    private:
        // Plaintext prime modulus.
        long plaintextModulus = 2;
        // Cyclotomic polynomial - defines phi(m).
        long phiM = 0;
        // Hensel lifting (default = 1).
        long lifting = 1;
        // Number of bits of the modulus chain.
        long numOfBitsOfModulusChain = 500;
        // Number of columns of Key-Switching matrix (typically 2 or 3).
        long numOfColOfKeySwitchingMatrix = 2;
        // slot count
        long desiredSlotCount = 3000;
        // security level
        long securityLevel = 80;

        helib::Context *context;
        helib::SecKey *secret_key;
        helib::PubKey *public_key;
        helib::EncryptedArray *encrypted_array;
    };

}

#endif //PRIVPOINTLOC_ENCRYPTOR_H
