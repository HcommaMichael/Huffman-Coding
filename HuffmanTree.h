#include <map>
#include <string>
#include <queue>
#include "BitStream.h"
#include "HuffmanNode.h"

#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H
class HuffmanTree{
public:
    HuffmanTree(map<char, int> counts);
    HuffmanTree(istream* in);
    map<char, string> createEncodings();
    void compress(ifstream* input, OBitStream* output);
    void decompress(IBitStream* input, OBitStream* output);
private:
    const int MY_EOF = 256;
    const int EOF_KEY = -1;
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Comparator> pq;
    HuffmanNode* huffmanTree;
    map<int, string> intCodes;
    void encodeHelper(string code, HuffmanNode* node);
    void huffHelper(int c, string code, int index, HuffmanNode*& root);
    void decompressHelper(HuffmanNode* node, IBitStream* input, OBitStream* output);
};
#endif
