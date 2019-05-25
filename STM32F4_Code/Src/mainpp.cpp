//
// Created by aszcz on 25-May-19.
//

#include <mainpp.h>
#include <SmartAttendance.hpp>
#include <my_rc522.h>
#include <HTTP.hpp>
#include <USB_Serial.hpp>

SmartAttendance smartAttendance; // NOLINT(cert-err58-cpp)

using namespace std;

extern UART_HandleTypeDef huart3;


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

void setup(void) {
    //Inicjalizacja czytnika RFID
    RFID_init();

    if (smartAttendance.configureWifi()) {
        diodeBlue.on();

        // ESP-01 connection init and network connecting
        if (smartAttendance.wifi.init(huart3, true)) { diodeGreen.on(); }
        else { diodeRed.on(); }

        diodeBlue.off();

        smartAttendance.wifi.connect(smartAttendance.serverType, smartAttendance.serverAddress,
                                     smartAttendance.serverPort);
        string respData;
        HAL_Delay(100);
        smartAttendance.wifi.sendHttpRequest("GET", "/test", respData);

        HTTP::Response resp = HTTP::parseResponse(respData);
        USB_Serial::transmit(resp.toString() + "\r\n");

    }
}

void copy_id(const uint8_t input[4], uint8_t output[4]) {
    for (uint8_t i = 0; i < 4; i++) {
        output[i] = input[i];
    }
}

void loop(void) {
    unsigned char CardIdPrev[4]{0, 0, 0, 0};
    unsigned char CardId[5]{0, 0, 0, 0, 0};

    diodeOrange.on();

    // Pragmy usuwają ostrzeżenie o nieskończonej pętli
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        uint8_t result = check_for_card(CardId);
        if (result == MI_OK) {
            diodeOrange.off();
            if (compare_ID(CardIdPrev, CardId) == MI_ERR) {
                diodeBlue.on();
                smartAttendance.addPresenceEntry(bytesToInt(CardId));
                copy_id(CardId, CardIdPrev);
            }
        }
        else {
            diodeBlue.off();
            diodeOrange.on();
        }
        HAL_Delay(200);
    }
#pragma clang diagnostic pop
}
