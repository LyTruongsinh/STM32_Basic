/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "UartRingbuffer_multi.h"
#include "NMEA.h"
#include "i2c-lcd.h"
#include <stdio.h>
#include <string.h>
#include "ESP8266_HAL.h"
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define gps_uart &huart1 // gps_uart
#define wifi_uart &huart2 // wifi_uart
#define WIFI_SSID "Tho" // Your wifi SSID
#define WIFI_PASSWORD "123456789" // Your wifi password
#define API_KEY "123456789" // Your own Thingspeak API key
char GGA[100];
char RMC[100];
GPSSTRUCT gpsData;
int flagGGA = 0, flagRMC = 0;
char lcdBuffer[50];
int VCCTimeout = 5000; // GGA or RMC will not be received if the VCC is not sufficient
uint32_t master_count = 0;
uint8_t master_toggle = 0;
uint8_t master_atcnt = 0;
uint8_t prev_atcnt = 0;
float Value_Buf[2];
uint16_t test_timeout=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void led_on() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
void led_off() {
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	lcd_init();
	Ringbuf_init();
	HAL_Delay(500);
	ESP_Init(WIFI_SSID, WIFI_PASSWORD);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		HAL_Delay(100);
		if (master_count++ > 10000)
			master_count = 0;
		master_toggle = (master_count / 10) % 2;
		if (master_toggle == 0)
			led_off();
		else
			led_on();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if (Wait_for("GGA", gps_uart) == 1) {
			VCCTimeout = 5000; // Reset the VCC Timeout indicating the GGA is being received
			Copy_upto("*", GGA, gps_uart);
			if (decodeGGA(GGA, &gpsData.ggastruct) == 0) {
				flagGGA = 2;  // 2 indicates the data is valid
//				led_on();
			} else {
				flagGGA = 1;  // 1 indicates the data is invalid
//				led_off();
			}
		}
		if (Wait_for("RMC", gps_uart) == 1) {
			VCCTimeout = 5000; // Reset the VCC Timeout indicating the RMC is being received
			Copy_upto("*", RMC, gps_uart);
			if (decodeRMC(RMC, &gpsData.rmcstruct) == 0) {
				flagRMC = 2;  // 2 indicates the data is valid
//				led_on();
			} else {
				flagRMC = 1;  // 1 indicates the data is invalid
//				led_off();
			}
		}
		if ((flagGGA == 2) | (flagRMC == 2)) {
#if(0)
		//			lcd_blank();

					lcd_put_cur(0, 0);
					memset(lcdBuffer, '\0', 50);
					sprintf(lcdBuffer, "%02d:%02d:%02d, %02d%02d%02d",
							gpsData.ggastruct.tim.hour, gpsData.ggastruct.tim.min,
							gpsData.ggastruct.tim.sec, gpsData.rmcstruct.date.Day,
							gpsData.rmcstruct.date.Mon, gpsData.rmcstruct.date.Yr);
					lcd_send_string(lcdBuffer);

					lcd_put_cur(1, 0);
					memset(lcdBuffer, '\0', 50);
			#if(0)
					sprintf(lcdBuffer, "%.2f%c, %.2f%c  ",
							gpsData.ggastruct.lcation.latitude,
							gpsData.ggastruct.lcation.NS,
							gpsData.ggastruct.lcation.longitude,
							gpsData.ggastruct.lcation.EW);
			#else
					sprintf(lcdBuffer, "%.4f %.4f ",
							gpsData.ggastruct.lcation.test1,
							gpsData.ggastruct.lcation.test2);
			#endif
					lcd_send_string(lcdBuffer);
#else
			//			lcd_blank();

			memset(lcdBuffer, '\0', 50);
			lcd_put_cur(0, 0);
			sprintf(lcdBuffer, "%.6f %c    ", gpsData.ggastruct.lcation.test1,
					gpsData.ggastruct.lcation.NS);
			lcd_send_string(lcdBuffer);

			memset(lcdBuffer, '\0', 50);
			lcd_put_cur(1, 0);
			sprintf(lcdBuffer, "%.6f %c    ", gpsData.ggastruct.lcation.test2,
					gpsData.ggastruct.lcation.EW);
			lcd_send_string(lcdBuffer);
#endif
		} else if ((flagGGA == 1) | (flagRMC == 1)) {
			// Instead of clearing the display, it's better if we print spaces.
			// This will avoid the "refreshing" part
			//			lcd_blank();
			lcd_put_cur(0, 0);
			lcd_send_string("   NO FIX YET   ");
			lcd_put_cur(1, 0);
			lcd_send_string("   Please wait  ");
		}
		if (VCCTimeout <= 0) {
			VCCTimeout = 5000;  // Reset the timeout
			//reset flags
			flagGGA = flagRMC = 0;
			// You are here means the VCC is less, or maybe there is some connection issue
			// Check the VCC, also you can try connecting to the external 5V
			//			lcd_blank();
			lcd_put_cur(0, 0);
			lcd_send_string("    VCC Issue   ");
			lcd_put_cur(1, 0);
			lcd_send_string("Check Connection");
		}
		// Send AT commands, feed data to Thingspeak server
		master_atcnt = master_count / 10 / 15;
#define check1 ((flagGGA==2)|(flagRMC==2))&(prev_atcnt < master_atcnt)
		if (check1) {
			Value_Buf[0] = gpsData.ggastruct.lcation.test1;
			Value_Buf[1] = gpsData.ggastruct.lcation.test2;
			ESP_Send_Multi(API_KEY, 2, Value_Buf);
		}
		prev_atcnt = master_atcnt;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
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
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  huart1.Init.BaudRate = 9600;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
