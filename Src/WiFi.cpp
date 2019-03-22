//
// Created by Tomasz Kiljańczyk on 13-Mar-19.
//

#include <WiFi.hpp>

#include "WiFi.hpp"

#include "GPIO_Pin.hpp"
#include "USB_Serial.hpp"
#include "HTTP.hpp"

using namespace std;


bool WiFi::init(UART_HandleTypeDef &_uart, bool connect) {
    this->uart = UART(&_uart);

    if (!this->restart()) { return false; }
    HAL_Delay(100);
    if (!this->disableEcho()) { return false; }
    if (!this->test()) { return false; }

    if (connect) {
        if (!this->setMode(CWMODE_STATION)) { return false; }
        if (!this->quit()) { return false; }
        if (!this->checkNetworks()) { return false; }
        HAL_Delay(100);

        if (this->join()) {
            if (this->checkConnection()) {
                this->checkIP();
                this->transmitCommand("AT+CIPMUX=0");
            }
        }
        USB_Serial::transmit("WiFi init end\r\n\r\n");
    }
    return true;
}


void WiFi::setConfiguredNetworks(const rapidjson::Document &data) {
    USB_Serial::transmit("WiFi configuring networks\r\n");
    for (const auto &elem : data["NETWORKS"].GetArray()) {
        const string ssid = elem["SSID"].GetString();
        const string passwd = elem["PASSWD"].GetString();
        USB_Serial::transmit(ssid + "\r\n");
        this->configuredNetworks[ssid] = passwd;
    }
    USB_Serial::transmit("\r\n");
}


bool WiFi::sendCommand(const string &cmd, string *result) {
    USB_Serial::transmit("WiFi command start -----------------------------\r\n");
    USB_Serial::transmit("Command: " + cmd + "\r\n");

    uart.discardAll();

    string temp;
    while (true) {
        if (temp.find("busy p") == string::npos) {
            if (!uart.transmit(cmd + "\r\n")) {
                USB_Serial::transmit("WiFi command fail ------------------------------\r\n\r\n");
                return false;
            }
        }
        uart.receiveAll(temp);
        if (temp.find("ready") != string::npos || temp.find("OK") != string::npos) {
            break;
        }
        if (temp.find("ERROR") != string::npos || temp.find("FAIL") != string::npos) {
            break;
        }
    }

    USB_Serial::transmitLines(temp);

    if (result != nullptr) { *result = temp; }

    if (temp.find("OK") != string::npos || temp.find("ready") != string::npos) {
        USB_Serial::transmit("WiFi command success ---------------------------\r\n\r\n");
        return true;
    }
    else {
        USB_Serial::transmit("WiFi command fail ------------------------------\r\n\r\n");
        return false;
    }
}

bool WiFi::transmitCommand(const string &cmd) {
    return this->sendCommand(cmd, nullptr);
}


bool WiFi::restart() {
    USB_Serial::transmit("WiFi restart\r\n");
    return this->transmitCommand("AT+RST");
}

bool WiFi::restore() {
    USB_Serial::transmit("WiFi restore\r\n");
    return this->transmitCommand("AT+RESTORE");
}

bool WiFi::test() {
    USB_Serial::transmit("WiFi test\r\n");
    return this->transmitCommand("AT");
}

bool WiFi::disableEcho() {
    USB_Serial::transmit("WiFi echo disable\r\n");
    return this->transmitCommand("ATE0");
}

bool WiFi::enableEcho() {
    USB_Serial::transmit("WiFi echo enable\r\n");
    return this->transmitCommand("ATE1");
}

void WiFi::transmitConnectionStatus(const string &received) {
    if (received.find("STATUS:2") != string::npos) {
        USB_Serial::transmit("Result: Got IP\r\n");
    }
    else if (received.find("STATUS:3") != string::npos) {
        USB_Serial::transmit("Result: Connected\r\n");
    }
    else if (received.find("STATUS:4") != string::npos) {
        USB_Serial::transmit("Result: Disconnected\r\n");
    }
    else if (received.find("STATUS:5") != string::npos) {
        USB_Serial::transmit("Result: Connection fail\r\n");
    }
}

bool WiFi::checkConnection() {
    USB_Serial::transmit("WiFi status check\r\n");
    string result;
    const bool fResult = this->sendCommand("AT+CIPSTATUS", &result);
    if (fResult) { transmitConnectionStatus(result); }
    return fResult;
}

bool WiFi::setMode(const CWMODE &mode) {
    USB_Serial::transmit("WiFi mode set\r\n");
    return this->transmitCommand("AT+CWMODE=" + to_string(mode));
}

bool WiFi::setDHCP(const CWMODE &mode, const bool &state) {
    USB_Serial::transmit("WiFi DHCP set\r\n");
    const bool _state = !state;
    return this->transmitCommand("AT+CWDHCP=" + to_string(mode) + "," + to_string((int) _state));
}

void WiFi::parseFoundNetworks(const string &data) {
    this->detectedNetworks.clear();

    bool ssidParse = false;
    bool changedLine = true;
    string ssid;
    for (const auto &c : data) {
        if (!ssidParse && c == '\"' && changedLine) {
            ssidParse = true;
            changedLine = false;
        }
        else if (ssidParse && c == '\"' && !changedLine) {
            ssidParse = false;
            detectedNetworks.push_back(ssid);
            ssid.clear();
        }
        else if (ssidParse) { ssid.push_back(c); }
        else if (!changedLine && c == '\n') { changedLine = true; }
    }
}

bool WiFi::checkNetworks() {
    USB_Serial::transmit("WiFi network check\r\n");
    string data;
    bool fResult = this->sendCommand("AT+CWLAP", &data);
    if (!fResult) { return false; }
    else { parseFoundNetworks(data); }
    return true;
}

bool WiFi::checkIP() {
    USB_Serial::transmit("WiFi ip check\r\n");
    return this->transmitCommand("AT+CIPSTA?");
}

bool WiFi::checkVersion() {
    USB_Serial::transmit("WiFi version check\r\n");
    return this->transmitCommand("AT+GMR");
}

bool WiFi::join(const string &ssid, const string &passwd) {
    USB_Serial::transmit("WiFi join\r\n");
    return this->transmitCommand("AT+CWJAP=\"" + ssid + "\",\"" + passwd + "\"");
}

bool WiFi::join() {
    for (const string &str : this->detectedNetworks) {
        const auto it = configuredNetworks.find(str);
        if (it != configuredNetworks.end()) {
            if (this->join(it->first, it->second)) { return true; }
        }
    }
    return false;
}

bool WiFi::quit() {
    USB_Serial::transmit("WiFi quit\r\n");
    return this->transmitCommand("AT+CWQAP");
}

bool WiFi::transmit(const string &req, const string &dataToSend) {
    const string request = HTTP::buildRequest(req, dataToSend);
    this->transmitCommand("AT+CIPSEND=" + to_string(request.length()));
    this->transmitCommand(request);
    USB_Serial::transmit("WiFi transmit\r\n");
    return false;
}

bool WiFi::connect(const string &type, const string &address, const uint16_t &port) {
    USB_Serial::transmit("WiFi connect\r\n");
    const string cmd = "AT+CIPSTART=\"" + type + "\",\"" + address + "\"," + to_string(port);
    return this->transmitCommand(cmd);
}

bool WiFi::send(const string &data) {
    if(!this->transmitCommand("AT+CIPSEND=" + to_string(data.length()))) { return false; }
    if(!this->transmitCommand(HTTP::buildRequest("GET", "/test"))) { return false; }
    return false;
}
