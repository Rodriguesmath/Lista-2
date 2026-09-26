/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for TaskSensor */
osThreadId_t TaskSensorHandle;
const osThreadAttr_t TaskSensor_attributes = {
  .name = "TaskSensor",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskControle */
osThreadId_t TaskControleHandle;
const osThreadAttr_t TaskControle_attributes = {
  .name = "TaskControle",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for uartMutex */
osMutexId_t uartMutexHandle;
const osMutexAttr_t uartMutex_attributes = {
  .name = "uartMutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TaskSensor_fun(void *argument);
void TaskControle_fun(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of uartMutex */
  uartMutexHandle = osMutexNew(&uartMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of TaskSensor */
  TaskSensorHandle = osThreadNew(TaskSensor_fun, NULL, &TaskSensor_attributes);

  /* creation of TaskControle */
  TaskControleHandle = osThreadNew(TaskControle_fun, NULL, &TaskControle_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_TaskSensor_fun */
/**
  * @brief  Function implementing the TaskSensor thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_TaskSensor_fun */
void TaskSensor_fun(void *argument)
{
  /* USER CODE BEGIN TaskSensor_fun */
  /* Infinite loop */

	// int cicloSensor = 0; - EXPRIMENTO TESTE

  for(;;)
  {
	osMutexAcquire(uartMutexHandle, osWaitForever); // EXPERIMENTO COM MUTEX
	// cicloSensor++; EXPERIMENTO TESTE

	for (int i = 0; i < 50; i++)
	{
	  char msg[] = "SENSOR...\r\n";
	  // int tamanho = snprintf(msg, sizeof(msg), "[Ciclo %d] SENSOR...\r\n", cicloSensor); EXPERIMENTO TESTE
	  HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg)-1, HAL_MAX_DELAY);
	  //HAL_UART_Transmit(&huart1, (uint8_t*)msg, (uint16_t)tamanho, HAL_MAX_DELAY); EXPERIMENTO TESTE
	}

	osMutexRelease(uartMutexHandle);

	osDelay(1000);
  }
  /* USER CODE END TaskSensor_fun */
}

/* USER CODE BEGIN Header_TaskControle_fun */
/**
* @brief Function implementing the TaskControle thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TaskControle_fun */
void TaskControle_fun(void *argument)
{
  /* USER CODE BEGIN TaskControle_fun */
  /* Infinite loop */
	// 	int cicloControle = 0; - EXPERIMENTO TESTE
  for(;;)
  {
	osMutexAcquire(uartMutexHandle, osWaitForever); // EXPERIMENTO COM MUTEX
	//cicloControle++; EXPERIMENTO TESTE

	for (int i = 0; i < 50; i++)
	{
	  char msg[] = "CONTROLE...\r\n";
	  //int tamanho = snprintf(msg, sizeof(msg), "[Ciclo %d] CONTROLE...\r\n", cicloControle); EXPERIMENTO TESTE
	  HAL_UART_Transmit(&huart1, (uint8_t*)msg, sizeof(msg)-1, HAL_MAX_DELAY);
      //HAL_UART_Transmit(&huart1, (uint8_t*)msg, (uint16_t)tamanho, HAL_MAX_DELAY); EXPERIMENTO TESTE
	}

	osMutexRelease(uartMutexHandle);

	osDelay(1000);
  }
  /* USER CODE END TaskControle_fun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

