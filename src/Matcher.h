#ifndef MATCHER_H
#define MATCHER_H

#include <string>
#include <vector>
#include <memory>

// Strategy Interface
class SearchStrategy {
public:
    virtual ~SearchStrategy() = default;
    // Returns the index of the first occurrence of pattern in text, or -1 if not found.
    virtual int search(const std::string& text, const std::string& pattern) = 0;
    virtual std::string getName() const = 0;
};

// Naive Algorithm
class NaiveSearch : public SearchStrategy {
public:
    int search(const std::string& text, const std::string& pattern) override;
    std::string getName() const override { return "Naive"; }
};

// KMP Algorithm
class KMPSearch : public SearchStrategy {
public:
    int search(const std::string& text, const std::string& pattern) override;
    std::string getName() const override { return "KMP"; }
private:
    std::vector<int> computeLPSArray(const std::string& pattern);
};

// Rabin-Karp Algorithm
class RabinKarpSearch : public SearchStrategy {
public:
    int search(const std::string& text, const std::string& pattern) override;
    std::string getName() const override { return "Rabin-Karp"; }
};

class MatcherFactory {
public:
    static std::unique_ptr<SearchStrategy> create(const std::string& algorithmName);
};

#endif // MATCHER_H
