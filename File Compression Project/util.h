//  Project 5 - File compression
//  Jay Patel, Ashir Saleemi
//  Net ID : Jpate260, Asalee20
//  CS 251 data strcutures
//  file compression and decompression using encoding and decoding
#include "bitstream.h"
#include "hashmap.h"
#include <queue>
#include <utility>
#include <vector>
#include <string>
#pragma once



typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;  //  Going to the left node
    HuffmanNode* one;  //  Going to the right node
    int order;  //  Use to make sure objects are orderd
};

class prioritize {
	public:
	bool operator()(const pair<HuffmanNode*, int> &p1, const pair <HuffmanNode*, int> &p2)const {
	    if (p1.second == p2.second) {  //  if both equal
	        return p1.first->order > p2.first->order;  //  check for the order of insertion
	    }
		return p1.second > p2.second;  //  returns the priority_queue in order
	}
};


priority_queue <pair<HuffmanNode*, int>, vector< pair<HuffmanNode*, int>>, prioritize> pq;  //  making the priority_queue

  //  Freeing the tree using post-order recursive function
void freeTree(HuffmanNode* node) {
	if(node == nullptr) {
		return;
	}
	freeTree(node->zero);
	freeTree(node->one);
	delete node;
}

//
// This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
    if (isFile == true) {
    	ifbitstream inFile(filename);
	    char temp;
	    int freq = 1;
	    int i = 0;
	    while (inFile >> noskipws >> temp) {
	    	//  making sure were not skiping spaces
	    	if(map.containsKey(temp) == true) {
	    		//  if map has the key increase frequency
	    		i = map.get(temp);
	    		i++;
	    		map.put(temp, i);  //  Updating the keys frequency value
	    	} else {
	    		map.put(temp, freq);  //  Initailly puting the val into the map
	    	}
	    }
    } else {
	    char temp1;
	    int freq1 = 1;
	    int j = 0;
	    int a = filename.size();
    	for(int i = 0; i < a; i++) {
    		temp1 = filename[i];
			if (map.containsKey(temp1) == true) {
				// Iterating if the key exists
    			j = map.get(temp1);
    			j++;
    			map.put(temp1, j);
	    	} else {
	    		map.put(temp1, freq1);  //  Putting the key in the map
	    	}
	    }
    }
    map.put(PSEUDO_EOF, 1);  //  puting the EOF character into the map
}
//
// This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {
    int counter = 0;
    for (int key : map.keys()) {  //  making a new node from each map values
        counter++;  //  Using the counter for order
        HuffmanNode* n = new HuffmanNode();
        n->character = key;
        n->count = map.get(key);
        n->zero = nullptr;
        n->one = nullptr;
        n->order = counter;
        pq.push(make_pair(n, map.get(key)));
    }
    int temp;
    temp = counter;
    HuffmanNode* root;
    pair<HuffmanNode*, int> top = pq.top();  // Initial pair from top
    while (pq.size() != 1) {
    	temp++;
        int sumCount = 0;
        HuffmanNode* temp1 = nullptr;
        HuffmanNode* temp2 = nullptr;
        HuffmanNode* PNODE = new HuffmanNode();
        temp1 = top.first;  // First top
        sumCount += top.second;
        pq.pop();  // First pop
        top = pq.top();  // Getting the second top
        PNODE->zero = temp1;
        temp2 = top.first;
        sumCount += top.second;
        pq.pop();  // second pop
        top = pq.top();
        PNODE->one = temp2;
		PNODE->order = temp;
        PNODE->character = NOT_A_CHAR;
        PNODE->count = sumCount;
        pq.push(make_pair(PNODE, sumCount));  // pushing par into the tree
        root = PNODE;
    }
    return root;
}

