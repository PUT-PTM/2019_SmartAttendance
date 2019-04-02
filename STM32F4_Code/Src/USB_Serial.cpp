//
// Created by Tomasz Kiljańczyk on 20-Mar-19.
//


#include "USB_Serial.hpp"

#include "usbd_cdc_if.h"

using namespace std;

void USB_Serial_init(USBD_HandleTypeDef *_hUsbDeviceFS) {
    USB_Serial::init(_hUsbDeviceFS);
}


USBD_HandleTypeDef *USB_Serial::hUsbDeviceFS = nullptr;
bool USB_Serial::enabled = false;


void USB_Serial::setEnabled(const bool &val) {
    USB_Serial::enabled = val;
    if(val) {
        USB_Serial::transmit("USB serial enabled\r\n");
    }
}

void USB_Serial::init(USBD_HandleTypeDef *_hUsbDeviceFS) {
    USB_Serial::hUsbDeviceFS = _hUsbDeviceFS;
}

void USB_Serial::transmit(const char *dataToSend) {
    uint16_t size = (uint16_t) std::string(dataToSend).length();
    USB_Serial::transmit(dataToSend, size);
}

void USB_Serial::transmit(const char *dataToSend, const uint16_t &size) {
    if (enabled) {
        while (((USBD_CDC_HandleTypeDef *) USB_Serial::hUsbDeviceFS->pClassData)->TxState != 0) {}
        CDC_Transmit_FS((uint8_t *) dataToSend, size);
    }
}

void USB_Serial::transmit(const std::string &dataToSend) {
    USB_Serial::transmit(dataToSend.c_str(), (uint16_t) dataToSend.length());
}

void USB_Serial::transmitLines(const std::string &dataToSend) {
    string line;
    for (const auto &c : dataToSend) {
        line.push_back(c);
        if (c == '\n') {
            if(line[0] != '\r') {
                USB_Serial::transmit(line);
            }
            line.clear();
        }
    }
}
