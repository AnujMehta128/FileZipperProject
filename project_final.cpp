#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

// A Huffman tree node
struct Node {
    char ch;                // Character
    int freq;               // Frequency of the character
    Node* left;             // Left child
    Node* right;            // Right child

    // Constructor for the node
    Node(char character, int frequency) {
        ch = character;
        freq = frequency;
        left = right = nullptr;
    }
};

// Comparator to order the nodes based on frequency
struct compare {
    bool operator()(Node* left, Node* right) {
        return left->freq > right->freq;
    }
};

// Function to build the Huffman Tree
Node* buildHuffmanTree(unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, compare> pq;

    // Create a leaf node for each character and add it to the priority queue
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }

    // Combine nodes until only one node is left in the queue
    while (pq.size() != 1) {
        // Extract the two nodes of highest priority (lowest frequency)
        Node* left = pq.top();
        pq.pop();
        Node* right = pq.top();
        pq.pop();

        // Create a new internal node with a frequency equal to the sum of the two nodes' frequencies
        Node* newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;

        // Add the new node to the priority queue
        pq.push(newNode);
    }

    // The remaining node is the root of the Huffman Tree
    return pq.top();
}

// Function to encode characters and store the result in a map
void encode(Node* root, string str, unordered_map<char, string>& huffmanCode) {
    if (root == nullptr) return;

    // If it's a leaf node, store the character and its code
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    // Traverse the tree to generate codes for each character
    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

// Function to decode the encoded string using the Huffman Tree
string decode(Node* root, string encodedStr) {
    string decodedStr = "";
    Node* currentNode = root;
    for (char bit : encodedStr) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }

        // If we reach a leaf node, append the character to the result
        if (!currentNode->left && !currentNode->right) {
            decodedStr += currentNode->ch;
            currentNode = root;
        }
    }
    return decodedStr;
}

// Function to calculate the frequency of each character in the input string
unordered_map<char, int> calculateFrequency(const string& text) {
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }
    return freq;
}

// Compression function
string compress(const string& text, unordered_map<char, string>& huffmanCode) {
    string compressedStr = "";
    for (char ch : text) {
        compressedStr += huffmanCode[ch];
    }
    return compressedStr;
}

// Decompression function
string decompress(const string& compressedStr, Node* root) {
    return decode(root, compressedStr);
}

// Function to read input from a file
string readFile(const string& filename) {
    ifstream inFile(filename);
    string text((istreambuf_iterator<char>(inFile)), (istreambuf_iterator<char>()));
    inFile.close();
    return text;
}

// Function to write the compressed data to a file
void writeFile(const string& filename, const string& data) {
    ofstream outFile(filename);
    outFile << data;
    outFile.close();
}

// Utility to save the compressed file and Huffman tree structure
void saveCompressedFile(const string& compressedStr, const unordered_map<char, string>& huffmanCode) {
    ofstream outFile("compressed.txt");
    outFile << compressedStr << endl;

    // Saving the character codes as well for decompression
    for (auto pair : huffmanCode) {
        outFile << pair.first << ":" << pair.second << endl;
    }
    outFile.close();
}

// Utility to load the encoded file and Huffman tree structure (for simplicity)
string loadCompressedFile() {
    ifstream inFile("compressed.txt");
    string encodedStr;
    getline(inFile, encodedStr); // First line is the compressed data
    return encodedStr;
}

// Driver program
int main() {
    // Step 1: Read input from a file
    string inputFile = "input.txt";  // Replace with your input file name
    string text = readFile(inputFile);

    // Step 2: Calculate frequency of each character
    unordered_map<char, int> freq = calculateFrequency(text);

    // Step 3: Build Huffman Tree
    Node* root = buildHuffmanTree(freq);

    // Step 4: Generate Huffman Codes for each character
    unordered_map<char, string> huffmanCode;
    encode(root, "", huffmanCode);

    // Step 5: Compress the input text
    string compressedStr = compress(text, huffmanCode);

    // Step 6: Save the compressed file
    string outputFile = "compressed.bin";  // Output file for compressed data
    writeFile(outputFile, compressedStr);  // Write compressed data to a binary file

    // Save Huffman codes and encoded string for future decoding
    saveCompressedFile(compressedStr, huffmanCode);

    // Step 7: Decompress the file
    string compressedFileContent = loadCompressedFile();  // Load the compressed file
    string decompressedText = decompress(compressedFileContent, root);

    // Display the results
    cout << "Original text: " << text << endl;
    cout << "Compressed text: " << compressedStr << endl;
    cout << "Decompressed text: " << decompressedText << endl;

    return 0;
}
