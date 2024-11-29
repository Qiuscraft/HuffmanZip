#ifndef GUARD_HuffmanNode_h
#define GUARD_HuffmanNode_h

#include <string>
#include <unordered_map>
#include "HuffmanCode.h"
#include <cstdint>

class HuffmanNode {
private:
	char data;
    uint64_t count;

	void initByCountArray(uint64_t *countArray);

	static void serializedStringToCountArray(const std::string &serialized_string, uint64_t *countArray);

public:
	HuffmanNode *leftChild, *rightChild;

	HuffmanNode();
	HuffmanNode(const HuffmanNode &other);
	~HuffmanNode();
	HuffmanNode & operator=(const HuffmanNode &other);

    HuffmanNode(char data, uint64_t count);
    HuffmanNode(char data, uint64_t count, HuffmanNode *leftChild, HuffmanNode *rightChild);
	HuffmanNode(const std::string &serialized_string);
	HuffmanNode(uint64_t *countArray);
	
    std::strong_ordering operator<=>(const HuffmanNode& other) const;
    bool operator==(const HuffmanNode& other) const;

	int treeHeight() const;
	uint64_t getCount() const;
	char getCharacter() const;
	bool isLeaf() const;

	std::string serialize() const;
	static std::string serialize(HuffmanNode *node);
	static HuffmanNode * deserialize(const std::string &serialized_string);

	static std::unordered_map<char, HuffmanCode> & writeCodeMap(
		const HuffmanNode *node, HuffmanCode code, std::unordered_map<char, HuffmanCode> &codeMap);
};

#endif
