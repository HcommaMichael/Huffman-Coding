//HuffmanTree class: Handles all the compression and decompression logic.
#include "HuffmanTree.h"

//constructor for file compression. loops through the priority queue to create tree nodes, placing them
//back in the queue once created/merged untill only one, the overall root, remains.
HuffmanTree::HuffmanTree(map<char, int>counts){
    if(counts.empty()){
        throw "Character counts is empty.";
    }
    //pushes place holder EOF as 256 with frequency 1 since it is guaranteed to be in a file, ven if file is empty.
    //256 was chosen because the spec has it as that, but also because this makes EOF lose comparison to all other
    //nodes with frequency 1, meaning out of a pair it will always be the right child.
    pq.push(new HuffmanNode(MY_EOF, 1));
    for(const auto kv : counts){
        pq.push(new HuffmanNode(kv.first, kv.second));
    }
    while(pq.size() > 1){
        HuffmanNode* first = pq.top();
        pq.pop();
        HuffmanNode* second = pq.top();
        pq.pop();

        //merged nodes are created with a value of 257 for the same reason that EOF is value 256, they should not be placed at a higher priority
        //than non merged nodes, therefore making them right only nodes when compared with characters of the same frequency.
        pq.push(new HuffmanNode(257, (first->getFrequency() + second->getFrequency()), first, second));
    }
    huffmanTree = pq.top();
    pq.pop();
}
//decompression constructor, requires a helper method for recursive tree building.
HuffmanTree::HuffmanTree(istream* in){
    if(in == nullptr){
        throw "Passed in file is not valid.";
    }

    string key, code;

    //loops through the inputted code file pulling first the character key
    //then after a character check, the code, recurs to build the tree for
    //each key/code pair in the file.
    while(getline(*in, key)){
        int ch;

        //check to see if the key is the EOF key, if so set the character to MY_EOF
        //and use that as the insert into the node when the path ends.
        if(key == "-1"){
            ch = MY_EOF;
        }
        //otherwise let the character be the ASCII value of the read string.
        else{
            ch = stoi(key);
        }
        //get the code for the character, call the helper to trace the path to the leaf node where it belongs.
        getline(*in, code);
        huffHelper(ch, code, 0, huffmanTree);
        //once this while loop makes all the recursive calls and exits, tree will contain all necessary paths for decompression.
    }

}
//afformentioned helper that takes the characters ASCII, its code, an index and a tree node.
//I did consider using IBit here instead of index, until I realized that IBit did not support reading from a string.
void HuffmanTree::huffHelper(int c, string code, int index, HuffmanNode*& root){
    //base case: if the index is one past the maximum index, the final node has been reached
    //insert the character node there. Since each character has a unique path there is no need
    //to make sure the node at placement is nullptr, its guaranteed to be.
    if(index == code.length()){
        root = new HuffmanNode(c, -1);
    }
    else{
        //if path is still being traversed, first check if the current node is nullptr, if so insert a dummy
        //merged node with a garbage frequency since that part is not necessary when decoding from a file, codes
        //were already assigned with frequency considerations.
        if(root == nullptr){
            root = new HuffmanNode(257, -1);
        }
        //next check traversal direction based off the character at the specified index
        //left for 0, right for 1, continue this recursion for the entire string.
        if(code[index] == '0'){
            huffHelper(c, code, index + 1, root->getLeftRef());
        }
        else{
            huffHelper(c, code, index + 1, root->getRightRef());
        }
    }
}
//creates the encodings, string exception if the tree ends up null
map<char, string> HuffmanTree::createEncodings(){
    if(huffmanTree == nullptr){
        throw "Cannot create encodings from an empty tree.";
    }
    //this is the returned map, but I store a private member map as well that handles the characters by ASCII
    //since the EOF encoding will not work in a char map which will corrupt the return
    map<char, string>codes;

    //helper explained below
    encodeHelper("", huffmanTree);

    //for loop places all valid characters from the ASCII map into the returned char map
    for(const auto kv : intCodes){
        if(kv.first != MY_EOF){
            codes[kv.first] = kv.second;
        }
        else{
            //pre cast because I don't know.
            codes[(char)EOF_KEY] = intCodes[MY_EOF];
        }
    }

    return codes;
}
//afformentioned encoding helper, handles all recursive traversal of the tree.
void HuffmanTree::encodeHelper(string code, HuffmanNode* node){
    //again catches the nullptr possibility.
    if(node != nullptr){
        //when a leaf is reached the character held by that node now has a code that can be placed in the map.
        if(node->isLeaf()){
            intCodes[node->getData()] = code;
        }
        //otherwise recur down all subtrees, adding 1's and 0's to the code until base case is reached.
        else{
            encodeHelper(code + "0", node->getLeft());
            encodeHelper(code + "1", node->getRight());
        }
    }
}
//compress writes from the codes in the stored map to the file in bit form.
void HuffmanTree::compress(ifstream* input, OBitStream* output){
    //catch for if the pointers are null
    if(input == nullptr || output == nullptr){
        throw "One or more of the passed in files are not valid.";
    }
    char ch;
    //reads input file character by character, creates a string from that characters code,
    //write the code bit by bit to the file.
    while(input->get(ch)){
        string charCode = intCodes[ch];
        output->writeBits(charCode);
    }

    //manually writes the EOF marker into the end of the file, then close the output.
    output->writeBits(intCodes[MY_EOF]);
    output->close();
}
//decompress was orginally recursive (see recursive implementation below) which worked fine
//for small files, but very quickly hit the recursive call stack limit when used on larger files like hamlet.
//if you would like to test that comment out the iterative part and uncomment the recursive call.
void HuffmanTree::decompress(IBitStream* input, OBitStream* output){
    //nullptr catch
    if(input == nullptr || output == nullptr){
        throw "One or more of the passed in files are not valid.";
    }
    //saftey for single character files, or non null empty files, just catches logical or symbolic EOF
    //otherwise writes the same character until an EOF condition is met.
    if(huffmanTree->getLeft() == nullptr || huffmanTree->getRight() == nullptr){
        while(true){
            int bit = input->readBit();
            if(bit == -1){
                output->close();
                return;
            }
            if(huffmanTree->getData() == MY_EOF){
                output->close();
                return;
            }
            output->write((char)huffmanTree->getData());
        }
    }

    HuffmanNode* curr = huffmanTree;
    bool end = false;
    //loop through the file bit by bit, traverse the tree, check if the new node is a nullptr
    //if it is write that character (unless its an EOF) and reset curr to the root
    while(!end){
        int bit = input->readBit();
        if(bit == -1){
            return;
        }

        if(bit == 0){
            curr = curr->getLeft();
        }
        else{
            curr = curr->getRight();
        }

        if(curr->isLeaf()){
            if(curr->getData() == MY_EOF){
                end = true;
            }
            else{
                output->write((char)curr->getData());
                curr = huffmanTree;
            }
        }
    }
    //decompressHelper(huffmanTree, input, output);

    //close the file
    output->close();
}
//old recursive implementation, as mentioned worked for small files, but couldn't handle the size of large files like hamlet.
//works by reading through the bits recursively and deciding where to go based off of that, but instead of
//a base case that returns through the entire stack, it starts a new call stack back from the root
void HuffmanTree::decompressHelper(HuffmanNode* node, IBitStream* input, OBitStream* output){
    if(node->isLeaf()){
        if(node->getData() == MY_EOF){
            return;
        }
        output->write((char)node->getData());
        decompressHelper(huffmanTree, input, output);
        return;
    }
    else{
        int bit = input->readBit();
        if(bit == -1){
            return;
        }
        if(bit == 0){
            decompressHelper(node->getLeft(), input, output);
        }
        else{
            decompressHelper(node->getRight(), input, output);
        }
    }
}
