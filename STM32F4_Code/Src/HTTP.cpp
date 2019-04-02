//
// Created by aszcz on 22-Mar-19.
//

#include <HTTP.hpp>

#include "HTTP.hpp"

using namespace std;

string HTTP::buildRequest(const string &host, const string &req, const string &url) {
    string request;

    request.append(req + " " + url + " HTTP/1.1\r\n");
    request.append("Host: " + host + "\r\n");

    return request;
}

string HTTP::Response::toString() {
    string result;
    result += "   Status code : " + to_string(this->statusCode) + "\r\n";
    result += "  Content type : " + this->contentType + "\r\n";
    result += "Content length : " + to_string(this->contentLength) + "\r\n";
    result += "       Content : " + this->content + "\r\n";
    return result;
}

HTTP::Response HTTP::parseResponse(const string &data) {
    HTTP::Response resp;

    string temp;

    bool statusCode = false;
    for (const auto &c : data) {
        if (!statusCode && c == ' ') {
            statusCode = true;
        }
        else if (statusCode && c == ' ') {
            resp.statusCode = (uint16_t) stoi(temp);
            temp.clear();
            temp.shrink_to_fit();
            break;
        }
        else if (statusCode) { temp.push_back(c); }
    }

    uint32_t foundPos = data.find("Content-Type: ");
    if (foundPos != string::npos) {
        foundPos += 14;
        do {
            if (data[foundPos] == ';' || data[foundPos] == '\r') {
                resp.contentType = temp;
                temp.clear();
                temp.shrink_to_fit();
                break;
            }
            else {
                temp.push_back(data[foundPos]);
            }
        } while (foundPos++);
    }

    foundPos = data.find("Content-Length: ");
    if (foundPos != string::npos) {
        foundPos += 16;
        do {
            if (data[foundPos] == '\r') {
                resp.contentLength = (uint16_t) stoi(temp);
                temp.clear();
                temp.shrink_to_fit();
                break;
            }
            else {
                temp.push_back(data[foundPos]);
            }
        } while (foundPos++);
    }

    foundPos = data.find("\r\n\r\n");
    if (foundPos != string::npos) {
        foundPos += 4;
        for ( /*foundPos*/ ; foundPos < data.length(); foundPos++) {
            temp.push_back(data[foundPos]);
        }
        resp.content = temp;
        temp.clear();
        temp.shrink_to_fit();
    }

    return resp;
}


