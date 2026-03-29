#include "suffix_tree.h"

using namespace std;

SuffixTree::SuffixTree() {
    root = new Node();
}

void SuffixTree::addId(Node* node, int id) {
    if (node->ids.empty() || node->ids.back() != id)
        node->ids.push_back(id);
}

void SuffixTree::build(const vector<string>& input) {
    logs = input;

    for (int id = 0; id < logs.size(); id++) {
        const string& s = logs[id];

        for (int i = 0; i < s.size(); i++) {
            Node* cur = root;

            for (int j = i; j < s.size(); j++) {
                char c = s[j];

                if (!cur->next[c])
                    cur->next[c] = new Node();

                cur = cur->next[c];
                addId(cur, id);
            }
        }
    }
}

vector<int> SuffixTree::search(const string& pattern) {
    Node* cur = root;

    for (char c : pattern) {
        if (!cur->next[c])
            return {};
        cur = cur->next[c];
    }

    return cur->ids;
}

const string& SuffixTree::get(int id) const {
    return logs[id];
}
