#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <fstream>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char character, int frequency) {
        ch = character;
        freq = frequency;
        left = right = nullptr;
    }
};

struct compare {
    bool operator()(Node* left, Node* right) {
        return left->freq > right->freq;
    }
};

Node* buildHuffmanTree(unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, compare> pq;
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }
    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* newNode = new Node('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }
    return pq.top();
}

void encode(Node* root, string str, unordered_map<char, string>& huffmanCode) {
    if (root == nullptr) return;
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }
    encode(root->left, str + "0", huffmanCode);
    encode(root->right, str + "1", huffmanCode);
}

string decode(Node* root, string encodedStr) {
    string decodedStr = "";
    Node* currentNode = root;
    for (char bit : encodedStr) {
        currentNode = (bit == '0') ? currentNode->left : currentNode->right;
        if (!currentNode->left && !currentNode->right) {
            decodedStr += currentNode->ch;
            currentNode = root;
        }
    }
    return decodedStr;
}

unordered_map<char, int> calculateFrequency(const string& text) {
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }
    return freq;
}

string compress(const string& text, unordered_map<char, string>& huffmanCode) {
    string compressedStr = "";
    for (char ch : text) {
        compressedStr += huffmanCode[ch];
    }
    return compressedStr;
}

string decompress(const string& compressedStr, Node* root) {
    return decode(root, compressedStr);
}

string readFile(const string& filename) {
    ifstream inFile(filename);
    string text((istreambuf_iterator<char>(inFile)), (istreambuf_iterator<char>()));
    inFile.close();
    return text;
}

void writeFile(const string& filename, const string& data) {
    ofstream outFile(filename);
    outFile << data;
    outFile.close();
}

void saveCompressedFile(const string& compressedStr, const unordered_map<char, string>& huffmanCode) {
    ofstream outFile("compressed.txt");
    outFile << compressedStr << endl;
    for (auto pair : huffmanCode) {
        outFile << pair.first << ":" << pair.second << endl;
    }
    outFile.close();
}

string loadCompressedFile() {
    ifstream inFile("compressed.txt");
    string encodedStr;
    getline(inFile, encodedStr);
    return encodedStr;
}

int main() {
    string inputFile = "input.txt";
    string text = readFile(inputFile);
    unordered_map<char, int> freq = calculateFrequency(text);
    Node* root = buildHuffmanTree(freq);
    unordered_map<char, string> huffmanCode;
    encode(root, "", huffmanCode);
    string compressedStr = compress(text, huffmanCode);
    string outputFile = "compressed.bin";
    writeFile(outputFile, compressedStr);
    saveCompressedFile(compressedStr, huffmanCode);
    string compressedFileContent = loadCompressedFile();
    string decompressedText = decompress(compressedFileContent, root);
    cout << "Original text: " << text << endl;
    cout << "Compressed text: " << compressedStr << endl;
    cout << "Decompressed text: " << decompressedText << endl;
    return 0;
}
