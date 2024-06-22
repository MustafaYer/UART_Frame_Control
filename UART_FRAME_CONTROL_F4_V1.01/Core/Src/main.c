/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
UART_HandleTypeDef huart4;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart4_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define BUFFER_SIZE 256

#define HEADER1 ':'
#define HEADER2 'A'
#define HEADER3 '4'
#define HEADER4 '2'
#define HEADER5 '*'
#define TRAILER ';'

uint8_t rxBuffer[BUFFER_SIZE];
uint8_t circularBuffer[BUFFER_SIZE];
uint16_t writeIndex = 0;
uint8_t dataSizeStr[BUFFER_SIZE] = {0};
uint8_t dataBuffer[BUFFER_SIZE] = {0};
uint8_t dataSizeIndex = 0;
uint16_t dataIndex = 0;

int dataSize = 0;
int uart_data_index = 0;
int ham_data = 0;

uint8_t calculatedCRC = 0;
uint8_t receivedCRC = 0;
uint8_t validPacket = 1;


uint8_t CalculateCRC( uint8_t *data, uint16_t length) {

    uint8_t crc = 0;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
    }
    return crc;
}

void ProcessData(void) {

	for (int i = 0; i < BUFFER_SIZE; i++) {
		// Paket başlangıcı aranıyor
		if (circularBuffer[i] == HEADER1
				&& circularBuffer[(i + 1) % BUFFER_SIZE] == HEADER2
				&& circularBuffer[(i + 2) % BUFFER_SIZE] == HEADER3
				&& circularBuffer[(i + 3) % BUFFER_SIZE] == HEADER4
				&& circularBuffer[(i + 4) % BUFFER_SIZE] == HEADER5) {

			// Paket başlangıcı bulundu
			dataIndex = (i + 5) % BUFFER_SIZE;

			memset(dataSizeStr, 0, sizeof(dataSizeStr));
			dataSizeIndex = 0;

			while (circularBuffer[dataIndex] != HEADER5 && dataSizeIndex < 2) {
				dataSizeStr[dataSizeIndex++] = circularBuffer[dataIndex];
				dataIndex = (dataIndex + 1) % BUFFER_SIZE;
			}

			// Yıldız (*) karakteri kontrolü
			if (circularBuffer[dataIndex] != HEADER5) {
				validPacket = 0;
			}

			if (validPacket) {
				dataSizeStr[dataSizeIndex] = '\0';
				dataSize = atoi(dataSizeStr);

				// Veriyi oku
				dataIndex = (dataIndex + 1) % BUFFER_SIZE; // * karakterini atla

				for (uint16_t j = 0; j < dataSize; j++) {

					dataBuffer[j] = circularBuffer[dataIndex];
					dataIndex = (dataIndex + 1) % BUFFER_SIZE;
				}

				if (circularBuffer[dataIndex] != HEADER5) {
					validPacket = 0;
				}

				// CRC'yi oku
				dataIndex = (dataIndex + 1) % BUFFER_SIZE; // * karakterini atla

				receivedCRC = circularBuffer[dataIndex];
				dataIndex = (dataIndex + 1) % BUFFER_SIZE;

				// Sonlandırıcı karakteri kontrol et
				if (circularBuffer[dataIndex] == TRAILER) {
					// CRC'yi hesapla ve kontrol et
					calculatedCRC = CalculateCRC(dataBuffer, dataSize);

					if (calculatedCRC == receivedCRC) {
						// Veri doğru iletilmiş
						ham_data = atoi((char*) dataBuffer);
						validPacket = 1; // Paket geçerli

					}
				} else {
					validPacket = 0;
				}
			}
		}
	}
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {

    if (huart->Instance == UART4) {
        // Gelen veriyi dönen buffer'a yaz
    	 HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rxBuffer, BUFFER_SIZE);
		for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
			circularBuffer[i] = rxBuffer[i];
			if (writeIndex >= BUFFER_SIZE) {
				writeIndex = 0;
			}
		}

        // Gelen veriyi işle
        ProcessData();
    }
}


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
  MX_DMA_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */
//  HAL_UART_Receive_DMA(&huart4, rxBuffer, 1);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rxBuffer, BUFFER_SIZE);
//  HAL_UART_Transmit_DMA(&huart4, data_2, 10);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  data_control();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

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
  while (1)
  {
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
