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
#include <stdio.h>
#include "usart.h"
// #include <stdbool.h" - EXPERIMENTO POLLING

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

// volatile bool novoDado = false; - EXPERIMENTO POLLING

/* USER CODE END Variables */
/* Definitions for TaskSensor */
osThreadId_t TaskSensorHandle;
const osThreadAttr_t TaskSensor_attributes = {
  .name = "TaskSensor",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for TaskProcessamen */
osThreadId_t TaskProcessamenHandle;
const osThreadAttr_t TaskProcessamen_attributes = {
  .name = "TaskProcessamen",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for sensorSem */
osSemaphoreId_t sensorSemHandle;
const osSemaphoreAttr_t sensorSem_attributes = {
  .name = "sensorSem"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void TaskSensor_fun(void *argument);
void TaskProcessamento_fun(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of sensorSem */
  sensorSemHandle = osSemaphoreNew(1, 1, &sensorSem_attributes);

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

  /* creation of TaskProcessamen */
  TaskProcessamenHandle = osThreadNew(TaskProcessamento_fun, NULL, &TaskProcessamen_attributes);

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

// EXPERIMENTO SEMÁFORO:
  for(;;)
  {
	char mensagem[80];

	int tamanho = snprintf(
		mensagem,
		sizeof(mensagem),
		"Novo Dado Disponivel!\r\n");

	HAL_UART_Transmit(&huart1, (uint8_t*)mensagem, (uint16_t)tamanho, HAL_MAX_DELAY);

	osSemaphoreRelease(sensorSemHandle);

	osDelay(1000);

	// EXPERIMENTO POLLING :
//	  for(;;)
//	  {
//		char mensagem[80];
//
//		if (!novoDado)
//		{
//			novoDado = true;
//
//			int tamanho = snprintf(
//				mensagem,
//				sizeof(mensagem),
//				"Novo Dado Disponivel!\r\n");
//
//			HAL_UART_Transmit(&huart1, (uint8_t*)mensagem, (uint16_t)tamanho, HAL_MAX_DELAY);
//		}
//	    osDelay(1000);

  }
  /* USER CODE END TaskSensor_fun */
}

/* USER CODE BEGIN Header_TaskProcessamento_fun */
/**
* @brief Function implementing the TaskProcessamen thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TaskProcessamento_fun */
void TaskProcessamento_fun(void *argument)
{
  /* USER CODE BEGIN TaskProcessamento_fun */
  /* Infinite loop */

// EXPERIMENTO SEMÁFORO:
  for(;;)
  {
	osSemaphoreAcquire(sensorSemHandle, osWaitForever);
	char mensagem[80];

	int tamanho = snprintf(
		mensagem,
		sizeof(mensagem),
		"Dado trabalhado com sucesso!\r\n");

	HAL_UART_Transmit(&huart1, (uint8_t*)mensagem, (uint16_t)tamanho, HAL_MAX_DELAY);
  }

  // EXPERIMENTO POLLING:
//  for(;;)
//  {
//	char mensagem[80];
//
//	if (novoDado)
//	{
//		novoDado = false;
//
//		int tamanho = snprintf(
//			mensagem,
//			sizeof(mensagem),
//			"Dado trabalhado com sucesso!\r\n");
//
//		HAL_UART_Transmit(&huart1, (uint8_t*)mensagem, (uint16_t)tamanho, HAL_MAX_DELAY);
//	}
//	osDelay(1000);

  /* USER CODE END TaskProcessamento_fun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

