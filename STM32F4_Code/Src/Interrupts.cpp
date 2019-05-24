//
// Created by Tomasz Kiljańczyk on 01-Mar-19.
//

#include <Interrupts.hpp>

#include "Interrupts.hpp"

template<typename TypeDef, typename HandleTypeDef>
void interrupt_with_instance(const std::map<TypeDef *, std::vector<void (*)()>> &map, const HandleTypeDef *handle) {
    for (const auto &elem : map) {
        if (handle->Instance == elem.first) {
            for (const auto &func : elem.second) {
                func();
            }
        }
    }
}


#if defined(HAL_GPIO_MODULE_ENABLED) && defined( GPIO_PIN_HPP_)
std::map<uint16_t, std::vector<void (*)()>> Interrupts::gpioInterrupts;

void Interrupts::add_gpio_interrupt(const GPIO_Pin &PIN, void(*func)()) {
    Interrupts::gpioInterrupts[PIN.getNumber()].push_back(func);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    for (const auto &elem : Interrupts::gpioInterrupts) {
        if (elem.first == GPIO_Pin) {
            for (const auto &func : elem.second) {
                func();
            }
        }
    }
}

#endif //HAL_GPIO_MODULE_ENABLED

#if defined(STM32F4xx_HAL_TIM_H) && defined( TIMER_HPP_ )
std::map<TIM_TypeDef *, std::vector<void (*)()>> Interrupts::timInterrupts;

void Interrupts::add_tim_interrupt(const Timer &TIM, void(*func)()) {
    Interrupts::timInterrupts[TIM.get_handle()->Instance].push_back(func);
}

void Interrupts::remove_tim_interrupt(const Timer &TIM, void (*func)()) {
    auto it = timInterrupts.find(TIM.get_handle()->Instance);

    if (it != timInterrupts.end()) {
        auto* funcVec = &(it->second);
        for (auto i = funcVec->begin(); i != funcVec->end(); ++i) {
            if(*(*i) == *func) {
                funcVec->erase(i);
                return;
            }
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    interrupt_with_instance(Interrupts::timInterrupts, htim);
}

#endif //__STM32F4xx_ADC_H

#if defined(__STM32F4xx_HAL_UART_H) && defined( UART_HPP_ )
std::map<USART_TypeDef *, std::vector<void (*)()>> Interrupts::uartInterrupts;

void Interrupts::add_uart_interrupt(const UART &uart, void(*func)()) {
    Interrupts::uartInterrupts[uart.getHandle()->Instance].push_back(func);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    interrupt_with_instance(Interrupts::uartInterrupts, huart);
}

#endif //__STM32F4xx_HAL_UART_H

