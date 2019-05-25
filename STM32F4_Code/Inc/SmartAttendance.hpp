//
// Created by Tomasz Kiljańczyk on 08-Apr-19.
//

#ifndef WIFI_TEST_SMARTATTENDANCE_H
#define WIFI_TEST_SMARTATTENDANCE_H

#include "WiFi.hpp"
#include "GPIO_Pin.hpp"
#include "main.h"
#include "ff.h"

static GPIO_Pin diodeBlue = GPIO_Pin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
static GPIO_Pin diodeRed = GPIO_Pin(LED_RED_GPIO_Port, LED_RED_Pin);
static GPIO_Pin diodeOrange = GPIO_Pin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin);
static GPIO_Pin diodeGreen = GPIO_Pin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
static GPIO_Pin espReset = GPIO_Pin(ESP_RST_GPIO_Port, ESP_RST_Pin);

class SmartAttendance final {
private:

    int CID = 0;
    std::string room;

public:
    explicit SmartAttendance() = default;

    bool configureWifi();

    WiFi wifi;
    std::string serverType;
    std::string serverAddress;
    uint16_t serverPort = 0;

    void addPresenceEntry(const uint32_t &SID);
};


#endif //WIFI_TEST_SMARTATTENDANCE_H
