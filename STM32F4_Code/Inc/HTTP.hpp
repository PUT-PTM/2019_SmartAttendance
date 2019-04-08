//
// Created by aszcz on 22-Mar-19.
//

#ifndef WIFI_TEST_HTTP_H
#define WIFI_TEST_HTTP_H

#include <string>

namespace HTTP {

    std::string buildRequest(const std::string &host, const std::string &req, const std::string &url);
    std::string buildRequest(const std::string &host, const std::string &req, const std::string &url, const std::string& payload);

    struct Response {
        Response() = default;

        uint16_t statusCode = 0;
        std::string contentType;
        uint16_t contentLength = 0;
        std::string content;

        std::string toString();
    };

    Response parseResponse(const std::string& data);

};


#endif //WIFI_TEST_HTTP_H
