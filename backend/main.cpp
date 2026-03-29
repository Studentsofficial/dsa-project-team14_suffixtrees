#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

#include "log_loader.h"
#include "suffix_tree.h"
#include "analyzer.h"
#include "database.h"

using namespace std;

string extractFileContent(const string& request)
{
    size_t first = request.find("\r\n\r\n");
    if(first == string::npos) return "";
    first += 4;

    size_t second = request.find("\r\n\r\n", first);
    if(second == string::npos) return "";
    second += 4;

    size_t endBoundary = request.find("\r\n------", second);
    if(endBoundary == string::npos)
        endBoundary = request.size();

    return request.substr(second, endBoundary-second);
}

string readRequest(int client)
{
    string request;
    char buffer[4096];

    while(true)
    {
        ssize_t bytes = read(client, buffer, sizeof(buffer));
        if(bytes <= 0) break;

        request.append(buffer, bytes);

        if(request.find("\r\n\r\n") != string::npos)
        {
            size_t headerEnd = request.find("\r\n\r\n");
            string headers = request.substr(0, headerEnd);

            size_t cl = headers.find("Content-Length:");
            if(cl != string::npos)
            {
                size_t endLine = headers.find("\r\n", cl);
                string lenStr = headers.substr(cl+15, endLine-cl-15);
                int contentLength = stoi(lenStr);

                size_t bodyStart = headerEnd + 4;

                while(request.size() < bodyStart + contentLength)
                {
                    bytes = read(client, buffer, sizeof(buffer));
                    if(bytes <= 0) break;
                    request.append(buffer, bytes);
                }
            }

            break;
        }
    }

    return request;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 10);

    cout << "Server running at http://localhost:8080\n";

    while(true)
    {
        int client = accept(server_fd, nullptr, nullptr);

        string request = readRequest(client);

        // Upload log and analyze
        if(request.find("POST /upload") != string::npos)
        {
            string content = extractFileContent(request);

            ofstream temp("uploaded.txt");
            temp << content;
            temp.close();

            vector<LogEntry> logs = LogLoader::load("uploaded.txt");

            vector<string> raw;
            for(auto &l : logs)
                raw.push_back(l.raw);

            SuffixTree tree;
            tree.build(raw);

            Analyzer analyzer(logs, tree);
            AnalysisResult report = analyzer.run();

            string output = report.summary + "\n" + report.details;

            Database::storeReport(output);

            string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            + output;

            write(client, response.c_str(), response.size());
        }

        // fetch previous reports
        else if(request.find("GET /reports") != string::npos)
        {
            auto reports = Database::getReports();

            string out;

            for(auto &r : reports)
            {
                out += "========================\n";
                out += r + "\n\n";
            }

            string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n\r\n"
            + out;

            write(client, response.c_str(), response.size());
        }

        // serve css
        else if(request.find("GET /style.css") != string::npos)
        {
            ifstream file("../frontend/style.css");
            string css((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

            string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/css\r\n\r\n"
            + css;

            write(client, response.c_str(), response.size());
        }

        // serve frontend
        else
        {
            ifstream file("../frontend/index.html");
            string html((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

            string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n\r\n"
            + html;

            write(client, response.c_str(), response.size());
        }

        close(client);
    }

    return 0;
}