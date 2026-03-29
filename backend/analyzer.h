#ifndef ANALYZER_H
#define ANALYZER_H

#include <vector>
#include <string>
#include "log_loader.h"
#include "suffix_tree.h"

using namespace std;

struct AnalysisResult {
    string summary;
    string details;
};

class Analyzer {
private:
    vector<LogEntry>& logs;
    SuffixTree& tree;

public:
    Analyzer(vector<LogEntry>& l, SuffixTree& t);
    AnalysisResult run();
};

#endif