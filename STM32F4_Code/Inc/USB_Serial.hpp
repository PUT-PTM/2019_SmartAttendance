//
// Created by Tomasz Kiljańczyk on 20-Mar-19.
//

#ifndef WIFI_TEST_USB_SERIAL_HPP
#define WIFI_TEST_USB_SERIAL_HPP

#include "usb_device.h"

#ifdef __cplusplus

#include <string>

#endif //__cplusplus

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
void USB_Serial_init(USBD_HandleTypeDef *_hUsbDeviceFS);
#ifdef __cplusplus
}
#endif //__cplusplus

#ifdef __cplusplus

class USB_Serial final {
private:
    static USBD_HandleTypeDef *hUsbDeviceFS;
    static bool enabled;

public:
    static void init(USBD_HandleTypeDef *_hUsbDeviceFS);

    static void setEnabled(const bool &val);

    static void transmit(const char *dataToSend);

    static void transmit(const char *dataToSend, const uint16_t &size);

    static void transmit(const std::string &dataToSend);

    static void transmitLines(const std::string &dataToSend);
};

#endif //__cplusplus

#endif //WIFI_TEST_USB_SERIAL_HPP
