//
// Created by Tomasz Kiljańczyk on 08-Apr-19.
//

#include <SmartAttendance.hpp>

#include "my_rc522.h"
#include "HTTP.hpp"
#include "SmartAttendance.hpp"
#include "USB_Serial.hpp"

using namespace std;

extern UART_HandleTypeDef huart3;

SmartAttendance::SmartAttendance() {
    diodeBlue = GPIO_Pin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    diodeRed = GPIO_Pin(LED_RED_GPIO_Port, LED_RED_Pin);
    diodeOrange = GPIO_Pin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin);
    diodeGreen = GPIO_Pin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    espReset = GPIO_Pin(ESP_RST_GPIO_Port, ESP_RST_Pin);
    diodeOrange.on();
    espReset.off();
    HAL_Delay(1000);
    espReset.on();
    HAL_Delay(1000);
    diodeOrange.off();
}

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
            "{\"SID\":" + to_string(SID) + ",\"CID\":" + to_string(CID) + ",\"Room\":\"" + room + "\"}";

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
    // Partition mounting
    diodeRed.on();
    FATFS FatFs;
    FRESULT fresult = f_mount(&FatFs, "", 0);
    if (fresult != FR_OK) { return false; }
    diodeRed.off();

    UINT bytesRead;
    char buffer[256];
    string result;

    // File opening
    diodeOrange.on();
    FIL configFile;
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
    diodeBlue.off();

    return true;
}

void SmartAttendance::initRfid() {
    init();
}

void SmartAttendance::configure(const bool &connect) {
    this->initRfid();
    if (this->configureWifi()) {
        diodeBlue.on();

        // ESP-01 connection init and network connecting
        if (wifi.init(huart3, connect)) { diodeGreen.on(); }
        else { diodeRed.on(); }

        diodeBlue.off();

        wifi.connect(serverType, serverAddress, serverPort);
        string respData;
        HAL_Delay(100);
        wifi.sendHttpRequest("GET", "/test", respData);

        HTTP::Response resp = HTTP::parseResponse(respData);
        USB_Serial::transmit(resp.toString() + "\r\n");

    }
}


/*
 * Konwertuje tablicę 4 bajtów na 32 bitowy int.
 */
uint32_t bytesToInt(unsigned char data[4]) {
    uint32_t result = data[0];
    result <<= 8;
    result += data[1];
    result <<= 8;
    result += data[2];
    result <<= 8;
    result += data[3];
    return result;
}

unsigned char CardIdPrev[4] {0, 0, 0, 0};
unsigned char CardId[4];

#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void SmartAttendance::loop() {
    diodeOrange.on();
    while (true) {
        uint8_t result = check_for_card(CardId);
        if (result == MI_OK) {
            diodeOrange.off();
            if (compare_ID(CardIdPrev, CardId) == MI_ERR) {
                diodeBlue.on();
                addPresenceEntry(bytesToInt(CardId));
                strcpy(reinterpret_cast<char *>(CardIdPrev), reinterpret_cast<const char *>(CardId));
            }
        }
        else {
            diodeBlue.off();
            diodeOrange.on();
        }
        HAL_Delay(200);
    }
}
