//
// Created by Tomasz Kiljańczyk on 13-Mar-19.
//

#ifndef WIFI_TEST_WIFI_HPP
#define WIFI_TEST_WIFI_HPP

#include "main.h"
#include "UART.hpp"
#include "GPIO_Pin.hpp"

#include <string>
#include <vector>
#include <map>
#include "rapidjson/document.h"

enum CIPSTATUS_CODE {
    GOT_IP = 2,
    CONECTED = 3,
    DISCONNECTED = 4,
    CONNECTION_FAIL = 5
};

enum CWMODE {
    CWMODE_STATION = 1,
    CWMODE_AP = 2,
    CWMODE_AP_STATION = 3
};

class WiFi final {
    UART uart;

    std::map<std::string, std::string> configuredNetworks;
    std::vector<std::string> detectedNetworks;
    std::string connectedHost;

    void connectionStatus(const std::string &received);

    void parseFoundNetworks(const std::string &data);

public:
    explicit WiFi() = default;


    // uncategorized methods section
    bool init(UART_HandleTypeDef &_UART, bool connect);

    void setConfiguredNetworks(const rapidjson::Document &data);

    bool join();
    // end uncategorized methods section



    // AT command section
    bool restore();

    bool reset();

    bool disableEcho();

    bool enableEcho();

    bool checkConnection();

    bool test();

    bool setMode(const CWMODE &mode);

    bool setDHCP(const CWMODE &mode, const bool &state);

    bool checkNetworks();

    bool checkIP();

    bool checkVersion();

    bool join(const std::string &ssid, const std::string &passwd);

    bool quit();

    bool connect(const std::string &type, const std::string &address, const uint16_t &port);
    // end AT command section



    // transmision section
    bool transmitCommand(const std::string &cmd);

    bool transmitCommand(const std::string &cmd, std::string *result);

    bool sendHttpRequest(const std::string &req, const std::string &url, std::string &response);

    bool sendHttpRequest(const std::string &req, const std::string &url, const std::string &dataToSend,
                         std::string &response);

    bool send(const std::string &data, std::string &response);
    // end transmision section

};


#endif //WIFI_TEST_WIFI_HPP
