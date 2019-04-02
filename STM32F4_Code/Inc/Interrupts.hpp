//
// Created by Tomasz Kiljańczyk on 01-Mar-19.
//

#ifndef TEST_CPP_INTERRUPTS_H
#define TEST_CPP_INTERRUPTS_H

#include "main.h"
#include "used_headers.h"
#include <utility>
#include <vector>
#include <map>

class Interrupts {
public:
#if defined(HAL_GPIO_MODULE_ENABLED) && defined( GPIO_PIN_HPP_)
    static std::map<uint16_t, std::vector<void(*)()>> gpioInterrupts;
    static void add_gpio_interrupt(const GPIO_Pin& PIN, void(*func)());
#endif
#if defined(STM32F4xx_HAL_TIM_H) && defined( TIMER_HPP_ )
    static std::map<TIM_TypeDef*, std::vector<void(*)()>> timInterrupts;
    static void add_tim_interrupt(const Timer& TIM, void(*func)());
    static void remove_tim_interrupt(const Timer& TIM, void(*func)());
#endif
#if defined(__STM32F4xx_HAL_UART_H) && defined( UART_HPP_ )
    static std::map<USART_TypeDef*, std::vector<void(*)()>> uartInterrupts;
    static void add_uart_interrupt(const UART& uart, void(*func)());
#endif
};

#if defined(HAL_GPIO_MODULE_ENABLED) && defined( GPIO_PIN_HPP_)
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
#endif

#if defined(STM32F4xx_HAL_TIM_H) && defined( TIMER_HPP_ )
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);
#endif

#if defined(__STM32F4xx_HAL_UART_H) && defined( UART_HPP_ )
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart);
#endif

#endif //TEST_CPP_INTERRUPTS_H
