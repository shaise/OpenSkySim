/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_it.h"
#include "stdio.h"
#include "debug.h"
#include "cc25xx.h"
#include "frsky.h"
#include "storage.h"
#include "io.h"
#include "usbd_customhid.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
typedef struct _simHID {
  uint8_t thr;
  uint8_t rud;
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t slid;
    uint8_t btns;
} s_simHID;

typedef struct _simPnx {
    uint8_t ch1;
    uint8_t ch2;
    uint8_t ch3;
    uint8_t ch4;
    uint8_t ch5;
    uint8_t ch6;
    uint8_t ch7;
    uint8_t ch8;
} s_simPnx;

s_simHID reportSim;
s_simPnx reportPnx;

#define REPORT_SIM_SIZE 7
#define REPORT_PNX_SIZE 8

uint8_t PhoenixRCHash = 0;
uint8_t PhoenixRCRepCount = 0;
uint8_t is_phoenix = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
static void dwt_init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  led_green_off();
  led_red_off();
  dwt_init();
  debug_init();
  cc25xx_init();
  storage_init();
  frsky_init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	frsky_main();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 72;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 100;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
  HAL_TIM_Base_Start_IT(&htim3);
  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 48;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ONBOARD_LED_GPIO_Port, ONBOARD_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CC2500_MOSI_Pin|CC2500_CLK_Pin|CC2500_CSN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ONBOARD_LED_Pin */
  GPIO_InitStruct.Pin = ONBOARD_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ONBOARD_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PNX_SEL_Pin CC2500_MISO_Pin BIND_Pin */
  GPIO_InitStruct.Pin = PNX_SEL_Pin|CC2500_MISO_Pin|BIND_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RED_LED_Pin */
  GPIO_InitStruct.Pin = RED_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RED_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CC2500_MOSI_Pin CC2500_CLK_Pin CC2500_CSN_Pin */
  GPIO_InitStruct.Pin = CC2500_MOSI_Pin|CC2500_CLK_Pin|CC2500_CSN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
static void dwt_init(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    //DWT->LAR = 0xC5ACCE55;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

#pragma GCC push_options
#pragma GCC optimize ("O3")
void delay_us(uint32_t us) {
	volatile uint32_t cycles = (SystemCoreClock/1000000L)*us;
	volatile uint32_t start = DWT->CYCCNT;
	do  {
	} while(DWT->CYCCNT - start < cycles);
}
#pragma GCC pop_options

uint8_t frsky_to_byte(uint16_t channel)
{
	// map frsky data to single byte
	// frsky seems to send us*1.5 (~1480...3020)
	// frsky span is 3020-1480  = 1540
	int val = channel - 1480;
	if (val < 0)
		val = 0;
	val = (val * 255) / 1540;
	if (val > 255)
		val = 255;
	return (uint8_t)val;
}

static void UpdatePhoenixHash()
{
  if (PhoenixRCRepCount > 0)
  {
    PhoenixRCRepCount--;
    if (PhoenixRCRepCount == 0)
      PhoenixRCHash = 0;
  }
}

static void hid_update_sim(uint16_t *channels)
{
  uint8_t ch7, ch8;
  reportSim.x = frsky_to_byte(channels[0]);
  reportSim.y = frsky_to_byte(channels[1]);
  reportSim.z = frsky_to_byte(channels[4]);
  reportSim.thr = frsky_to_byte(channels[2]);
  reportSim.rud = frsky_to_byte(channels[3]);
  reportSim.slid = frsky_to_byte(channels[5]);
  ch7 = frsky_to_byte(channels[6]);
  ch8 = frsky_to_byte(channels[7]);
  reportSim.btns = 0;
  if (ch7 > 196)
    reportSim.btns |= 1;
  else if (ch7 > 64)
    reportSim.btns |= 4;
  if (ch8 > 196)
    reportSim.btns |= 2;
  else if (ch8 > 64)
    reportSim.btns |= 8;

  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&reportSim, REPORT_SIM_SIZE);
}

static void hid_update_pnx(uint16_t *channels)
{
  reportPnx.ch1 = frsky_to_byte(channels[0]);
  reportPnx.ch2 = PhoenixRCHash;
  reportPnx.ch3 = frsky_to_byte(channels[1]);
  reportPnx.ch4 = frsky_to_byte(channels[2]);
  reportPnx.ch5 = frsky_to_byte(channels[3]);
  reportPnx.ch6 = frsky_to_byte(channels[4]);
  reportPnx.ch7 = frsky_to_byte(channels[5]);
  reportPnx.ch8 = 0x00;

  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&reportPnx, REPORT_PNX_SIZE);
}

void hid_update(uint16_t *channels)
{
  if (is_phoenix)
    hid_update_pnx(channels);
  else
    hid_update_sim(channels);
}

static void hid_update_sim_null(void)
{
  reportSim.x = 0x80;
  reportSim.y = 0x80;
  reportSim.z = 0x80;
  reportSim.thr = 0x00;
  reportSim.rud = 0x80;
  reportSim.slid = 0x80;
  reportSim.btns = 0;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&reportSim, REPORT_SIM_SIZE);
}

static void hid_update_pnx_null(void)
{
  UpdatePhoenixHash();
  reportPnx.ch1 = 0xff;
  reportPnx.ch2 = PhoenixRCHash;
  reportPnx.ch3 = 0xff;
  reportPnx.ch4 = 0xff;
  reportPnx.ch5 = 0xff;
  reportPnx.ch6 = 0x00;
  reportPnx.ch7 = 0x00;
  reportPnx.ch8 = 0x00;
  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&reportPnx, REPORT_PNX_SIZE);
}

void hid_update_null(void)
{
  if (is_phoenix)
    hid_update_pnx_null();
  else
    hid_update_sim_null();
}

void HandleSetReport(void)
{
  uint8_t i;
  uint8_t type;
  uint32_t sum;
  uint8_t *b;

  PhoenixRCHash = 0;
  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)hUsbDeviceFS.pClassData;
  b = hhid->Report_buf;

  // calculate phoenixrc hash
  type = b[2];
  if (type > 0)
    type--;
  type >>= 5;
  // 1. calculate sum
  switch (type) {
  case 0: sum = b[6]; break;
  case 1: sum = b[3]; break;
  case 2: sum = b[4] + b[5]; break;
  case 3: sum = b[1] + b[6]; break;
  case 4: sum = b[0] + b[7]; break;
  case 5: sum = b[5] + b[6]; break;
  case 6: sum = b[0] + b[1] + b[3] + b[4]; break;
  case 7:
    sum = 0;
    for(i = 0; i < 8; i++)
      if (i != 2)
        sum += b[i];
    break;
  }
  // 2. calc hash
  type &= 3;
  switch (type) {
  case 0: PhoenixRCHash = (uint8_t)(((sum * 8) & 0x7F) + 0x0D); break;
  case 1: PhoenixRCHash = (uint8_t)(((sum * 2) & 0x3F) + 0x12); break;
  case 2: PhoenixRCHash = (uint8_t)((((sum + 8) / 2) & 0x1F) + 0x10); break;
  case 3: PhoenixRCHash = (uint8_t)((((sum + 4) / 8) & 0xF) + 0x18); break;
  }
  PhoenixRCRepCount = 250;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
