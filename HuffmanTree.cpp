#include "HuffmanTree.h"
#include <queue>
#include <vector>

HuffmanTree::HuffmanTree(): root(nullptr) {}

HuffmanTree::HuffmanTree(const HuffmanTree &other) {
	if (other.root == nullptr)
		root = nullptr;
	else root = new HuffmanNode(*other.root);
}

HuffmanTree::~HuffmanTree() {
	delete root;
}

HuffmanTree & HuffmanTree::operator=(const HuffmanTree &other) {
	if (&other != this) {
		delete root;
		if (other.root == nullptr)
			root = nullptr;
		else root = new HuffmanNode(*other.root);
	}
	return *this;
}

HuffmanTree::HuffmanTree(HuffmanNode *root): root(root) {}

HuffmanTree::HuffmanTree(const std::string &serialized_string) {
	root = HuffmanNode::deserialize(serialized_string);
}

HuffmanTree::HuffmanTree(uint64_t *countArray) {
	root = new HuffmanNode(countArray);
}

std::string HuffmanTree::serialize() const {
	return HuffmanNode::serialize(root);
}

HuffmanTree * HuffmanTree::deserialize(const std::string &serialized_string) {
	return new HuffmanTree(HuffmanNode::deserialize(serialized_string));
}

std::unordered_map<char, HuffmanCode> HuffmanTree::getCodeMap() const {
	std::unordered_map<char, HuffmanCode> codeMap;
	HuffmanNode::writeCodeMap(root, HuffmanCode(), codeMap);
	return codeMap;
}

int HuffmanTree::getHeight() const {
	if (root == nullptr) return 0;
	else return root->treeHeight();
}

HuffmanNode * HuffmanTree::getRoot() const {
	return root;
}

bool HuffmanTree::equals(const HuffmanTree &other) const {
	std::queue<HuffmanNode *> nodeQueue;
	std::vector<HuffmanNode *> vec1, vec2;

	nodeQueue.push(root);
	while (!nodeQueue.empty()) {
		HuffmanNode *currentNode = nodeQueue.front();
		nodeQueue.pop();

		if (currentNode == nullptr)
			continue;

		nodeQueue.push(currentNode->leftChild);
		nodeQueue.push(currentNode->rightChild);

		vec1.push_back(currentNode);
	}

	nodeQueue.push(other.root);
	while (!nodeQueue.empty()) {
		HuffmanNode *currentNode = nodeQueue.front();
		nodeQueue.pop();

		if (currentNode == nullptr)
			continue;

		nodeQueue.push(currentNode->leftChild);
		nodeQueue.push(currentNode->rightChild);

		vec2.push_back(currentNode);
	}

	if (vec1.size() != vec2.size())
		return false;
	for (size_t i = 0; i < vec1.size(); i++) {
		if (vec1[i]->getCharacter() != vec2[i]->getCharacter())
			return false;
		if (vec1[i]->getCount() != vec2[i]->getCount())
			return false;
		if (vec1[i]->leftChild != vec2[i]->leftChild)
			return false;
		if (vec1[i]->rightChild != vec2[i]->rightChild)
			return false;
	}
	return true;
}

std::ostream& operator<<(std::ostream& os, const HuffmanTree& tree) {
	for (const auto& pair : tree.getCodeMap()) {
        os << pair.first << ": " << pair.second << '\n';
    }
	return os;
}
