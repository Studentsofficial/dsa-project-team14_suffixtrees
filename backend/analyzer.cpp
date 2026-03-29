#include "analyzer.h"
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

Analyzer::Analyzer(vector<LogEntry>& l, SuffixTree& t)
    : logs(l), tree(t) {}

AnalysisResult Analyzer::run() {

    unordered_map<string,int> levelCount;

    for (auto& l : logs)
        levelCount[l.level]++;

    stringstream summary;
    summary << "==== SUMMARY ====\n\n";
    summary << "Total Logs: " << logs.size() << "\n\n";
    summary << "Log Level Distribution:\n";

    for (auto& p : levelCount)
        summary << p.first << " : " << p.second << "\n";

    stringstream details;

    // =====================================================
    // 1️⃣ Proper Burst Detection (Grouped, No Duplicates)
    // =====================================================

    int i = 0;

    while (i < logs.size()) {

        int j = i + 1;
        string base = logs[i].message;

        while (j < logs.size()) {

            string next = logs[j].message;

            int common = 0;
            while (common < base.size() &&
                   common < next.size() &&
                   base[common] == next[common])
                common++;

            if (common >= 10)
                j++;
            else
                break;
        }

        if (j - i >= 3) {  // require at least 3 consecutive

            details << "\n----------------------------------\n";
            details << ">> Behavioral Burst Detected\n";
            details << "Pattern: \""
                    << base.substr(0, min(20,(int)base.size()))
                    << "...\"\n\n";

            for (int k = i; k < j; k++)
                details << logs[k].raw << "\n";
        }

        i = j;
    }

    // =====================================================
    // 2️⃣ Improved Structural Anomaly Detection
    // Only ERROR / CRITICAL levels
    // =====================================================

    for (auto& l : logs) {

        if (l.level != "ERROR" && l.level != "CRITICAL")
            continue;

        if (l.message.size() < 10)
            continue;

        int uniqueParts = 0;

        for (int k = 0; k < l.message.size() - 5; k += 5) {

            string part = l.message.substr(k, 5);
            auto ids = tree.search(part);

            if (ids.size() == 1)
                uniqueParts++;
        }

        if (uniqueParts >= 3) {

            details << "\n----------------------------------\n";
            details << ">> Structural Anomaly Detected\n";
            details << l.raw << "\n";
        }
    }

    AnalysisResult result;
    result.summary = summary.str();
    result.details = details.str();

    return result;
}