/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SHLD_D13_Pin GPIO_PIN_5
#define SHLD_D13_GPIO_Port GPIOA
#define SHLD_D12_Pin GPIO_PIN_6
#define SHLD_D12_GPIO_Port GPIOA
#define SHLD_D11_Pin GPIO_PIN_7
#define SHLD_D11_GPIO_Port GPIOA
#define SHLD_D10_Pin GPIO_PIN_6
#define SHLD_D10_GPIO_Port GPIOB

#define MOV  32 // define MOV opcode
#define WAIT  64 // define WAIT opcode
#define LOOP  128 // define LOOP opcode
#define END_LOOP 160 // define END_LOOP opcode
#define RECIPE_END  0 //  define RECIPE_END opcode
#define S1  96 // define EXTRA 1 opcode
#define S2  192 // define EXTRA 2 opcode
#define S3  224 // define EXTRA 3 opcode
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
DMA_HandleTypeDef hdma_tim2_ch1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//int servo = TIM2->CCR1;
char arr[];
char err[];
char arr1[];
uint8_t rx_buff[2];
char q;
int l, sum1, sum2;
int timer_val;
int *s1;
int *s2;

uint8_t recipe1[] = {MOV|0, MOV|5, MOV|0, MOV|3, LOOP|0, MOV|1, MOV|4, END_LOOP, MOV|0, MOV|2, WAIT|0, MOV|3, WAIT|0, MOV|3, MOV|0, WAIT|31, WAIT|31, WAIT|31, MOV|3}; //Recipe with delay
uint8_t recipe2[] = {MOV|0, MOV|1, LOOP|3, MOV|0,  MOV|5, END_LOOP, MOV|3, RECIPE_END, MOV|0 }; // Recipe with a loop
uint8_t recipe3[] = {LOOP|3, MOV|1, MOV|2, LOOP|2, MOV|3,END_LOOP, MOV|4, MOV|5, RECIPE_END, MOV|3 }; // Recipe with loop error
uint8_t recipe4[] = {MOV|0, MOV|5, MOV|0, MOV|3, LOOP|0, MOV|1, MOV|4, END_LOOP, MOV|0, MOV|2, WAIT|0, MOV|3, WAIT|0, MOV|2, MOV|3, MOV|4, MOV|2,MOV|5,MOV|1,MOV|0}; // Like recipe 1 without delay
uint8_t recipe5[] = {S1|3, RECIPE_END}; // Recipe with extra opcode
uint8_t recipe6[] = {MOV|0,MOV|1,MOV|2,MOV|3,MOV|4,MOV|5}; // General recipe to test all positions
uint8_t recipe7[] = {MOV|0,MOV|5,MOV|31}; // Recipe to test recipe error
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
  MX_TIM2_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // Starting PWM at TIM2 CH1
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // Starting PWM at TIM2 CH2

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_TIM_Base_Start_IT(&htim3);
  timer_val = __HAL_TIM_GET_COUNTER(&htim4); // Store current value of timer 4






  //get_action(recipe1);
  //htim2.Instance->CR1 &= ~TIM_CR1_CEN;


  TIM2->CCR1 = 30; // Move to position 0
  TIM2->CCR2 = 30; // Move to position 0
  HAL_Delay(1000); // A 1 second delay

  s1 = recipe1; // Pointer to recipe for servo 1
  sum1 = sizeof(recipe1)/sizeof(recipe1[0]); // Calculate size of recipe for servo 1

  s2 = recipe4; // Pointer to recipe for servo 2
  sum2 = sizeof(recipe4)/sizeof(recipe4[0]); // Calculate size of recipe for servo 2



  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  	 delay(100); // Wait for 100ms
		HAL_UART_Receive_IT (&huart2, rx_buff, 3); // Recieve input
		  get_action(s1,sum1); // Function to run servo 1
		 get_action2(s2,sum2); // Function to run servo 2
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 320-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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
  htim3.Init.Prescaler = 16000-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
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
  htim4.Init.Prescaler = 16000-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 100-1;
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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOA, SHLD_D13_Pin|SHLD_D12_Pin|SHLD_D11_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, SHLD_D10_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : SHLD_D13_Pin SHLD_D12_Pin SHLD_D11_Pin SHLD_D7_SEG7_Clock_Pin */
  GPIO_InitStruct.Pin = SHLD_D13_Pin|SHLD_D12_Pin|SHLD_D11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = SHLD_D10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SHLD_D10_GPIO_Port, &GPIO_InitStruct);



}

/* USER CODE BEGIN 4 */



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{


	if (rx_buff[2] == '\r'){  // Check if enter key is pressed

		arr1[0] = rx_buff[0];  arr1[1] = rx_buff[1]; // Store input values
	q = sprintf((char *)arr, "> %c%c\r\n", arr1[0], arr1[1]);
	user(arr1[0], arr1[1]); // Check for user commands
	arr1[0] = " ";  arr1[1] = " "; // Clear the input values
	HAL_UART_Transmit_IT (&huart2, arr, q); // Print out values on the terminal
	}
	else if(rx_buff[2] != '\r'){
		Error_Handler(); // Return error if enter is not pressed
	}
	timer_val = __HAL_TIM_GET_COUNTER(&htim4); // Reassign timer value
	 HAL_UART_Receive_IT (&huart2, rx_buff, 3);

}

void delay(uint16_t ms)
{

__HAL_TIM_SET_COUNTER(&htim3,0);
while(__HAL_TIM_GET_COUNTER(&htim3) < ms);

}

void set_led_1( uint32_t on ) // Function to use led 1
{
	if ( on ) // a5
		HAL_GPIO_WritePin(SHLD_D13_GPIO_Port, SHLD_D13_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(SHLD_D13_GPIO_Port, SHLD_D13_Pin, GPIO_PIN_SET);
}

void set_led_2( uint32_t on ) // Function to use led 2
{
	if ( on ) //a6
		HAL_GPIO_WritePin(SHLD_D12_GPIO_Port, SHLD_D12_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(SHLD_D12_GPIO_Port, SHLD_D12_Pin, GPIO_PIN_SET);
}

void set_led_3( uint32_t on ) // Function to use led 3
{
	if ( on ) //a7
		HAL_GPIO_WritePin(SHLD_D11_GPIO_Port, SHLD_D11_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(SHLD_D11_GPIO_Port, SHLD_D11_Pin, GPIO_PIN_SET);
}

void set_led_4( uint32_t on ) // Function to use led 4
{
	if ( on )
		HAL_GPIO_WritePin(SHLD_D10_GPIO_Port, SHLD_D10_Pin, GPIO_PIN_RESET);
	else
		HAL_GPIO_WritePin(SHLD_D10_GPIO_Port, SHLD_D10_Pin, GPIO_PIN_SET);
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

