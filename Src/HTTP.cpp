//
// Created by aszcz on 22-Mar-19.
//

#include "HTTP.hpp"

std::string HTTP::buildRequest(const std::string &req, const std::string &url) {
    std::string request;

    request.append(req + " " + url + "\r\n");
    request.append("Access-Control-Allow-Origin: *\r\n");

    return request;
}
