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

    FATFS FatFs;
    FIL configFile;
    FRESULT fresult;

    std::string serverType;
    std::string serverAddress;
    uint16_t serverPort = 0;

    int CID = 0;
    std::string room;

    void clearArray(char *cString, const uint16_t &size);

    bool configure();

    void addPresenceEntry(const int& SID);

public:
    SmartAttendance(UART_HandleTypeDef &_uart, bool connect, GPIO_TypeDef *RST_Port, const uint16_t &RST_Pin);
};


#endif //WIFI_TEST_SMARTATTENDANCE_H
