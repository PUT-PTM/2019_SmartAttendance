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
uint32_t bytesToInt(const uint8_t data[4]) {
    uint32_t result = data[0];
    result <<= 8;
    result += data[1];
    result <<= 8;
    result += data[2];
    result <<= 8;
    result += data[3];
    return result;
}

void copy_id(const uint8_t input[4], uint8_t output[4]) {
    for (uint8_t i = 0; i < 4; i++) {
        output[i] = input[i];
    }
}


//Ta funkcja dzieje sie przed pętlą główną
void setup(void) {
    //Inicjalizacja czytnika RFID
    RFID_init();

    if (smartAttendance.configureWifi()) {
        diodeBlue.on();

        // ESP-01 connection init and network connecting
        if (smartAttendance.wifi.init(huart3, true)) { diodeGreen.on(); }
        else { diodeRed.on(); }

        diodeBlue.off();

        bool connected = false;
        while (!connected) {
            connected = smartAttendance.wifi.connect(smartAttendance.serverType, smartAttendance.serverAddress,
                                                     smartAttendance.serverPort);
        }
        string respData;
        HTTP::Response response;

        while(response.statusCode != 200) {
            HAL_Delay(100);
            smartAttendance.wifi.sendHttpRequest("GET", "/test", respData);

            response = HTTP::parseResponse(respData);
            USB_Serial::transmit(response.toString() + "\r\n");
        }

    }
}

void loop(void) {
    uint8_t EmptyId[5]{0, 0, 0, 0};
    uint8_t CardIdPrev[4]{0, 0, 0, 0};
    uint8_t CardId[5]{0, 0, 0, 0, 0};

    diodeOrange.on();

    // Pragmy usuwają ostrzeżenie o nieskończonej pętli
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        uint8_t result = check_for_card(CardId);
        if (result == MI_OK && compare_ID(EmptyId, CardId) == MI_ERR) {
            diodeOrange.off();
            if (compare_ID(CardIdPrev, CardId) == MI_ERR) {
                diodeBlue.on();
                smartAttendance.addPresenceEntry(bytesToInt(CardId));
                copy_id(CardId, CardIdPrev);
                USB_Serial::transmit("Card ID post finished!");
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
