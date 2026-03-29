#include "log_loader.h"
#include <fstream>

using namespace std;

vector<LogEntry> LogLoader::load(const string& filename) {
    vector<LogEntry> logs;
    ifstream file(filename);
    string line;

   while (getline(file, line)) {

    // Remove carriage return if present
    if (!line.empty() && line.back() == '\r')
        line.pop_back();

    // Skip empty lines
    if (line.empty())
        continue;

    // Skip any line that does NOT start with '['
    if (line[0] != '[')
        continue;

    LogEntry entry;
    entry.raw = line;

    size_t start = line.find("[");
    size_t end = line.find("]");

    if (start != string::npos && end != string::npos && end > start) {
        entry.level = line.substr(start + 1, end - start - 1);
        entry.message = line.substr(end + 2);
    } else {
        entry.level = "UNKNOWN";
        entry.message = line;
    }

    logs.push_back(entry);
}

    return logs;
}