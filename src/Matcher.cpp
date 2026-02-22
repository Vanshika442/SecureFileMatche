#include "Matcher.h"
#include <vector>
#include <iostream>

// --- Naive Search ---
int NaiveSearch::search(const std::string& text, const std::string& pattern) {
    int n = text.length();
    int m = pattern.length();
    if (m == 0) return 0;
    if (m > n) return -1;

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (text[i + j] != pattern[j])
                break;
        }
        if (j == m)
            return i;
    }
    return -1;
}

// --- KMP Search ---
std::vector<int> KMPSearch::computeLPSArray(const std::string& pattern) {
    int m = pattern.length();
    std::vector<int> lps(m);
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

int KMPSearch::search(const std::string& text, const std::string& pattern) {
    int n = text.length();
    int m = pattern.length();
    if (m == 0) return 0;
    if (m > n) return -1;

    std::vector<int> lps = computeLPSArray(pattern);
    int i = 0; // index for text
    int j = 0; // index for pattern
    while (i < n) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        if (j == m) {
            return i - j;
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0)
                j = lps[j - 1];
            else
                i = i + 1;
        }
    }
    return -1;
}

// --- Rabin-Karp Search ---
int RabinKarpSearch::search(const std::string& text, const std::string& pattern) {
    int n = text.length();
    int m = pattern.length();
    if (m == 0) return 0;
    if (m > n) return -1;

    int d = 256; // number of characters in the input alphabet
    int q = 101; // A prime number
    int h = 1;
    int p = 0; // hash value for pattern
    int t = 0; // hash value for text

    // The value of h would be "pow(d, m-1)%q"
    for (int i = 0; i < m - 1; i++)
        h = (h * d) % q;

    // Calculate the hash value of pattern and first window of text
    for (int i = 0; i < m; i++) {
        p = (d * p + pattern[i]) % q;
        t = (d * t + text[i]) % q;
    }

    // Slide the window
    for (int i = 0; i <= n - m; i++) {
        if (p == t) {
            bool found = true;
            for (int j = 0; j < m; j++) {
                if (text[i + j] != pattern[j]) {
                    found = false;
                    break;
                }
            }
            if (found) return i;
        }

        if (i < n - m) {
            t = (d * (t - text[i] * h) + text[i + m]) % q;
            if (t < 0) t = (t + q);
        }
    }
    return -1;
}

// --- Factory ---
std::unique_ptr<SearchStrategy> MatcherFactory::create(const std::string& algorithmName) {
    if (algorithmName == "kmp") {
        return std::make_unique<KMPSearch>();
    } else if (algorithmName == "rabin-karp") {
        return std::make_unique<RabinKarpSearch>();
    } else {
        return std::make_unique<NaiveSearch>();
    }
}
