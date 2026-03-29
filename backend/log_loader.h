#ifndef LOG_LOADER_H
#define LOG_LOADER_H

#include <vector>
#include <string>

using namespace std;

struct LogEntry {
    string level;
    string message;
    string raw;
};

class LogLoader {
public:
    static vector<LogEntry> load(const string& filename);
};

#endif