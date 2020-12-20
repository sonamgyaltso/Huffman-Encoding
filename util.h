#include "hashmap.h"
#include "bitstream.h"
#pragma once
#include <queue>
#include <string>
typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;
struct HuffmanNode {
    int character;
    int count;
    int order;
    HuffmanNode* zero;
    HuffmanNode* one;
  bool operator< (const HuffmanNode & p) const {
        if (count == p.count) {
            return order > p.order;
        } else {
            return count > p.count;
        }
    }
};
// This method frees the memory allocated for the Huffman tree.
//
// ---------------------------------------------------------------
void freeTree(HuffmanNode* node) {
    // TO DO:  Write this function.
}
//
// This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
//
// ---------------------------------------------------------------
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
    // TO DO:  Write this function.
    // step 1 read file
    char c;
    int count;
    if (isFile == false) {
        for (int i = 0; i < filename.length(); i++) {
            if (map.containsKey(filename[i])) {
                count = map.get(filename[i]) + 1;
                map.put(filename[i], count);
            } else {
                map.put(filename[i], 1);
            }
        }
        map.put(PSEUDO_EOF, 1);
    } else {
        ifstream file;
        file.open(filename);
        string temp;
        while (file.get(c)) {
            if (map.containsKey(c)) {
                count = map.get(c) + 1;
                map.put(c, count);
            } else {
                map.put(c, 1);
            }
        }
        map.put(PSEUDO_EOF, 1);
    }
}
//
// This function builds an encoding tree from the frequency map.
// ---------------------------------------------------------------
HuffmanNode* buildEncodingTree(hashmapF &map) {
    // TO DO:  Write this function.
    // make a loop that goes thru the entire map
    int curOrder = 1;
    HuffmanNode *curNode;
    HuffmanNode *leftNode;
    HuffmanNode *rightNode;
    priority_queue<HuffmanNode> pq;
    // varibles to build tree
    for (int key : map.keys()) {
    	curNode = new HuffmanNode;
    	curNode -> character = key;
    	curNode -> count = map.get(key);
    	curNode -> order = curOrder;
    	curNode -> zero = nullptr;
    	curNode -> one = nullptr;
    	pq.push(*curNode);
    	curOrder = curOrder + 1;
    }
    while (pq.size() != 1) {
    	leftNode = new HuffmanNode;
    	rightNode = new HuffmanNode;
    	*leftNode = (pq.top());
    	pq.pop();
    	*rightNode = (pq.top());
    	pq.pop();
    	curNode = new HuffmanNode;
    	curNode -> zero = leftNode;
    	curNode -> one = rightNode;
    	curNode -> count = leftNode -> count + rightNode -> count;
    	curNode -> order = curOrder;
    	curNode -> character = NOT_A_CHAR;
    	pq.push(*curNode);
    	curOrder = curOrder + 1;
    }
    return curNode;  // TO DO: update this return
}
// ---------------------------------------------------------------
void _buildEncodingMap(HuffmanNode* root, int index,
hashmapE &encodingMap, string sa) {
    // Assign 0 to left edge and recur
    if (root -> zero) {
    _buildEncodingMap(root->zero, index + 1,
    encodingMap, sa.insert(index, 1, '0'));
    }
    // Assign 1 to right edge and recur
    if (root -> one) {
    _buildEncodingMap(root->one, index+ 1,
    encodingMap, sa.insert(index, 1, '1'));
    }
    if (root -> zero == nullptr && root -> one == nullptr) {
        string tmp;
        for (int i = 0; i < index; ++i) {
            tmp += sa[i];
            }
        encodingMap[root -> character] = tmp;
    }
}
// ---------------------------------------------------------------
hashmapE buildEncodingMap(HuffmanNode* tree) {
    HuffmanNode* cur = tree;
    hashmapE encodingMap;
    int index = 0; 
    string s = "";
    _buildEncodingMap(cur, index, encodingMap, s);
    return encodingMap;
}
//
// This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap,
ofbitstream& output, int &size, bool makeFile) {
    string idk;
    while (!input.eof()) {
        char tmp;
        tmp = char(input.get());
        if (encodingMap.find(tmp) != encodingMap.end()) {
                idk += encodingMap.find(tmp) -> second;
        } else {
            idk += encodingMap.find(256) -> second;
        }
     }
    size = idk.size();
    for (int i = 0; i< idk.size(); i++) {
        if (idk[i] == '1') {
             output.writeBit(1);
        } else {
             output.writeBit(0);
        }
    }
     // TO DO:  Write this function.
    return idk;
}
//
// This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
// ---------------------------------------------------------------
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    // plan
    // put everything into a string or if we go by char by char
    // i could check if the char is '0' or 1
    // if it is those 2 move left if 0 move right if 1
    // now if theres nothing at left and right that means we hi
    // it so we put that in a string and then curr is gu
    // encoding treeto return to originl spot
    string idk;
    HuffmanNode* curr = encodingTree;
    while (!input.eof()) {
        if (input.readBit() == 0) {
            curr = curr -> zero;
        } else {
            curr = curr -> one;
        }
        if (curr -> zero == nullptr && curr -> one == nullptr) {
            if (curr -> character != 256) {
                idk += char(curr -> character);
            }
            curr = encodingTree;
        }
    }
    if (idk.at(idk.length()-1) != '\n') {
        idk.erase(idk.length()-1);
    }
    return idk;
}
//
// This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    // TO DO:  Write this function.
    return "";
}
//
// This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    // TO DO:  Write this function.
    return "";
}
