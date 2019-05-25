#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-non-const-parameter"
//
// Created by Tomasz Kiljańczyk on 08-Apr-19.
//

#include <SmartAttendance.hpp>

#include "my_rc522.h"
#include "HTTP.hpp"
#include "SmartAttendance.hpp"
#include "USB_Serial.hpp"

using namespace std;

/*
 * Helper method
 */
void clearArray(char *cString, const uint16_t &size) {
    for (uint16_t i = 0; i < size; i++) {
        cString[i] = '\0';
    }
}

/*
 * Adds entry to Presence table in database
 */
void SmartAttendance::addPresenceEntry(const uint32_t &SID) {
    string respData;
    const string dataToSend =
            "{\"SID\":" + to_string(SID) + ",\"CID\":" + to_string(CID) + R"(,"Room":")" + room + "\"}";

    wifi.sendHttpRequest("POST", "/tables/Presence/", dataToSend, respData);

    HTTP::Response resp = HTTP::parseResponse(respData);
    USB_Serial::transmit(resp.toString() + "\r\n");
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

/*
 * Reads configuration data from SD card (saved in JSON)
 * Configures ESP and USB serial com
 */
bool SmartAttendance::configureWifi() {
    // Restart modułu wifi
    diodeOrange.on();
    espReset.off();
    HAL_Delay(1000);
    espReset.on();
    HAL_Delay(1000);
    diodeOrange.off();

    // Podmontowanie partycji
    diodeRed.on();
    FATFS FatFs;
    FRESULT fresult = f_mount(&FatFs, "", 0);
    if (fresult != FR_OK) { return false; }
    diodeRed.off();

    UINT bytesRead;
    char buffer[256];
    string result;

    // Otwarcie pliku
    diodeOrange.on();
    FIL configFile;
    fresult = f_open(&configFile, "CONFIG.INI", FA_OPEN_EXISTING | FA_READ);
    if (fresult != FR_OK) {
        diodeRed.on();
        return false;
    }
    diodeOrange.off();

    // Odczyt pliku
    do {
        clearArray(buffer, 256);
        f_read(&configFile, buffer, 256, &bytesRead);
        result.append(string(buffer));
    } while (bytesRead >= 256);
    f_close(&configFile);

    // Parsowanie JSON
    diodeRed.on();
    rapidjson::Document configuration;
    configuration.Parse(result.c_str());
    diodeRed.off();

    // Rozstawianie USB serial com
    diodeBlue.on();
    USB_Serial::setEnabled(configuration["USB_SERIAL"].GetBool());
    diodeBlue.off();

    // Konfiguracja danych sieci
    diodeGreen.on();
    wifi.setConfiguredNetworks(configuration);
    serverType = configuration["SERVER"]["TYPE"].GetString();
    serverAddress = configuration["SERVER"]["ADDRESS"].GetString();
    serverPort = (uint16_t) configuration["SERVER"]["PORT"].GetInt();
    diodeGreen.off();

    // Ustawienie danych zajęć
    diodeBlue.on();
    room = configuration["ROOM"].GetString();
    CID = configuration["CID"].GetInt();
    diodeBlue.off();

    return true;
}
