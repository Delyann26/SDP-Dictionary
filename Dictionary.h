#pragma once
#include <iostream>
#include <stdexcept>
#include <unordered_set>

static constexpr size_t ALPHABET_SIZE = 26;

class incorrect_word_exception : public std::runtime_error {
public:
    incorrect_word_exception()
        : runtime_error("incorrect word")
    {
        // Nothing to do here
    }
};

class Dictionary {
public:
    Dictionary();
    Dictionary(const Dictionary& other);
    Dictionary& operator=(const Dictionary& other);
    ~Dictionary();

    void insert(const char* word);
    void erase(const char* word) noexcept;
    bool contains(const char* word) const noexcept;
    size_t size() const noexcept;
    static bool isCorrectWord(const char* word) noexcept;

    void loadFiles(const char* fileName1, const char* fileName2, const char* fileName3);
    void printDictionary() const; //for testing
private:
    struct TrieNode
    {
        TrieNode* children[ALPHABET_SIZE];
        bool terminal;
        unsigned occurrances;//this attribute is needed for the statistics

        TrieNode() :terminal(false), occurrances(0)
        {
            for (size_t i = 0; i < ALPHABET_SIZE; i++)
            {
                children[i] = nullptr;
            }
        }
    };

    TrieNode* root;
    size_t count;

private:
    void loadDictionaryFile(const char* fileName, int& correct_words, int& incorrect_words);
    void loadFilterFile(const char* fileName, int& correct_words, int& incorrect_words, int& resultant);
    void loadTextFile(const char* fileName, int& correct_words, int& incorrect_words) const;
    void showStats(int c_dict, int i_dict, int c_filter, int i_filter, int result, int c_text, int i_text, bool showTextStats) const;

    void free();
    void copyFrom(const Dictionary& other);

    void freeRec(TrieNode*& root);
    void copyFromRec(TrieNode*& root, TrieNode* otherRoot);

    bool eraseRec(TrieNode*& root, const char* word, unsigned depth);

    void collectWords(TrieNode* root, std::string& prefix, std::vector<std::string>& words) const; //using it when printing, so its for testing too

    bool hasChildren(const TrieNode* root) const;

    unsigned getWordOccurrances(const char* word) const;
};
