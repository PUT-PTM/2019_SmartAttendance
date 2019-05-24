//
// Created by Tomasz Kiljańczyk on 08-Apr-19.
//

#ifndef WIFI_TEST_SMARTATTENDANCE_H
#define WIFI_TEST_SMARTATTENDANCE_H

#include "WiFi.hpp"
#include "GPIO_Pin.hpp"
#include "main.h"
#include "ff.h"

class SmartAttendance final {
private:
    WiFi wifi;

    GPIO_Pin diodeBlue;
    GPIO_Pin diodeRed;
    GPIO_Pin diodeOrange;
    GPIO_Pin diodeGreen;
    GPIO_Pin espReset;


    std::string serverType;
    std::string serverAddress;
    uint16_t serverPort = 0;

    int CID = 0;
    std::string room;

    bool configureWifi();

    void addPresenceEntry(const uint32_t &SID);

    void initRfid();

public:
    explicit SmartAttendance();

    void configure(const bool& connect);

    void loop();
};


#endif //WIFI_TEST_SMARTATTENDANCE_H
