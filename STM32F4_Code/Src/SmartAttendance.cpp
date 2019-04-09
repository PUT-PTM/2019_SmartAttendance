//
// Created by Tomasz Kiljańczyk on 08-Apr-19.
//

#include <SmartAttendance.hpp>

#include "HTTP.hpp"
#include "SmartAttendance.hpp"
#include "USB_Serial.hpp"

using namespace std;

SmartAttendance::SmartAttendance(UART_HandleTypeDef &_uart, bool connect, GPIO_TypeDef *RST_Port,
                                 const uint16_t &RST_Pin
) : diodeBlue(GPIOD, GPIO_PIN_15),
    diodeRed(GPIOD, GPIO_PIN_14),
    diodeOrange(GPIOD, GPIO_PIN_13),
    diodeGreen(GPIOD, GPIO_PIN_12),
    espReset(RST_Port, RST_Pin) {

    diodeOrange.on();
    espReset.off();
    HAL_Delay(1500);
    espReset.on();
    HAL_Delay(1500);

    diodeOrange.off();

    if (this->configure()) {
        diodeBlue.on();

        // ESP-01 connection init and network connecting
        if (wifi.init(_uart, connect)) { diodeGreen.on(); }
        else { diodeRed.on(); }

        //wifi.checkVersion();    // Display of ESP software version
        diodeBlue.off();

        wifi.connect(serverType, serverAddress, serverPort);
        string respData;
        wifi.sendHttpRequest("GET", "/test", respData);

        HTTP::Response resp = HTTP::parseResponse(respData);
        USB_Serial::transmit(resp.toString() + "\r\n");

        addPresenceEntry(136257);
    }
}

/*
 * Helper method
 */
void SmartAttendance::clearArray(char *cString, const uint16_t &size) {
    for (uint16_t i = 0; i < size; i++) {
        cString[i] = '\0';
    }
}

/*
 * Adds entry to Presence table in database
 */
void SmartAttendance::addPresenceEntry(const int &SID) {
    string respData;
    const string dataToSend =
            "{\"SID\":" + to_string(SID) + ",\"CID\":" + to_string(CID) + ",\"Room\":\"" + room + "\"}";

    wifi.sendHttpRequest("POST", "/tables/Presence/", dataToSend, respData);

    HTTP::Response resp = HTTP::parseResponse(respData);
    USB_Serial::transmit(resp.toString() + "\r\n");
}

/*
 * Reads configuration data from SD card (saved in JSON)
 * Configures ESP and USB serial com
 */
bool SmartAttendance::configure() {
    // Partition mounting
    diodeRed.on();
    fresult = f_mount(&FatFs, "", 0);
    if (fresult != FR_OK) { return false; }
    diodeRed.off();

    UINT bytesRead;
    char buffer[256];
    string result;

    // File opening
    diodeOrange.on();
    fresult = f_open(&configFile, "CONFIG.INI", FA_OPEN_EXISTING | FA_READ);
    if (fresult != FR_OK) {
        diodeRed.on();
        return false;
    }
    diodeOrange.off();

    // File reading
    do {
        clearArray(buffer, 256);
        f_read(&configFile, buffer, 256, &bytesRead);
        result.append(string(buffer));
    } while (bytesRead >= 256);
    f_close(&configFile);

    // JSON parsing
    diodeRed.on();
    rapidjson::Document configuration;
    configuration.Parse(result.c_str());
    diodeRed.off();

    // USB serial com setup
    diodeBlue.on();
    USB_Serial::setEnabled(configuration["USB_SERIAL"].GetBool());
    diodeBlue.off();

    // Network configuration setup
    diodeGreen.on();
    wifi.setConfiguredNetworks(configuration);
    serverType = configuration["SERVER"]["TYPE"].GetString();
    serverAddress = configuration["SERVER"]["ADDRESS"].GetString();
    serverPort = (uint16_t) configuration["SERVER"]["PORT"].GetInt();
    diodeGreen.off();

    // Course data setup
    diodeBlue.on();
    room = configuration["ROOM"].GetString();
    CID = configuration["CID"].GetInt();
    diodeGreen.off();

    return true;
}
