/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void delay_us(uint32_t us);
void hid_update(uint16_t *channels);
void hid_update_null(void);
void HandleSetReport(void);
extern uint8_t is_phoenix;



/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ONBOARD_LED_Pin GPIO_PIN_13
#define ONBOARD_LED_GPIO_Port GPIOC
#define PNX_SEL_Pin GPIO_PIN_14
#define PNX_SEL_GPIO_Port GPIOB
#define RED_LED_Pin GPIO_PIN_15
#define RED_LED_GPIO_Port GPIOA
#define CC2500_MOSI_Pin GPIO_PIN_4
#define CC2500_MOSI_GPIO_Port GPIOB
#define CC2500_CLK_Pin GPIO_PIN_5
#define CC2500_CLK_GPIO_Port GPIOB
#define CC2500_MISO_Pin GPIO_PIN_6
#define CC2500_MISO_GPIO_Port GPIOB
#define CC2500_CSN_Pin GPIO_PIN_7
#define CC2500_CSN_GPIO_Port GPIOB
#define BIND_Pin GPIO_PIN_9
#define BIND_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define DEBUG 1
#define UART_DEBUG

// failsafe: bind button press time in ms the will initiate failsafe programming sequence
#define FAILSAFE_PRESS_TIME 50   // 5 seconds
#define FAILSAFE_PROGRAM_WAIT_TIME 100   // 10 seconds

// frsky defs
#define PWM_FRSKY_TO_US(_frsky) ((_frsky)*2/3)
#define DEFAULT_FSCAL_VALUE -62

// no watch dog for now
#define wdt_reset()

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
