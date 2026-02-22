#include <iostream>
#include <fstream>
#include "Crypto.h"

int main() {
    std::string text = "The Knuth-Morris-Pratt string searching algorithm (or KMP algorithm) searches for occurrences of a word W within a main text string S by employing the observation that when a mismatch occurs, the word itself embodies sufficient information to determine where the next match could begin, thus bypassing re-examination of previously matched characters.";
    
    // Encrypt with Key 1
    std::string encrypted = Crypto::encrypt(text, 1);
    
    std::ofstream out("../test_data/sample_encrypted.txt"); // Writing to relative path
    if (out.is_open()) {
        out << encrypted;
        out.close();
        std::cout << "File generated successfully." << std::endl;
    } else {
        std::cout << "Error opening file." << std::endl;
    }
    return 0;
}
