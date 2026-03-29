#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

struct Node {
    unordered_map<char, Node*> next;
    vector<int> ids;
};

class SuffixTree {
    Node* root;
    vector<string> logs;

    void addId(Node* node, int id);

public:
    SuffixTree();
    void build(const vector<string>& input);
    vector<int> search(const string& pattern);
    const string& get(int id) const;
};

#endif