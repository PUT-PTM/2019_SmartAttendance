/*
 * GPIO_Pin.h
 *
 *  Created on: Dec 16, 2018
 *      Author: Tomasz Kilja�czyk
 */

#ifndef GPIO_PIN_HPP_
#define GPIO_PIN_HPP_

#include <initializer_list>
#include "main.h"


class GPIO_Pin final {
private:
	GPIO_TypeDef* port;
	const uint16_t number = 0;

public:
	explicit GPIO_Pin(GPIO_TypeDef* GPIOx, const uint16_t& GPIO_Pin) noexcept;
	~GPIO_Pin() = default;

	GPIO_TypeDef* getPort() const;
	const uint16_t& getNumber() const;

	void on() const;
	void off() const;
	void toggle() const;
	void write(const bool& state) const;
	void write(const GPIO_PinState& state) const;
	bool read() const;
};

void pins_toggle(const std::initializer_list<const GPIO_Pin*>& pins);
void pins_write(const std::initializer_list<const GPIO_Pin*>& pins, const GPIO_PinState& state);
void pins_write(const std::initializer_list<const GPIO_Pin*>& pins, const bool& state);

#endif /* GPIO_PIN_HPP_ */
