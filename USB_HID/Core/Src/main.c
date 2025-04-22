/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_custom_hid_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
unsigned char USB_Recive_Buffer[64]; 
unsigned char USB_Received_Count = 0;


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t buff[64] ={0};
void test_circular_buffer();
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
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
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  for(uint8_t j=0;j<64;j++)
  {
	  buff[j]=j;
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("开始测试环形缓冲区\n");
  test_circular_buffer();
  printf("测试完成\n");
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		  
    DEBUG_PRINT("circle_range_test\n");
      HAL_Delay(1000);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void test_circular_buffer() {
  circular_buffer cb;
  size_t length_to_read;
  size_t written_len1, written_len2; // 用于记录实际写入长度
  bool result; // 用于存储函数调用结果

  const size_t buffer_size = 16; // Buffer size (power of 2 recommended)
  char write_data1[] = "Msg1."; // 第一个消息
  char write_data2[] = "SecondMsg."; // 第二个消息
  char read_buffer[buffer_size + 1]; // Read buffer (+1 for null terminator)

  printf("--- Circular Buffer Test: Multiple Messages ---\n");

  // 1. 初始化
  if (!circular_buffer_init(&cb, buffer_size)) {
      printf("FAIL: Failed to initialize circular buffer\n");
      return;
  }
  printf("PASS: Initialization successful. Size: %zu\n", buffer_size);

  // 2. 写入第一个消息
  written_len1 = strlen(write_data1);
  printf("Writing message 1: '%s' (%zu bytes)\n", write_data1, written_len1);
  result = circular_buffer_write(&cb, write_data1, written_len1);
  if (result) {
      printf("PASS: Write 1 successful. Current length: %zu\n", circular_buffer_length(&cb));
  } else {
      printf("FAIL: Write 1 failed.\n");
      circular_buffer_free(&cb);
      return;
  }

  // 3. 写入第二个消息
  written_len2 = strlen(write_data2);
  printf("Writing message 2: '%s' (%zu bytes)\n", write_data2, written_len2);
  result = circular_buffer_write(&cb, write_data2, written_len2);
   if (result) {
      printf("PASS: Write 2 successful. Current length: %zu\n", circular_buffer_length(&cb));
  } else {
      // 如果第二次写入失败（例如缓冲区空间不足），打印警告但继续测试读取
      printf("WARN: Write 2 failed (Buffer might be full). Current length: %zu\n", circular_buffer_length(&cb));
      // 根据您的环形缓冲区实现，如果写入失败不应改变长度，这里可能需要调整
      // 如果部分写入是可能的，需要更复杂的处理来知道实际写入了多少
  }

  // 4. 读取第一个消息
  length_to_read = written_len1; // 尝试读取第一个消息的长度
  printf("Reading message 1 (%zu bytes)\n", length_to_read);
  memset(read_buffer, 0, sizeof(read_buffer)); // 清空读取缓冲区
  result = circular_buffer_read(&cb, read_buffer, length_to_read);
  if (result) {
      read_buffer[length_to_read] = '\0'; // Null terminate
      printf("PASS: Read 1 successful. Data: '%s'. Remaining length: %zu\n", read_buffer, circular_buffer_length(&cb));
      if (strcmp(read_buffer, write_data1) != 0) {
          printf("FAIL: Data verification for message 1 failed!\n");
      }
  } else {
      printf("FAIL: Read 1 failed. Remaining length: %zu\n", circular_buffer_length(&cb));
  }

  // 5. 读取第二个消息
  // 注意：如果第二次写入失败或部分成功，这里的读取长度需要调整
  // 这个示例假设第二次写入完全成功
  length_to_read = written_len2; // 尝试读取第二个消息的长度
  printf("Reading message 2 (%zu bytes)\n", length_to_read);
  memset(read_buffer, 0, sizeof(read_buffer)); // 清空读取缓冲区
  result = circular_buffer_read(&cb, read_buffer, length_to_read);
   if (result) {
      read_buffer[length_to_read] = '\0'; // Null terminate
      printf("PASS: Read 2 successful. Data: '%s'. Remaining length: %zu\n", read_buffer, circular_buffer_length(&cb));
       if (strcmp(read_buffer, write_data2) != 0) {
          printf("FAIL: Data verification for message 2 failed!\n");
      }
  } else {
      // 如果第二次写入失败，那么这里读取失败是正常的
      printf("INFO/FAIL: Read 2 failed (Maybe expected if write 2 failed). Remaining length: %zu\n", circular_buffer_length(&cb));
  }

  // 6. 检查缓冲区最终是否为空
  printf("Final check: Is buffer empty? %s\n", circular_buffer_is_empty(&cb) ? "Yes" : "No");
  if (!circular_buffer_is_empty(&cb)) {
       printf("WARN: Buffer is not empty at the end. Length: %zu\n", circular_buffer_length(&cb));
       // 可以尝试读取剩余数据以清空
       // size_t remaining = circular_buffer_length(&cb);
       // circular_buffer_read(&cb, read_buffer, remaining);
  }


  // 7. 释放缓冲区
  circular_buffer_free(&cb);
  printf("Circular buffer freed\n");
  printf("--- Test Finished ---\n");
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
