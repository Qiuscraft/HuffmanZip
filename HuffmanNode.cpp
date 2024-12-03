#include "HuffmanNode.h"
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <queue>
#include <string.h>

HuffmanNode::HuffmanNode():leftChild(nullptr), rightChild(nullptr), data('\0'), count(0) {}

HuffmanNode::HuffmanNode(const HuffmanNode &other): data(other.data), count(other.count) {
	if (other.leftChild != nullptr)
		leftChild = new HuffmanNode(*other.leftChild);
	if (other.rightChild != nullptr)
		rightChild = new HuffmanNode(*other.rightChild);
}

HuffmanNode::~HuffmanNode() {
	delete leftChild, rightChild;
}

HuffmanNode & HuffmanNode::operator=(const HuffmanNode &other) {
	if (&other != this) {
		delete leftChild, rightChild;
		data = other.data;
		count = other.count;
		if (other.leftChild != nullptr)
			leftChild = new HuffmanNode(*other.leftChild);
		if (other.rightChild != nullptr)
			rightChild = new HuffmanNode(*other.rightChild);
	}
	return *this;
}

HuffmanNode::HuffmanNode(char data, uint64_t count): 
	data(data), leftChild(nullptr), rightChild(nullptr), count(count) {}

HuffmanNode::HuffmanNode(
	char data, uint64_t count, HuffmanNode *leftChild, HuffmanNode *rightChild): 
	data(data), count(count), leftChild(leftChild), rightChild(rightChild) {}

HuffmanNode::HuffmanNode(const std::string &serialized_string) {
	uint64_t countArray[256];
	serializedStringToCountArray(serialized_string, countArray);
	initByCountArray(countArray);
}

HuffmanNode::HuffmanNode(uint64_t *countArray) {
	initByCountArray(countArray);
}

std::strong_ordering HuffmanNode::operator<=>(const HuffmanNode& other) const {
    return count <=> other.count;
}

bool HuffmanNode::operator==(const HuffmanNode& other) const {
    return count == other.count;
}

int HuffmanNode::treeHeight() const {
	if (leftChild == nullptr) {
		if (rightChild == nullptr) return 0;
		else return rightChild->treeHeight() + 1;
	} else {
		if (rightChild == nullptr) return leftChild->treeHeight() + 1;
		else return __max(leftChild->treeHeight() + 1, rightChild->treeHeight() + 1);
	}
}

uint64_t HuffmanNode::getCount() const {
	return count;
}

char HuffmanNode::getCharacter() const {
	return data;
}

bool HuffmanNode::isLeaf() const {
	return leftChild == nullptr && rightChild == nullptr;
}

std::string HuffmanNode::serialize() const {
	return serialize(this);
}

std::string HuffmanNode::serialize(const HuffmanNode *node) {
	if (node == nullptr)
		return std::string();
	else {
		std::string result;

		std::queue<const HuffmanNode *> nodeQueue;
		nodeQueue.push(node);

		while (!nodeQueue.empty()) {
			const HuffmanNode *currentNode = nodeQueue.front();
			nodeQueue.pop();

			if (currentNode == nullptr)
				continue;

			nodeQueue.push(currentNode->leftChild);
			nodeQueue.push(currentNode->rightChild);

			if (currentNode->isLeaf()) {
				result.push_back(currentNode->data);
				// 转大端序
				uint64_t big_endian_value = 
					((currentNode->count & 0x00000000000000FF) << 56) | 
					((currentNode->count & 0x000000000000FF00) << 40) |
					((currentNode->count & 0x0000000000FF0000) << 24) |
					((currentNode->count & 0x00000000FF000000) << 8)  |
					((currentNode->count & 0x000000FF00000000) >> 8)  |
					((currentNode->count & 0x0000FF0000000000) >> 24) |
					((currentNode->count & 0x00FF000000000000) >> 40) |
					((currentNode->count & 0xFF00000000000000) >> 56);
				result.append(reinterpret_cast<const char*>(&big_endian_value), sizeof(big_endian_value));
			}
		}
	}
}

HuffmanNode * HuffmanNode::deserialize(const std::string &serialized_string) {
	if (serialized_string.empty())
		return nullptr;
	uint64_t countArray[256];
	serializedStringToCountArray(serialized_string, countArray);
	HuffmanNode *result = new HuffmanNode(countArray);
	return result;
}

std::unordered_map<char, HuffmanCode> & HuffmanNode::writeCodeMap(
	const HuffmanNode *node, HuffmanCode code, std::unordered_map<char, HuffmanCode> &codeMap) {
	if (node == nullptr)
		return codeMap;
	
	if (node->isLeaf())
		codeMap[node->data] = code;

	else {
		writeCodeMap(node->leftChild, code.append(0), codeMap);
		writeCodeMap(node->rightChild, code.append(1), codeMap);
	}

	return codeMap;
}

void HuffmanNode::initByCountArray(uint64_t *countArray) {
	struct compare {
		bool operator()(HuffmanNode *a, HuffmanNode *b) {
			return *a > *b;
		}
	};
	std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, compare> pri_queue;
	for (size_t i = 0; i < 256; i++) {
		if (countArray[i] == 0)
			continue;
		HuffmanNode *node = new HuffmanNode(i, countArray[i]);
		pri_queue.push(node);
	}
	
	while (pri_queue.size() > 2) {
		HuffmanNode *node1 = pri_queue.top();
		pri_queue.pop();
		HuffmanNode *node2 = pri_queue.top();
		pri_queue.pop();
		HuffmanNode *parent = new HuffmanNode('\0', node1->getCount() + node2->getCount());
		parent->leftChild = node1;
		parent->rightChild = node2;
		pri_queue.push(parent);
	}
	
	if (!pri_queue.empty()) {
		HuffmanNode *node1 = pri_queue.top();
		pri_queue.pop();
		HuffmanNode *node2 = pri_queue.top();
		pri_queue.pop();
		data = '\0';
		count = node1->getCount() + node2->getCount();
		leftChild = node1;
		rightChild = node2;
	} else {
		throw std::domain_error("Initing a NULL HuffmanNode.");
	}
}

void HuffmanNode::serializedStringToCountArray(const std::string &serialized_string, uint64_t *countArray) {
    memset(countArray, 0, sizeof(uint64_t) * 256);

    if (serialized_string.size() % 9 != 0)
        throw std::domain_error("Incorrect serialized_string.size() when deserializing a HuffmanNode.");
    
    for (size_t i = 0; i < serialized_string.size() / 9; i++) {
        uint64_t amount = 0;
        for (size_t j = 0; j < 8; ++j) {
            amount |= 
            (static_cast<uint64_t>(static_cast<unsigned char>(serialized_string[i * 9 + 1 + j]))
                 << (8 * (7 - j)));
        }
        countArray[static_cast<unsigned char>(serialized_string[i * 9])] += amount;
    }
}

HuffmanNode * HuffmanNode::generateByCountArray(uint64_t *countArray) {
	for (int i = 0; i < 256; i++) {
		if (countArray[i] != 0) {
			HuffmanNode *result = new HuffmanNode(countArray);
			return result;
		}
	}
	return nullptr;
}
