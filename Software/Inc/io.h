#ifndef __LED_H__
#define __LED_H__

#include "main.h"

// set/clear/toggle
#define led_green_off()    HAL_GPIO_WritePin(ONBOARD_LED_GPIO_Port, ONBOARD_LED_Pin, 1)
#define led_green_on()     HAL_GPIO_WritePin(ONBOARD_LED_GPIO_Port, ONBOARD_LED_Pin, 0)
#define led_green_toggle() HAL_GPIO_TogglePin(ONBOARD_LED_GPIO_Port, ONBOARD_LED_Pin)

// set/clear/toggle
#define led_red_off()    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, 1)
#define led_red_on()     HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, 0)
#define led_red_toggle() HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin)

// led init routines
#define led_init() {  led_red_off(); led_green_off();}

// bind functions
#define io_bind_request() (HAL_GPIO_ReadPin(BIND_GPIO_Port, BIND_Pin) == 0)

#endif


