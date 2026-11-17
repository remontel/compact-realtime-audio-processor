/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "u8g2.h"
#include <stdio.h>

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
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
/* Isolated bring-up only: inspect while halted in the debugger. */
volatile uint32_t bringup_heartbeat = 0U;
static uint32_t heartbeat_last_tick = 0U;
static u8g2_t oled; // display configuration and driver state for U8g2

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Carry out U8g2's SPI requests using the F411's configured SPI1. */
static uint8_t OLED_U8g2Byte(u8x8_t *display, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  /* U8g2 passes its display object. This callback uses fixed board pins. */
  (void)display;

  switch (msg)
  {
    case U8X8_MSG_BYTE_INIT:
      /* CubeMX initializes SPI1 and GPIO before U8g2 starts. */
      HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
      break;

    case U8X8_MSG_BYTE_START_TRANSFER:
      /* Select the OLED before the first SPI clock pulse. */
      HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
      HAL_Delay(1);
      break;

    case U8X8_MSG_BYTE_SET_DC:
      /* For this message, arg_int is the pin level U8g2 requests. */
      HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int == 0U ? GPIO_PIN_RESET : GPIO_PIN_SET);
      break;

    case U8X8_MSG_BYTE_SEND:
      /* For this message, arg_ptr is the data and arg_int is its length. */
      if (HAL_SPI_Transmit(&hspi1, arg_ptr, arg_int, 100U) != HAL_OK)
      {
        /* Deselect the panel and stop so we can inspect a failed send. */
        HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
        Error_Handler();
        return 0U;
      }
      break;

    case U8X8_MSG_BYTE_END_TRANSFER:
      /* Keep CS low briefly after the last byte, then release it. */
      HAL_Delay(1);
      HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
      break;

    default:
      return 0U;
  }

  return 1U;
}

/* Handle U8g2's reset pin and millisecond delay requests on the F411. */
static uint8_t OLED_U8g2GpioDelay(u8x8_t *display, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  /* Keep U8g2's required function signature. These requests use msg and arg_int. */
  (void)display;
  (void)arg_ptr;

  switch (msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      /* main() runs the CubeMX GPIO and HAL setup before starting U8g2. */
      break;

    case U8X8_MSG_GPIO_RESET:
      /* arg_int is the requested level on PB1, the OLED reset wire. */
      HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, arg_int == 0U ? GPIO_PIN_RESET : GPIO_PIN_SET);
      break;

    case U8X8_MSG_DELAY_MILLI:
      /* arg_int is how many milliseconds U8g2 requests. */
      HAL_Delay(arg_int);
      break;

    default:
      /* Report that this board callback does not handle the request. */
      return 0U;
  }

  return 1U;
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
  MX_SPI1_Init();

  /* USER CODE BEGIN 2 */

  /* Select a 128×64 SSD1309 preset and give U8g2 the board functions. */
  u8g2_Setup_ssd1309_128x64_noname0_f(&oled, U8G2_R0, OLED_U8g2Byte, OLED_U8g2GpioDelay);

  /* Reset the controller and send the preset's configuration commands. */
  u8g2_InitDisplay(&oled);

  /* Begin the first visible test with a low contrast setting. */
  u8g2_SetContrast(&oled, 16U);

  /* Prepare a blank image in STM32 RAM. */
  u8g2_ClearBuffer(&oled);

  /* Select a font and position text relative to its top edge. */
  u8g2_SetFont(&oled, u8g2_font_6x10_tf);
  u8g2_SetFontPosTop(&oled);

  /* Draw a label and an off-center square into the RAM image. */
  u8g2_DrawStr(&oled, 4U, 4U, "COMPACT");
  u8g2_DrawStr(&oled, 4U, 16U, "REALTIME");
  u8g2_DrawStr(&oled, 4U, 28U, "AUDIO");
  u8g2_DrawStr(&oled, 4U, 40U, "PROCESSOR");
  u8g2_DrawBox(&oled, 116U, 40U, 8U, 8U);

  /* Copy the prepared image to the OLED while its output is off. */
  u8g2_SendBuffer(&oled);

  /* Disable power-saving mode so the OLED displays the image. */
  u8g2_SetPowerSave(&oled, 0U);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    const uint32_t now = HAL_GetTick();
    if ((uint32_t)(now - heartbeat_last_tick) >= 1000U)
    {
      heartbeat_last_tick = now;
      ++bringup_heartbeat;

      /* Hold up to 10 decimal digits plus the terminating zero byte. */
      char count_text[16];

      /* Convert the heartbeat value into a decimal text string. */
      snprintf(count_text, sizeof(count_text), "%lu", (unsigned long)bringup_heartbeat);

      /* Prepare a fresh image so previous digits are erased. */
      u8g2_ClearBuffer(&oled);

      /* Draw using the font and top positioning selected during startup. */
      u8g2_DrawStr(&oled, 4U, 4U, "Count");
      u8g2_DrawStr(&oled, 4U, 20U, count_text);

      /* Transfer the updated image to the OLED. */
      u8g2_SendBuffer(&oled);
    }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, OLED_DC_Pin|OLED_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : OLED_CS_Pin */
  GPIO_InitStruct.Pin = OLED_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OLED_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OLED_DC_Pin OLED_RST_Pin */
  GPIO_InitStruct.Pin = OLED_DC_Pin|OLED_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
#ifdef USE_FULL_ASSERT
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
