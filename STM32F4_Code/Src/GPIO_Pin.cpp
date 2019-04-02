/*
 * GPIO_Pin.cpp
 *
 *  Created on: Dec 16, 2018
 *      Author: Tomasz Kiljańczyk
 */

#include "GPIO_Pin.hpp"

// STL
#include <map>
#include <GPIO_Pin.hpp>


/*
 * @brief Initializes GPIO_Pin.
 * @param base GPIOx where x can be (A..K) to select the GPIO peripheral for STM32F429X device or
 *                      	x can be (A..I) to select the GPIO peripheral for STM32F40XX and STM32F427X devices.
 * @param GPIO_Pin specifies the port bit.
 * @retval None
 */
GPIO_Pin::GPIO_Pin(GPIO_TypeDef* GPIOx, const uint16_t& GPIO_Pin) noexcept : port(GPIOx), number(GPIO_Pin) {}

GPIO_TypeDef* GPIO_Pin::getPort() const
{
    return port;
}

const uint16_t& GPIO_Pin::getNumber() const
{
    return number;
}

/*
 * @brief Sets the number.
 * @retval None
 */
void GPIO_Pin::on() const
{
	HAL_GPIO_WritePin(port, number, GPIO_PIN_SET);
}

/*
 * @brief Clears the number.
 * @retval None
 */
void GPIO_Pin::off() const
{
	HAL_GPIO_WritePin(port, number, GPIO_PIN_RESET);
}

/*
 * @brief Toggles the number.
 * @retval None
 */
void GPIO_Pin::toggle() const
{
	HAL_GPIO_TogglePin(port, number);
}

/*
 * @brief Sets or clears the number.
 * @param state specifies the value to be written.
 *          This parameter can be bool value:
 *           	@arg true: to set the number
 *           	@arg false: to clear the number
 * @retval None
 */
void GPIO_Pin::write(const bool& state) const
{
	if (!state) 	{ HAL_GPIO_WritePin(port, number, GPIO_PIN_RESET); }
	else { HAL_GPIO_WritePin(port, number, GPIO_PIN_SET); 	}
}

/*
 * @brief Sets or clears the number.
 * @param state specifies the value to be written.
 *          This parameter can be one of the GPIO_PinState enum values:
 *           	@arg GPIO_PIN_RESET: to clear the number
 *           	@arg GPIO_PIN_SET: to set the number
 * @retval None
 */
void GPIO_Pin::write(const GPIO_PinState& state) const
{
	HAL_GPIO_WritePin(port, number, state);
}

/*
 * @brief Reads the number.
 * @retval The number value.
 */
bool GPIO_Pin::read() const
{
	return (HAL_GPIO_ReadPin(port, number) == GPIO_PIN_SET);
}



// Pin group functions ----------------

/*
 * @brief Toggles given pins.
 * @param pins specifies group of pins to toggle.
 * @retval None
 */
void pins_toggle(const std::initializer_list<const GPIO_Pin*>& pins)
{
	if(pins.size() > 0)
	{
		/*
		 * key: GPIO port
		 * value: number sum
		 */
		std::map<GPIO_TypeDef*, uint16_t> portPinMap;

		for(const GPIO_Pin* pin : pins) { portPinMap[pin->getPort()] |= pin->getNumber(); }
		for(const auto& elem : portPinMap)
		{
			if(elem.second != 0){ HAL_GPIO_TogglePin(elem.first, elem.second); }
		}
	}
}

/*
 * @brief Sets or clears group of pins.
 * @param state specifies the value to be written.
 *          This parameter can be one of the GPIO_PinState enum values:
 *           	@arg GPIO_PIN_RESET: to clear the number
 *           	@arg GPIO_PIN_SET: to set the number
 * @retval None
 */
void pins_write(const std::initializer_list<const GPIO_Pin*>& pins, const GPIO_PinState& state)
{
	if(pins.size() > 0)
	{
		/*
		 * key: GPIO port
		 * value: number sum
		 */
		std::map<GPIO_TypeDef*, uint16_t> portPinMap;

		for(const GPIO_Pin* pin : pins) { portPinMap[pin->getPort()] |= pin->getNumber(); }
		for(const auto& elem : portPinMap)
		{
			if(elem.second != 0) { HAL_GPIO_WritePin(elem.first, elem.second, state); }
		}
	}
}

/*
 * @brief Sets or clears group of pins.
 * @param state specifies the value to be written.
 *          This parameter can be bool value:
 *           	@arg true: to set the number
 *           	@arg false: to clear the number
 * @retval None
 */
void pins_write(const std::initializer_list<const GPIO_Pin*>& pins, const bool& state)
{
	if(state) { pins_write(pins, GPIO_PIN_SET); }
	else { pins_write(pins, GPIO_PIN_RESET); }
}
