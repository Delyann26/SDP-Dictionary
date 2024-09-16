#include "Dictionary.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cctype> // Contains std::isalpha()



Dictionary::Dictionary() :root(nullptr), count(0)
{
	root = new TrieNode();
}

Dictionary::Dictionary(const Dictionary& other) :root(nullptr), count(0)
{
	copyFrom(other);
}

Dictionary& Dictionary::operator=(const Dictionary& other)
{
	if (this != &other)
	{
		free();
		copyFrom(other);
	}
	return *this;
}

Dictionary::~Dictionary()
{
	free();
}

void Dictionary::loadFiles(const char* fileName1, const char* fileName2, const char* fileName3)
{
	int correct_words_from_dict = 0;
	int incorrect_words_from_dict = 0;

	int correct_words_from_filter = 0;
	int incorrect_words_from_filter = 0;
	int resultant_words_after_filtering = 0;

	int correct_words_from_text = 0;
	int incorrect_words_from_text = 0;
	try
	{
		loadDictionaryFile(fileName1, correct_words_from_dict, incorrect_words_from_dict);
	}
	catch (const std::invalid_argument& err)
	{
		std::cout << err.what() << std::endl;
		return;
	}
	if (size() == 0)
	{
		std::cout << "Empty dictionary!" << std::endl;
		return;
	}
	resultant_words_after_filtering = correct_words_from_dict;
	try
	{
		loadFilterFile(fileName2, correct_words_from_filter, incorrect_words_from_filter, resultant_words_after_filtering);
	}
	catch (const std::invalid_argument& err)
	{
		std::cout << err.what();
		std::cout << "Filtering skipped." << std::endl;
	}
	try
	{
		loadTextFile(fileName3, correct_words_from_text, incorrect_words_from_text);
	}
	catch (const std::invalid_argument& err)
	{
		showStats(correct_words_from_dict, incorrect_words_from_dict, correct_words_from_filter, incorrect_words_from_filter, resultant_words_after_filtering, correct_words_from_text, incorrect_words_from_text, false);
		return;
	}
	showStats(correct_words_from_dict, incorrect_words_from_dict, correct_words_from_filter, incorrect_words_from_filter, resultant_words_after_filtering, correct_words_from_text, incorrect_words_from_text, true);
}

void Dictionary::insert(const char* word)
{
	if (!isCorrectWord(word))
	{
		throw incorrect_word_exception();
	}
	TrieNode* iter = root;
	char c;
	for (size_t i = 0; i < strlen(word); i++)
	{
		c = tolower(word[i]);
		if (iter->children[c - 'a'] == nullptr)
		{
			TrieNode* newNode = new TrieNode();
			iter->children[c - 'a'] = newNode;
		}
		iter = iter->children[c - 'a'];
	}
	iter->terminal = true;
	iter->occurrances++;
	if (iter->occurrances == 1)
	{
		count++;
	}
}

void Dictionary::erase(const char* word) noexcept
{
	if (!isCorrectWord(word))
		return;
	
	eraseRec(root, word, 0);

}

bool Dictionary::contains(const char* word) const noexcept
{
	if (root == nullptr || !isCorrectWord(word))
		return false;

	return getWordOccurrances(word) >= 1;
}

size_t Dictionary::size() const noexcept
{
	return count;
}

bool Dictionary::isCorrectWord(const char* word) noexcept
{
	if (word == nullptr || strlen(word) == 0)
	{
		return false;
	}
	char c;
	for (size_t i = 0; i < strlen(word); i++)
	{
		c = word[i];
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
		{
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;
}

void Dictionary::printDictionary() const
{
	std::vector<std::string> words;
	std::string prefix;
	collectWords(root, prefix, words);

	for (size_t i = 0; i < words.size(); i++)
	{
		std::cout << words[i] << std::endl;
	}
}

void Dictionary::loadDictionaryFile(const char* fileName, int& correct_words, int& incorrect_words)
{
	std::ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		throw std::invalid_argument("Invalid dictionary file!");
	}
	std::cout << "Loading dictionary from: " << fileName << std::endl;
	char line[128];
	int i = 0;
	while (ifs.getline(line, sizeof(line), '\n'))
	{
		if (!ifs)
		{
			break;
		}
		if (line[0] == '#' || strlen(line) == 0)
		{
			continue;
		}
		i++;
		try
		{
			insert(line);
			correct_words++;
		}
		catch (const incorrect_word_exception&)
		{
			incorrect_words++;
			std::cout << "ERROR: incorrect entry " << "\"" << line << "\"" << " on line " << i << std::endl;
		}
	}
	ifs.close();
	std::cout << std::endl;
}

void Dictionary::loadFilterFile(const char* fileName, int& correct_words, int& incorrect_words, int& resultant)
{
	std::ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		throw std::invalid_argument("Invalid filter file! ");
	}
	std::cout << "Removing words listed at: " << fileName << std::endl;
	char line[128];
	int i = 0;
	while (ifs.getline(line, sizeof(line), '\n'))
	{
		if (!ifs)
		{
			break;
		}
		if (line[0] == '#' || strlen(line) == 0)
		{
			continue;
		}
		i++;
		if (isCorrectWord(line))
		{
			correct_words++;
			if (contains(line))
			{
				unsigned occurs = getWordOccurrances(line);
				erase(line);
				resultant -= occurs;
			}
		}
		else
		{
			std::cout << "ERROR: incorrect entry " << "\"" << line << "\"" << " on line " << i << std::endl;
			incorrect_words++;
		}
	}
	ifs.close();
	std::cout << std::endl;
}

