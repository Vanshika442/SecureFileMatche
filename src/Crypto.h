#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>

class Crypto {
public:
    // Simple Caesar Cipher (Shift + Key)
    static std::string decrypt(const std::string& input, int key) {
        std::string output = input;
        for (char &c : output) {
            c = c - key; // Shift back
        }
        return output;
    }

    static std::string encrypt(const std::string& input, int key) {
        std::string output = input;
        for (char &c : output) {
            c = c + key; // Shift forward
        }
        return output;
    }
};

#endif // CRYPTO_H
