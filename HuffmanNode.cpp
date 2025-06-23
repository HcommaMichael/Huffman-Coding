//simple implementations of all node methods, both constructors as well as my personal isLeaf to simplify leaf checking.
//none of these are private as this class should only be accessible from files that include its header
//not sure if thats an issue, but I assume its ok.
#include "HuffmanNode.h"

HuffmanNode::HuffmanNode(int d, int freq)
    :data(d), frequency(freq), left(nullptr), right(nullptr){

}
HuffmanNode::HuffmanNode(int d, int freq, HuffmanNode* left, HuffmanNode* right)
    :data(d), frequency(freq), left(left), right(right){

}
int HuffmanNode::getData() const{
    return data;
}
int HuffmanNode::getFrequency() const{
    return frequency;
}
HuffmanNode* HuffmanNode::getLeft(){
    return left;
}
HuffmanNode* HuffmanNode::getRight(){
    return right;
}
HuffmanNode*& HuffmanNode::getLeftRef(){
    return left;
}
HuffmanNode*& HuffmanNode::getRightRef(){
    return right;
}
void HuffmanNode::setLeft(HuffmanNode* l){
    left = l;
}
void HuffmanNode::setRight(HuffmanNode* r){
    right = r;
}
bool HuffmanNode::isLeaf() const{
    return left == nullptr && right == nullptr;
}