void Dictionary::loadTextFile(const char* fileName, int& correct_words, int& incorrect_words) const
{
	std::ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		throw std::invalid_argument("Invalid text file!");
	}
	std::cout << "Verifying the contents of: " << fileName << std::endl;
	std::string line;
	int i = 0;
	while (getline(ifs, line))
	{
		if (!ifs)
		{
			break;
		}
		i++;
		std::istringstream iss(line);
		std::string word;
		while (iss >> word)
		{
			if (!contains(word.c_str()))
			{
				std::cout << "SPELLING ERROR: " << "\"" << word << "\"" << " on line " << i << std::endl;
				incorrect_words++;
				continue;
			}
			correct_words++;
		}
	}
	ifs.close();
	std::cout << std::endl;
}

void Dictionary::showStats(int c_dict, int i_dict, int c_filter, int i_filter, int result, int c_text, int i_text, bool showTextStats) const
{
	std::cout << "Statistics:" << std::endl;
	std::cout << '\t' << "Dictionary entries: " << c_dict << " correct, " << i_dict << " incorrect" << std::endl;
	std::cout << '\t' << "Filter etries: " << c_filter << " correct, " << i_filter << " incorrect" << std::endl;
	std::cout << '\t' << "Resultant dictionary: " << result << std::endl;
	if (showTextStats == true)
	{
		std::cout << '\t' << "Words in text: " << c_text << " correct, " << i_text << " incorrect" << std::endl;
	}
}

void Dictionary::free()
{
	freeRec(root);
	count = 0;
}

void Dictionary::copyFrom(const Dictionary& other)
{
	copyFromRec(root, other.root);
	this->count = other.count;
}

void Dictionary::freeRec(TrieNode*& root)
{
	if (root == nullptr)
	{
		return;
	}
	for (size_t i = 0; i < ALPHABET_SIZE; i++)
	{
		freeRec(root->children[i]);
	}
	delete root;
	root = nullptr;
}

void Dictionary::copyFromRec(TrieNode*& root, TrieNode* otherRoot)
{
	if (otherRoot == nullptr)
	{
		return;
	}
	root = new TrieNode();
	root->terminal = otherRoot->terminal;
	root->occurrances = otherRoot->occurrances;
	for (size_t i = 0; i < ALPHABET_SIZE; i++)
	{
		copyFromRec(root->children[i], otherRoot->children[i]);
	}
}

bool Dictionary::eraseRec(TrieNode*& root, const char* word, unsigned depth)
{
	if (root == nullptr)
		return false;
	if (depth == strlen(word))
	{
		if (root->terminal == true)
		{
			root->terminal = false;
			root->occurrances = 0;
			count--;
			return !hasChildren(root); 
		}
		return false;
	}
	int index = tolower(word[depth]) - 'a';
	if (eraseRec(root->children[index], word, depth + 1))
	{
		delete root->children[index];
		root->children[index] = nullptr;
		return (!root->terminal && !hasChildren(root));
	}
	return false;
}
void Dictionary::collectWords(TrieNode* root, std::string& prefix, std::vector<std::string>& words) const
{
	if (root == nullptr)
		return;
	if (root->terminal == true)
		words.push_back(prefix);

	for (size_t i = 0; i < ALPHABET_SIZE; i++)
	{
		if (root->children[i] != nullptr)
		{
			prefix.push_back('a' + i);
			collectWords(root->children[i], prefix, words);
			prefix.pop_back();
		}
	}
}

bool Dictionary::hasChildren(const TrieNode* root) const
{
	for (size_t i = 0; i < ALPHABET_SIZE; i++)
	{
		if (root->children[i] != nullptr)
		{
			return true;
		}
	}
	return false;
}

unsigned Dictionary::getWordOccurrances(const char* word) const
{
	TrieNode* iter = root;
	char c;
	for (size_t i = 0; i < strlen(word); i++)
	{
		c = tolower(word[i]);
		if (iter->children[c - 'a'] == nullptr)
		{
			return 0;
		}
		iter = iter->children[c - 'a'];
	}
	if (iter->terminal == true)
	{
		return iter->occurrances;
	}
	return 0;
}

