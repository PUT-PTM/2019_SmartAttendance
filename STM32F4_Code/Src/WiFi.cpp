//
// Created by Tomasz Kiljańczyk on 13-Mar-19.
//

#include "WiFi.hpp"

#include "HTTP.hpp"
#include "GPIO_Pin.hpp"
#include "USB_Serial.hpp"

using namespace std;


// uncategorized methods section
bool WiFi::init(UART_HandleTypeDef &_uart, bool connect) {
    this->uart = UART(&_uart);

    HAL_Delay(100);
    if (!this->test()) { return false; }
    if (!this->reset()) { return false; }
    if (!this->disableEcho()) { return false; }

    if (connect) {
        if (!this->setMode(CWMODE_STATION)) { return false; }
        if (!this->quit()) { return false; }
        if (!this->transmitCommand("AT+CWLAPOPT=1,2")) { return false; }
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

    this->configuredNetworks.clear();

    for (const auto &elem : data["NETWORKS"].GetArray()) {
        const string ssid = elem["SSID"].GetString();
        const string passwd = elem["PASSWD"].GetString();
        USB_Serial::transmit(ssid + "\r\n");
        this->configuredNetworks.emplace(ssid, passwd);
    }
    USB_Serial::transmit("\r\n");
}

void WiFi::connectionStatus(const string &received) {
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

bool WiFi::join() {
    for (const string &str : this->detectedNetworks) {
        const auto it = configuredNetworks.find(str);
        if (it != configuredNetworks.end()) {
            if (this->join(it->first, it->second)) { return true; }
        }
    }
    return false;
}

void WiFi::parseFoundNetworks(const string &data) {
    this->detectedNetworks.clear();
    this->detectedNetworks.shrink_to_fit();

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
            detectedNetworks.emplace_back(ssid);
            ssid.clear();
            ssid.shrink_to_fit();
        }
        else if (ssidParse) { ssid.push_back(c); }
        else if (!changedLine && c == '\n') { changedLine = true; }
    }
}
// end uncategorized methods section



// AT command section
bool WiFi::reset() {
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

bool WiFi::checkConnection() {
    USB_Serial::transmit("WiFi status check\r\n");
    string result;
    const bool fResult = this->transmitCommand("AT+CIPSTATUS", &result);
    if (fResult) { connectionStatus(result); }
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

bool WiFi::checkNetworks() {
    USB_Serial::transmit("WiFi network check\r\n");
    string data;
    bool fResult = this->transmitCommand("AT+CWLAP", &data);
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

bool WiFi::quit() {
    USB_Serial::transmit("WiFi quit\r\n");
    return this->transmitCommand("AT+CWQAP");
}

bool WiFi::connect(const string &type, const string &address, const uint16_t &port) {
    USB_Serial::transmit("WiFi connect\r\n");
    const string cmd = "AT+CIPSTART=\"" + type + "\",\"" + address + "\"," + to_string(port);
    if (!this->transmitCommand(cmd)) { return false; }
    else {
        connectedHost = address + ":" + to_string(port);
        return true;
    }
}


// transmision section
bool WiFi::transmitCommand(const string &cmd, string *result) {
    USB_Serial::transmit("WiFi command start -----------------------------\r\n");
    USB_Serial::transmit("Command: " + cmd + "\r\n");

    uart.discardAll();

    string temp;
    while (true) {
        if (temp.find("busy") == string::npos) {
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
    return this->transmitCommand(cmd, nullptr);
}

bool WiFi::sendHttpRequest(const string &req, const string &url, const string &dataToSend, string &response) {
    USB_Serial::transmit(dataToSend + "\r\n");
    const string request = HTTP::buildRequest(connectedHost, req, url, dataToSend);
    USB_Serial::transmit(request);
    return this->send(request, response);
}

bool WiFi::sendHttpRequest(const string &req, const string &url, string &response) {
    const string request = HTTP::buildRequest(connectedHost, req, url);
    return this->send(request, response);
}

bool WiFi::send(const string &data, string &response) {
    USB_Serial::transmit("WiFi send\r\n");
    if (!this->transmitCommand("AT+CIPMODE=1")) { return false; }
    if (!this->uart.transmit("AT+CIPSEND\r\n")) { return false; }
    HAL_Delay(20);
    if (!this->uart.transmit(data + "\r\n")) { return false; }
    this->uart.receiveAll(response);
    HAL_Delay(50);
    if (!this->uart.transmit("+++")) { return false; }
    HAL_Delay(50);
    if (!this->transmitCommand("AT+CIPMODE=0")) { return false; }

    uint32_t respEnd = response.length() - 1;
    if (response[respEnd] == 'K' && response[respEnd - 1] == 'O' && response[respEnd - 2] == ' ') {
        response.resize(respEnd - 2);
        response.shrink_to_fit();
    }

    return false;
}
// end transmission section