//
// Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str, HuffmanNode* prev) {
	string str1 = "0";
	string str2 = "1";
	if (str.size() == 0) {
		prev = node;
	}
    if (node == nullptr) {  //  Base case
    	return;
    }
    if (node->zero != nullptr) {
    	_buildEncodingMap(node->zero, encodingMap, str+str1, prev);
    }
    if (node->one != nullptr) {
    	_buildEncodingMap(node->one, encodingMap, str+str2, prev);
    }
    if (node->character != NOT_A_CHAR) {
		encodingMap.emplace(node->character, str);
    }
	node = prev;  //  Updating the node
}

//
// This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
    HuffmanNode* prev = nullptr;
    string temp;
    _buildEncodingMap(tree, encodingMap, temp, prev);
    //  Call for resursive build
    return encodingMap;
}

//
// This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,
              int &size, bool makeFile) {
    string answer = "";
    stringstream ss;  //  Making a new stream
    ss << input.rdbuf();  //  pushing all the words in
    string line(ss.str());  //  Declaring the string variable
    for (int e : line) {
         if (encodingMap.find(e) != encodingMap.end()) {
            answer += encodingMap[e];  //  string addition to the map
        } else {
        	break;
        }
    }
    string eofChar = encodingMap[256];  //  adding EOF char
    answer += eofChar;
    int bitSize = answer.size();
    for (int i =0; i < bitSize; i++) {
        if (answer[i] == '1') {
            output.writeBit(1);
        } else {
            output.writeBit(0);
        }
    }
    size = answer.size();  // Done with size
    return answer;
}

//
// This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    string ret = "";
    HuffmanNode* nodeR = encodingTree;
    HuffmanNode* tNode;  //  iterator through the tree
    tNode = nodeR;
	int i;
	bool terminate = false;
    while (terminate != true) {
    	i = input.readBit();
    	if (i == -1) {
    		break;
    	}
    	if (tNode->zero != nullptr && i == 1) {
    		tNode = tNode->one;
    	} else if (tNode->zero != nullptr && i == 0) {
    		tNode = tNode->zero;
		} else {
			return "";
		}
    	if (tNode->zero == nullptr && tNode->one == nullptr) {
    		if (tNode->character == 256) {
    			break;
    		} else {
			ret += tNode->character;
			tNode = nodeR;
    		}
		 }
    }
    return ret;
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
	hashmapF frequencyMap;  //  Building map
    HuffmanNode* encodingTree = nullptr;  //  Initilizing the encodingTree
    hashmapE encodingMap;  //  Initilizing the encodingMap
    stringstream ss;
    string encoded;
    int size = 0;
    buildFrequencyMap(filename, true, frequencyMap);  //  Call to build map
    encodingTree = buildEncodingTree(frequencyMap);
    encodingMap = buildEncodingMap(encodingTree);
    ofbitstream output(filename + ".huf");  //  making the file
    ss << frequencyMap;
    output << frequencyMap;
    ifstream input(filename);
    encoded = encode(input, encodingMap, output, size, false);
    output.close();
    freeTree(encodingTree);  //  freeing the tree
    ss << encoded;
    return ss.str();  //  Returning the string compression
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
	size_t pos = filename.find(".huf");
	//  Using positions to make the filename
	if (pos >= 0) {
		filename = filename.substr(0, pos);
	}
	pos = filename.find(".");
	int x = filename.length();
	string temp;
	temp = filename.substr(pos, x - pos);
	filename = filename.substr(0, pos);
	ifbitstream input(filename + temp + ".huf");
	ofstream output(filename + "_unc" + temp);  //  Making the file
	hashmapF frequencyMap;
	HuffmanNode* encodingTree = nullptr;
	input >> frequencyMap;
	encodingTree = buildEncodingTree(frequencyMap);
	string decoded = decode(input, encodingTree, output);
    //  writing to decoded file
	output << decoded;
	output.close();  //  Closing the file for autograder
	freeTree(encodingTree);  //  Freeing the tree
    return  decoded;  //  Returning the message
}
