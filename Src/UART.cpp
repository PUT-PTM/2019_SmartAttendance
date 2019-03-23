/*
 * UART.cpp
 *
 *  Created on: Dec 17, 2018
 *      Author: Tomasz Kiljańczyk
 */

#include "UART.hpp"
#include <string>
#include <UART.hpp>

#define TIMEOUT 200

using namespace std;

UART::UART(UART_HandleTypeDef *huartx) noexcept : handle(huartx) {}

void UART::init(UART_HandleTypeDef *huartx) {
    handle = huartx;
}

UART_HandleTypeDef *UART::getHandle() {
    return this->handle;
}

const UART_HandleTypeDef *UART::getHandle() const {
    return this->handle;
}


bool UART::transmit(uint8_t *tData, const uint16_t &size) const {
    if (handle != nullptr) {
        const auto result = HAL_UART_Transmit(handle, tData, size, TIMEOUT);
        return result == HAL_OK;
    }
    else { return false; }
}

bool UART::transmit(const char *tData) const {
    if (handle != nullptr) {
        const uint16_t size = (uint16_t) string(tData).size();
        uint8_t converted[size];
        for (int16_t i = 0; i < size; i++) { converted[i] = (uint8_t) tData[i]; }
        return this->transmit(converted, size);
    }
    else { return false; }
}

bool UART::transmit(const char *tData, const uint16_t &size) const {
    if (handle != nullptr) {
        return this->transmit((uint8_t *) tData, size);
    }
    else { return false; }
}


bool UART::transmit(const std::string tData) const {
    if (handle != nullptr) {
        return this->transmit((uint8_t *) tData.c_str(), (uint16_t) tData.size());
    }
    else { return false; }
}


bool UART::receive(uint8_t *rData, const uint16_t &size) const {
    if (handle != nullptr) {
        const auto result = HAL_UART_Receive(handle, rData, size, TIMEOUT);
        return result == HAL_OK;
    }
    else { return false; }
}

bool UART::receive(char *rData, const uint16_t &size) const {
    return this->receive((uint8_t *) rData, size);
}

bool UART::receive(string &rData, const uint16_t &size) const {
    if (handle != nullptr) {
        uint8_t rData_[size];
        const auto result = HAL_UART_Receive(handle, rData_, size, 1000);
        if (result != HAL_OK) { return false; }
        else {
            rData = string((char *) rData_);
            return true;
        }
    }
    else { return false; }
}

void UART::discardAll() const {
    char tempC[1];
    while (this->receive(tempC, 1)) {}
}

void UART::receiveAll(std::string &rData) {
    std::string result;
    char tempC[1];
    while (this->receive(tempC, 1)) { result.push_back(tempC[0]); }
    rData.clear();
    rData.shrink_to_fit();
    rData = result;
}
