#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

class Database {

public:

    static void storeReport(const std::string& report);

    static std::vector<std::string> getReports();

};

#endif