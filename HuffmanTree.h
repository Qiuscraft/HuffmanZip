#ifndef GUARD_HuffmanTree_h
#define GUARD_HuffmanTree_h

#include "HuffmanNode.h"

class HuffmanTree {
private:
	HuffmanNode *root;

public:
	HuffmanTree();
	HuffmanTree(const HuffmanTree &other);
	~HuffmanTree();
	HuffmanTree & operator=(const HuffmanTree &other);
	
	HuffmanTree(HuffmanNode *root);
	HuffmanTree(const std::string &serialized_string);
	HuffmanTree(uint64_t *countArray);
	
	std::string serialize() const;
	static HuffmanTree * deserialize(const std::string &serialized_string);

	std::unordered_map<char, HuffmanCode> getCodeMap() const;
	int getHeight() const;
	HuffmanNode * getRoot() const;
	bool equals(const HuffmanTree &other) const;
};

#endif
