//node class that handles node struct and also contains the comparator for my priority queue.
#ifndef HUFF_NODE_H
#define HUFF_NODE_H

//class is ASCII based, so data is an int that accepts characters as ASCII
//but is also able to handle the 256 EOF and the 257 garbage data of a merged node, this is
//important in my comparator.
class HuffmanNode{
public:
    HuffmanNode(int d, int freq);
    HuffmanNode(int d, int freq, HuffmanNode* left, HuffmanNode* right);
    int getData() const;
    int getFrequency() const;
    HuffmanNode* getLeft();
    HuffmanNode* getRight();
    HuffmanNode*& getLeftRef();
    HuffmanNode*& getRightRef();
    void setLeft(HuffmanNode* l);
    void setRight(HuffmanNode* r);
    bool isLeaf() const;
private:
    int data;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;
};

//comparator at base just assigns priority by frequency, but if the frequencies match
//then the ASCII value is used to break the tie. As mentioned EOF has a value of 256
//meaning it will always lose to real characters, but not to merged nodes if thats even a concern.
//Merged nodes have value 257, meaning they lose all ties and will always be pulled second, making them
//constant right children when up against real characters.
struct Comparator{
    bool operator()(HuffmanNode* n1, HuffmanNode* n2) const{
        if(n1->getFrequency() == n2->getFrequency()){
            return n1->getData() > n2->getData();
        }
        return n1->getFrequency() > n2->getFrequency();
    }
};
#endif
