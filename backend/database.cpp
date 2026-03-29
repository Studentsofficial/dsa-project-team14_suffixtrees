#include "database.h"
#include <sqlite3.h>
#include <iostream>

using namespace std;

void Database::storeReport(const string& report) {

    sqlite3* db;

    if (sqlite3_open("log_analysis.db", &db)) {
        cerr << "Database open failed\n";
        return;
    }

    string sql =
        "INSERT INTO reports(report) VALUES(?);";

    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    sqlite3_bind_text(stmt,1,report.c_str(),-1,SQLITE_STATIC);

    sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

vector<string> Database::getReports() {

    vector<string> reports;

    sqlite3* db;

    sqlite3_open("log_analysis.db", &db);

    string sql = "SELECT report FROM reports ORDER BY id DESC;";

    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    while(sqlite3_step(stmt) == SQLITE_ROW){

        const unsigned char* text =
        sqlite3_column_text(stmt,0);

        reports.push_back((const char*)text);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return reports;
}