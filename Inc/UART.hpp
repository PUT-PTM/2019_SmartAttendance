/*
 * UART.hpp
 *
 *  Created on: Dec 17, 2018
 *      Author: Tomasz Kiljańczyk
 */

#ifndef UART_HPP_
#define UART_HPP_

#include "main.h"
#include <string>

class UART final {
	UART_HandleTypeDef* handle;

public:
	explicit UART() noexcept : handle(nullptr) {}
	explicit UART(UART_HandleTypeDef* huartx) noexcept;
	~UART() = default;

	void init(UART_HandleTypeDef* huartx);
	UART_HandleTypeDef* getHandle();
	const UART_HandleTypeDef* getHandle() const;

	bool transmit(uint8_t *tData, const uint16_t &size) const;
	bool transmit(const char *tData, const uint16_t &size) const;
	bool transmit(const char* tData) const;
	bool transmit(const std::string tData) const;

	void discardAll() const;

	bool receive(uint8_t *rData, const uint16_t &size) const;
	bool receive(char *rData, const uint16_t &size) const;
	bool receive(std::string& rData, const uint16_t &size) const;
	void receiveAll(std::string& rData);
};

#endif /* UART_HPP_ */
